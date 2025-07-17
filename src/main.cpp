#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <memory>

#if __cplusplus >= 201703L
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "config.hpp"
#include "vhardware/cpu.hpp"
#include "vhardware/device_factory.hpp"

// Include the debug framework
#include "debug/logger.hpp"
#include "debug/gui.hpp"

// Include the test framework
#include "test/test.hpp"
#include "test/test_framework.hpp"

// Include the assembler framework
#include "assembler/virtcomp_assembler.hpp"
#include "assembler/lexer.hpp"
#include "assembler/parser.hpp"
#include "assembler/assembler.hpp"

// For POSIX process execution instead of system()
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


using Logging::Logger;

class ArgParser;

void initialize_devices() {
    using namespace vhw;

    DeviceManager::instance().reset();  // Reset device manager to clear any previous state
    auto console = DeviceFactory::createConsoleDevice(0x01);  // Console on port 0x01

    auto counter = DeviceFactory::createCounterDevice(0x02);  // Counter on port 0x02

    // Set up initial counter value (optional)
    counter->setCounter(42);

    // Create a file device for virtual file I/O
    auto file = DeviceFactory::createFileDevice("virtual_storage/vhd.dat", 0x04);

    // Create a RAM disk device for block storage
    auto ramdisk = DeviceFactory::createRamDiskDevice(8192, 0x05, 0x06);

    // Optionally, create a real serial port device if available
    // Uncomment and modify the port name as needed for your system
    // auto serial = DeviceFactory::createSerialPortDevice("/dev/ttyUSB0", 0x03);

    Logger::instance().info() << "Device system initialized with standard and storage devices" << std::endl;
}

enum class ArgType { Value, Action };

struct ArgDef {
    std::string name;
    std::string arg;
    std::string alias;
    std::string help;
    ArgType type;
    std::function<void(const std::string&)> value_action; // For value args
    std::function<void()> action;                         // For action args
};

class ArgParser {
public:
    void add_value_arg(const std::string& name, const std::string& arg, const std::string& alias,
                       const std::string& help, std::function<void(const std::string&)> value_action) {
        args_.push_back({name, arg, alias, help, ArgType::Value, value_action, nullptr});
    }
    void add_action_arg(const std::string& name, const std::string& arg, const std::string& alias,
                        const std::string& help, std::function<void()> action) {
        args_.push_back({name, arg, alias, help, ArgType::Action, nullptr, action});
    }
    void add_bool_arg(const std::string& name, const std::string& arg, const std::string& alias,
                      const std::string& help, std::function<void(bool)> action) {
        args_.push_back({name, arg, alias, help, ArgType::Value,
            [action](const std::string& value) {
                // If value is empty, treat as true (flag style)
                if (value.empty()) action(true);
                else action(value == "true" || value == "1");
            }, nullptr});
    }

    void parse(int argc, char* argv[]) {        for (int i = 1; i < argc; ++i) {
            std::string token = argv[i];
            bool matched = false;
            for (auto& def : args_) {
                // Check for exact match or --arg=value / -a=value format
                bool is_match = false;
                std::string value;

                if (token == def.arg || token == def.alias) {
                    is_match = true;
                } else {
                    // Check for --arg=value format
                    auto eq = token.find('=');
                    if (eq != std::string::npos) {
                        std::string arg_part = token.substr(0, eq);
                        if (arg_part == def.arg || arg_part == def.alias) {
                            is_match = true;
                            value = token.substr(eq + 1);
                        }
                    }
                }

                if (is_match) {
                    matched = true;
                    if (def.type == ArgType::Value) {
                        // If we didn't get value from =, try next argument
                        if (value.empty() && i + 1 < argc && argv[i + 1][0] != '-') {
                            value = argv[++i];
                        }
                        // If no value, value remains empty
                        if (def.value_action) def.value_action(value);
                    } else if (def.type == ArgType::Action) {
                        if (def.action) def.action();
                    }
                    break;
                }
            }
            if (!matched && token.rfind("-", 0) == 0) {
                std::cerr << "Unknown argument: " << token << std::endl;
            }
        }
    }

    void print_help() const {
        std::cout << "virtcomp Usage: virtcomp [options]" << std::endl;
        for (const auto& def : args_) {
            // Use printf to align arguments and help text
            std::cout << fmt::format("  {:<20} {:<6}  {}\n", def.arg, def.alias, def.help);
        }
    }
private:
    std::vector<ArgDef> args_;
};

bool run_tests() {
    // Print a header
    // Print a colored ASCII art header (cyan)
    // If debug mode is on, use orange (ANSI 38;5;208), else cyan (36)
    const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
    std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
    std::cout << color << "│     Running VirtComp Unit Tests                      │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;

    // Run unit tests using the new framework
    run_unit_tests();

    // Also run the old integration tests for now
    std::cout << std::endl;
    std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
    std::cout << color << "│     Running VirtComp Integration Tests               │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;

    // Use TestRunner to run all .hex files in tests/
    TestRunner runner("tests");
    auto results = runner.run_all();
    int passed = 0, failed = 0;
    // Print result header with the same style as the test header
    const char* result_color = Config::debug ? "\033[38;5;208m" : "\033[36m";
    std::cout << result_color << "┌──────────────────────────────────────────────────────┤\033[0m" << std::endl;
    std::cout << result_color << "│     VirtComp Integration Test Results                │\033[0m" << std::endl;
    std::cout << result_color << "└──────────────────────────────────────────────────────┘\033[0m" << std::endl;
    for (const auto& result : results) {
        // Print test result with neat spacing (fixed width for name)
        [[maybe_unused]] constexpr int name_width = 24;
        // ANSI color codes: green for pass, red for fail
        const char* color = result.passed ? "\033[32m" : "\033[31m";
        const char* reset = "\033[0m";
        std::cout << fmt::format("{0}[{1}]{2} {3:<28}", color, result.passed ? "/" : "X", reset, result.name);
        if ((&result - &results[0] + 1) % 4 == 0)
            std::cout << std::endl;
        else
            std::cout << "    ";
        if (result.passed) ++passed; else ++failed;
    }
    std::cout << std::endl;
    // Summary: green if all passed, yellow if some failed
    const char* summary_color = (failed == 0) ? "\033[32m" : "\033[33m";
    std::cout << summary_color << "Integration tests passed: " << passed << " / " << results.size() << "\033[0m" << std::endl;
    exit(0);
}

void run_gui() {
    std::vector<uint8_t> program;
    if (!Config::program_file.empty()) {
        std::ifstream file(Config::program_file);
        if (!file) {
            std::cerr << "Failed to load program file: " << Config::program_file << std::endl;
            exit(1);
        }
        std::string token;
        while (file >> token) {
            if (token[0] == '#') { file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                program.push_back(byte);
            } catch (...) {
                std::cerr << "Invalid hex byte in program file: " << token << std::endl;
                exit(1);
            }
        }
    }
    Gui gui("VirtComp Debugger");
    gui.run_vm(program);
    exit(0);
}

class VirtComp {
public:
    VirtComp(int argc, char *argv[]) {
        // Help argument
        parser.add_action_arg("help", "--help", "-h", "Shows help information",
            [this]() { parser.print_help(); show_help = true; });
        // Debug argument
        parser.add_bool_arg("debug", "--debug", "-d", "Enable debug mode",
            [this](bool value) { Config::debug = value; Config::verbose = value; });
        // Verbose argument
        parser.add_bool_arg("verbose", "--verbose", "-v", "Show informational messages (use --verbose=false to disable)",
            [this](bool value) { Config::verbose = value; });

        // Extended registers argument
        parser.add_bool_arg("extended_registers", "--extended-registers", "-er", "Show extended register output (50 registers)",
            [this](bool value) { Config::extended_registers = value; });

        // Debug File argument
        parser.add_value_arg("debug_file", "--debug-file", "-f", "Debug file path",
            [this](const std::string& value) { Config::debug_file = value; });

        // Hex file argument
        parser.add_value_arg("hex", "--hex", "-H", "Path to hex file (hex bytes, space or newline separated)",
            [this](const std::string& value) { Config::program_file = value; });

        // Run tests argument
        parser.add_bool_arg("test", "--test", "-t", "Run tests",
            [this](bool value) { Config::running_tests = value; });

        // Gui argument
        parser.add_action_arg("gui", "--gui", "-g", "Enable debug GUI",
            [this]() { run_gui(); });

        // Assembly mode argument
        parser.add_value_arg("assembly", "--assembly", "-A", "Assembly mode: assemble and run .asm file",
            [this](const std::string& value) {
                Config::assembly_mode = true;
                Config::assembly_file = value;
            });

        // Compile argument
        parser.add_value_arg("compile", "--compile", "-o", "Compile program into a standalone executable (optionally specify output name)",
            [this](const std::string& value) {
                Config::compile_only = true;
                Config::output_name = value;
            });

        parser.parse(argc, argv);
    }

    // Run in compiled mode (create a standalone executable)
    void run_compiled(std::vector<uint8_t>& program) {
        // Create a standalone executable instead of running the program
        std::string output_name;

        if (Config::output_name.empty()) {
            // Generate a name if none provided
            output_name = generate_executable_name(Config::program_file);
        } else {
            // Use provided name and sanitize it
            output_name = sanitize_filename(Config::output_name);
            if (output_name.empty()) {
                std::cerr << "Error: Invalid output filename: " << Config::output_name << std::endl;
                std::cerr << "Filename cannot contain: . at start, ../, or shell metacharacters ;|&`$()[]{}*?<>" << std::endl;
                return;
            }

            // Create directory if it doesn't exist
            fs::path output_path(output_name);
            if (output_path.has_parent_path()) {
                fs::path dir = output_path.parent_path();
                if (!fs::exists(dir)) {
                    if (!fs::create_directories(dir)) {
                        std::cerr << "Error: Failed to create directory: " << dir << std::endl;
                        return;
                    }
                }
            }
        }
        if (create_standalone_executable(program, output_name)) {
            std::string shown_name = output_name;
            if (shown_name.substr(0, 2) != "./") {
                shown_name = "./" + shown_name;
            }
            std::cout << "Successfully compiled to executable: " << shown_name << std::endl;

            // Don't add ./ prefix if output_name already starts with ./
            std::string run_command = output_name;
            if (run_command.substr(0, 2) != "./") {
                run_command = "./" + run_command;
            }
            std::cout << "You can run it with: " << run_command << std::endl;
        }
    }

    // Generate a suitable executable name from the program file path
    std::string generate_executable_name(const std::string& program_file) {
        fs::path path(program_file);
        std::string name = path.stem().string();  // Get filename without extension

        // Make sure we have the bin directory
        fs::path bin_dir("bin");
        if (!fs::exists(bin_dir)) {
            fs::create_directory(bin_dir);
        }

        return (bin_dir / name).string();
    }

    // Sanitize filename to prevent command injection
    std::string sanitize_filename(const std::string& filename) {
        // First check for completely invalid patterns
        if (filename.empty() ||
            filename.find("../") != std::string::npos ||
            filename.find_first_of(";|&`$()[]{}*?<>") != std::string::npos) {
            return "";
        }

        // Check for hidden files (starting with . but not ./ which is current directory)
        if (filename[0] == '.' && filename.length() > 1 && filename[1] != '/') {
            return "";
        }

        std::string sanitized;
        sanitized.reserve(filename.length());

        for (char c : filename) {
            // Allow alphanumeric, dots, hyphens, underscores, and forward slashes for paths
            if (std::isalnum(c) || c == '.' || c == '-' || c == '_' || c == '/') {
                sanitized += c;
            }
            // Replace other characters with underscore
            else {
                sanitized += '_';
            }
        }

        return sanitized;
    }

    // Create a standalone executable with the program embedded
    bool create_standalone_executable(const std::vector<uint8_t>& program,
                                      const std::string& output_name) {
        // Note: output_name is already sanitized by run_compiled()

        // 1. Generate program_data.hpp with the program bytes
        if (!generate_program_data_header(program)) {
            std::cerr << "Failed to generate program data header" << std::endl;
            return false;
        } else {
            std::cout << "Generated program data header with " << program.size() << " bytes" << std::endl;
        }

        // 2. Compile the standalone main
        if (!compile_standalone_main(output_name)) {
            std::cerr << "Failed to compile standalone executable" << std::endl;
            return false;
        } else {
            std::cout << "Compiled standalone main to: " << output_name << std::endl;
        }

        return true;
    }

    // Generate program_data.hpp with the program bytes
    bool generate_program_data_header(const std::vector<uint8_t>& program) {
        std::ofstream outfile("src/program_data.hpp");
        if (!outfile) {
            std::cerr << "Error: Cannot create program_data.hpp" << std::endl;
            return false;
        }

        // Get current time for timestamp
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

        // Write the header file
        outfile << "// Auto-generated from program file\n";
        outfile << "// Generated on: " << timestamp.str() << "\n\n";
        outfile << "#ifndef PROGRAM_DATA_HPP\n";
        outfile << "#define PROGRAM_DATA_HPP\n\n";
        outfile << "#include <vector>\n";
        outfile << "#include <cstdint>\n\n";
        outfile << "// Program binary data\n";
        outfile << "const std::vector<uint8_t> PROGRAM_DATA = {\n    ";

        // Write the program bytes in a nicely formatted array
        const int ITEMS_PER_LINE = 12;
        for (size_t i = 0; i < program.size(); ++i) {
            outfile << "0x" << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(program[i]);

            if (i < program.size() - 1) {
                outfile << ", ";
                if ((i + 1) % ITEMS_PER_LINE == 0) {
                    outfile << "\n    ";
                }
            }
        }

        outfile << "\n};\n\n";
        outfile << "#endif // PROGRAM_DATA_HPP\n";

        return true;
    }

    // Securely compile standalone executable using execvp (no system())
    bool compile_standalone_main(const std::string& output_name) {
        // Create a simplified standalone main that doesn't include imgui
        std::string temp_file = "build/tmp_standalone.cpp";

        // Write a simplified version of standalone_main.cpp to a temporary file
        std::ofstream outfile(temp_file);
        if (!outfile) {
            std::cerr << "Error: Cannot create temporary file" << std::endl;
            return false;
        }

        outfile << "#include <iostream>\n";
        outfile << "#include <vector>\n";
        outfile << "#include <cstdint>\n";
        outfile << "#include \"config.hpp\"\n";
        outfile << "#include \"vhardware/cpu.hpp\"\n";
        outfile << "#include \"vhardware/device_factory.hpp\"\n\n";
        outfile << "// Include the generated program data\n";
        outfile << "#include \"program_data.hpp\"\n\n";
        outfile << "// Basic device initialization without logging\n";
        outfile << "void silent_initialize_devices() {\n";
        outfile << "    using namespace vhw;\n";
        outfile << "    auto console = DeviceFactory::createConsoleDevice(0x01);\n";
        outfile << "    auto counter = DeviceFactory::createCounterDevice(0x02);\n";
        outfile << "    auto file = DeviceFactory::createFileDevice(\"virtual_storage/vhd.dat\", 0x04);\n";
        outfile << "    auto ramdisk = DeviceFactory::createRamDiskDevice(8192, 0x05, 0x06);\n";
        outfile << "}\n\n";
        outfile << "int main(int, char**) {\n";
        outfile << "    // No debug mode by default\n";
        outfile << "    Config::debug = false;\n";
        outfile << "    Config::verbose = false;\n\n";
        outfile << "    // Initialize CPU\n";
        outfile << "    CPU cpu;\n";
        outfile << "    cpu.reset();\n\n";
        outfile << "    // Initialize devices silently\n";
        outfile << "    silent_initialize_devices();\n\n";
        outfile << "    // Execute the program\n";
        outfile << "    cpu.execute(PROGRAM_DATA);\n\n";
        outfile << "    // Handle errors if any\n";
        outfile << "    if (Config::error_count > 0) {\n";
        outfile << "        std::cerr << \"Execution failed with \" << Config::error_count << \" errors.\" << std::endl;\n";
        outfile << "        return 1;\n";
        outfile << "    }\n";
        outfile << "    return 0;\n";
        outfile << "}\n";
        outfile.close();

        // Create bin directory if it doesn't exist
        fs::path bin_dir("bin");
        if (!fs::exists(bin_dir)) {
            fs::create_directory(bin_dir);
        }        // Propietary files
        // Note: fmt library is included in extern/fmt
        // Note: imgui not included
        std::vector<std::string> extra_files = {
            "src/debug/logger.cpp",
            "src/vhardware/cpu.cpp",
            "src/vhardware/device_manager.cpp",
            // Use consolidated opcodes file for faster compilation
            "src/vhardware/opcodes/opcodes_consolidated.cpp",
            // "src/vhardware/device_factory.cpp",
            // "src/vhardware/devices/console_device.cpp",
            // "src/vhardware/devices/counter_device.cpp",
            // "src/vhardware/devices/file_device.cpp",
            // "src/vhardware/devices/ramdisk_device.cpp",
            "extern/fmt/src/format.cc"
        };

        // Prepare argument vector
        std::vector<std::string> args = {
            "g++", "-std=c++17", "-I./src",
            "-Iextern/fmt/include", "-Iextern",
            "-std=c++17",
            "-o", output_name,
            temp_file
        };
        // Add all extra source files
        args.insert(args.end(), extra_files.begin(), extra_files.end());

        // Convert args to char* array
        std::vector<char*> c_args;
        for (auto& arg : args) c_args.push_back(const_cast<char*>(arg.c_str()));
        c_args.push_back(nullptr);

        std::cout << "Building standalone executable..." << std::endl;

        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Failed to fork for compile" << std::endl;
            return false;
        } else if (pid == 0) {
            // child process
            execvp("g++", c_args.data());
            // If execvp fails
            std::cerr << "Failed to execute g++" << std::endl;
            _exit(127);
        }
        // parent process
        int status = 0;
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "Failed during waitpid" << std::endl;
            return false;
        } else {
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            return false;
        }

        // Make the executable file executable
        fs::permissions(output_name,
            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
            fs::perm_options::add);

        return true;
    }

    void run() {
        if (show_help) return;

        // Handle test mode first
        if (Config::running_tests) {
            // Validate conflicting flags for test mode
            if (Config::assembly_mode) {
                std::cerr << "Error: Test mode (-t/--test) cannot be used with assembly mode (-A/--assembly)" << std::endl;
                return;
            }
            if (!Config::program_file.empty()) {
                std::cerr << "Error: Test mode (-t/--test) cannot be used with hex file (-H/--hex)" << std::endl;
                return;
            }
            run_tests();
            return;
        }

        // Validate conflicting flags for assembly mode
        if (Config::assembly_mode && !Config::program_file.empty()) {
            std::cerr << "Error: Assembly mode (-A/--assembly) cannot be used with hex file (-H/--hex)" << std::endl;
            return;
        }

        // Handle assembly mode
        if (Config::assembly_mode) {
            run_assembly_mode();
            return;
        }

        CPU cpu;
        cpu.reset();

        std::vector<uint8_t> program;
        if (!Config::program_file.empty()) {
            if (!load_program_file(Config::program_file, program)) {
                std::cerr << "Failed to load program file: " << Config::program_file << std::endl;
                return;
            }
        } else {
            std::cerr << "No hex file specified. Use --hex or -H to specify a hex file." << std::endl;
            return;
        }

        // Check if we should compile instead of run
        if (Config::compile_only) {
            run_compiled(program);
            return;
        }

        // Print a simple, clean headerw
        if (!Config::compile_only) {
            const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
            std::cout << color << "\n=== VirtComp Virtual Machine ===" << "\033[0m" << std::endl;
            std::cout << color << "Execution started..." << "\033[0m\n" << std::endl;
        }

        // Initialize the device system
        initialize_devices();

        cpu.execute(program);

        // Print CPU state
        cpu.print_state("End");
        cpu.print_registers();

        // Print extended registers if enabled
        if (Config::extended_registers) {
            cpu.print_extended_registers();
        }

        cpu.print_memory();

        if (Config::error_count > 0) {
            Logger::instance().error() << "Execution failed with " << Config::error_count << " errors." << std::endl;
        } else {
            Logger::instance().success() << "Execution completed successfully." << std::endl;
        }
    }

private:
    ArgParser parser;
    std::string data;
    bool show_help = false;

    // Helper to load hex bytes from file
    bool load_program_file(const std::string& path, std::vector<uint8_t>& out) {
        std::ifstream file(path);
        if (!file) return false;
        std::string token;
        while (file >> token) {
            if (token[0] == '#') { file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                out.push_back(byte);
            } catch (...) {
                std::cerr << "Invalid hex byte in program file: " << token << std::endl;
                Config::error_count++;
                return false;
            }
        }
        return true;
    }

    // Assembly mode: assemble and run .asm file
    void run_assembly_mode() {
        if (Config::assembly_file.empty()) {
            std::cerr << "Error: No assembly file specified for assembly mode (-A/--assembly)" << std::endl;
            return;
        }

        // Check if file exists and has .asm extension
        if (!fs::exists(Config::assembly_file)) {
            std::cerr << "Error: Assembly file not found: " << Config::assembly_file << std::endl;
            return;
        }

        // Load assembly source
        std::ifstream file(Config::assembly_file);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open assembly file: " << Config::assembly_file << std::endl;
            return;
        }

        std::ostringstream oss;
        oss << file.rdbuf();
        std::string assembly_source = oss.str();

        if (Config::verbose) {
            std::cout << "Assembling: " << Config::assembly_file << std::endl;
        }

        // Step 1: Lexical analysis
        Assembler::Lexer lexer(assembly_source);
        auto tokens = lexer.tokenize();

        if (lexer.has_errors()) {
            std::cerr << "Lexer errors:" << std::endl;
            for (const auto& error : lexer.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        // Step 2: Parsing
        Assembler::Parser parser(tokens);
        auto ast = parser.parse();

        if (parser.has_errors()) {
            std::cerr << "Parser errors:" << std::endl;
            for (const auto& error : parser.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        // Step 3: Code generation
        Assembler::AssemblerEngine assembler;
        auto bytecode = assembler.assemble(*ast);

        if (assembler.has_errors()) {
            std::cerr << "Assembly errors:" << std::endl;
            for (const auto& error : assembler.get_errors()) {
                std::cerr << "  " << error << std::endl;
            }
            return;
        }

        if (Config::verbose) {
            std::cout << "Assembly successful. Generated " << bytecode.size() << " bytes of bytecode." << std::endl;

            // Show symbol table if verbose
            const auto& symbols = assembler.get_symbols();
            if (!symbols.empty()) {
                std::cout << "Symbol table:" << std::endl;
                for (const auto& [name, symbol] : symbols) {
                    std::cout << "  " << name << " = 0x" << std::hex << symbol.address << std::dec << std::endl;
                }
            }
        }

        // Initialize CPU and devices
        CPU cpu;
        cpu.reset();
        initialize_devices();

        // Print header for assembled program
        if (Config::verbose) {
            std::cout << "\n\033[36m┌─────────────────────────────────────────────────────────────┐\033[0m" << std::endl;
            std::cout << "\033[36m│\033[0m               \033[1mRunning Assembled Program\033[0m                     \033[36m│\033[0m" << std::endl;
            std::cout << "\033[36m└─────────────────────────────────────────────────────────────┘\033[0m" << std::endl;
        }

        try {
            // Execute the assembled bytecode
            cpu.execute(bytecode);

            // Print CPU state and registers (same as regular program mode)
            cpu.print_state("End");
            cpu.print_registers();

            // Print extended registers if enabled
            if (Config::extended_registers) {
                cpu.print_extended_registers();
            }

            cpu.print_memory();

            if (Config::error_count > 0) {
                Logger::instance().error() << "Assembly program failed with " << Config::error_count << " errors." << std::endl;
            } else {
                Logger::instance().success() << "Assembly program completed successfully." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
            Config::error_count++;
        }
    }
};

int main(int argc, char *argv[]) {
    VirtComp app(argc, argv);
    app.run();
    return 0;
}