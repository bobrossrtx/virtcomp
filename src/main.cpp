#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <fstream>
#include <filesystem>

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

// Function to initialize the device system
void initialize_devices() {
    using namespace vhw;

    DeviceManager::instance().reset();  // Reset device manager to clear any previous state    // Create and register standard devices
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

    void parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string token = argv[i];
            bool matched = false;
            for (auto& def : args_) {
                if (token == def.arg || token == def.alias) {
                    matched = true;
                    if (def.type == ArgType::Value) {
                        std::string value;
                        // Support --arg=value or -a value
                        auto eq = token.find('=');
                        if (eq != std::string::npos) {
                            value = token.substr(eq + 1);
                        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
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
            // Use fmt to align arguments and help text
            fmt::print("  {:<16} {:<6}  {}\n", def.arg, def.alias, def.help);
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
    std::cout << color << "│     Running VirtComp Tests                           │\033[0m" << std::endl;
    std::cout << color << "└──────────────────────────────────────────────────────┤\033[0m" << std::endl;

    // Use TestRunner to run all .hex files in tests/
    TestRunner runner("tests");
    auto results = runner.run_all();
    int passed = 0, failed = 0;
    // Print result header
    fmt::print("\033[36m┌{:─>54}┴{:─<83}┐\033[0m\n", "", "");
    // Cyan color for header
    fmt::print("\033[36m│{:>55}{}{:>62}│\033[0m\n", "", "VirtComp Test Results", "");
    fmt::print("\033[36m└{:─^{}}┘\033[0m\n", "", 138);
    for (const auto& result : results) {
        // Print test result with neat spacing (fixed width for name)
        constexpr int name_width = 24;
        // ANSI color codes: green for pass, red for fail
        const char* color = result.passed ? "\033[32m" : "\033[31m";
        const char* reset = "\033[0m";
        fmt::print("{}[{}]{} {:<28}", color, result.passed ? "/" : "X", reset, result.name);
        if ((&result - &results[0] + 1) % 4 == 0)
            std::cout << std::endl;
        else
            std::cout << "    ";
        if (result.passed) ++passed; else ++failed;
    }
    std::cout << std::endl;
    // Summary: green if all passed, yellow if some failed
    const char* summary_color = (failed == 0) ? "\033[32m" : "\033[33m";
    std::cout << summary_color << "Tests passed: " << passed << " / " << results.size() << "\033[0m" << std::endl;
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
            [this]() { parser.print_help(); show_help = true; });        // Debug argument
        parser.add_bool_arg("debug", "--debug", "-d", "Enable debug mode",
            [this](bool value) { Config::debug = value; Config::verbose = value; });        // Verbose argument
        parser.add_bool_arg("verbose", "--verbose", "-v", "Show informational messages (use --verbose=false to disable)",
            [this](bool value) { Config::verbose = value; });

        // Debug File argument
        parser.add_value_arg("debug_file", "--debug-file", "-f", "Debug file path",
            [this](const std::string& value) { Config::debug_file = value; });

        // Program file argument
        parser.add_value_arg("program", "--program", "-p", "Path to program file (hex bytes, space or newline separated)",
            [this](const std::string& value) { Config::program_file = value; });

        // Run tests argument
        parser.add_action_arg("test", "--test", "-t", "Run tests",
            [this]() { run_tests(); });

        // Gui argument
        parser.add_action_arg("gui", "--gui", "-g", "Enable debug GUI",
            [this]() { run_gui(); });

        // Compile argument
        parser.add_bool_arg("compile", "--compile", "-c", "Compile program into a standalone executable",
            [this](bool value) { Config::compile_only = value; });

        parser.parse(argc, argv);
    }

    // Run in compiled mode (create a standalone executable)
    void run_compiled(std::vector<uint8_t>& program) {
        // Create a standalone executable instead of running the program
        std::string output_name = generate_executable_name(Config::program_file);

        if (create_standalone_executable(Config::program_file, program, output_name)) {
            std::cout << "Successfully compiled to executable: " << output_name << std::endl;
            std::cout << "You can run it with: ./" << output_name << std::endl;
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

    // Create a standalone executable with the program embedded
    bool create_standalone_executable(const std::string& program_file,
                                     const std::vector<uint8_t>& program,
                                     const std::string& output_name) {
        // 1. Generate program_data.hpp with the program bytes
        if (!generate_program_data_header(program)) {
            std::cerr << "Failed to generate program data header" << std::endl;
            return false;
        }

        // 2. Compile the standalone main
        if (!compile_standalone_main(output_name)) {
            std::cerr << "Failed to compile standalone executable" << std::endl;
            return false;
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
    }    // Compile standalone executable
    bool compile_standalone_main(const std::string& output_name) {
        // First, let's create a simplified standalone main that doesn't include imgui
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
        }        // Compile directly to an executable
        // Find the CPU implementation file which we know exists
        std::string compile_cmd = "g++ -std=c++17 -I./src"
                                  " -o " + output_name +
                                  " " + temp_file +
                                  " src/vhardware/cpu.cpp"
                                  " -lfmt";

        std::cout << "Building standalone executable..." << std::endl;
        int compile_result = std::system(compile_cmd.c_str());
        if (compile_result != 0) {
            std::cerr << "Failed to compile standalone executable" << std::endl;
            return false;
        }

        // Make the executable file executable
        fs::permissions(output_name,
            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
            fs::perm_options::add);

        return true;
    }    void run() {
        if (show_help) return;

        CPU cpu;
        cpu.reset();

        std::vector<uint8_t> program;
        if (!Config::program_file.empty()) {
            if (!load_program_file(Config::program_file, program)) {
                std::cerr << "Failed to load program file: " << Config::program_file << std::endl;
                return;
            }
        } else {
            std::cerr << "No program file specified. Use --program or -p to specify a program file." << std::endl;
            return;
        }

        // Check if we should compile instead of run
        if (Config::compile_only) {
            run_compiled(program);
            return;
        }

        // Print a header
        // Print a colored ASCII art header (cyan)
        // If debug mode is on, use orange (ANSI 38;5;208), else cyan (36)
        const char* color = Config::debug ? "\033[38;5;208m" : "\033[36m";
        std::cout << color << "┌──────────────────────────────────────────────────────┐\033[0m" << std::endl;
        std::cout << color << R"(│                                                      │
│     __      ___      _                               │
│     \ \    / (_)    | |                              │
│      \ \  / / _ _ __| |_ ___ ___  _ __ ___  _ __     │
│       \ \/ / | | '__| __/ __/ _ \| '_ ` _ \| '_ \    │
│        \  /  | | |  | || (_| (_) | | | | | | |_) |   │
│         \/   |_|_|   \__\___\___/|_| |_| |_| .__/    │
│                                            | |       │
│                                            |_|       │
│                                                      │)" << "\033[0m" << std::endl;
std::cout << color << "└──────────────────────────────────────────────────────┘\033[0m" << std::endl;

        // Initialize the device system after displaying the logo
        initialize_devices();

        cpu.execute(program);

        // Print CPU state
        cpu.print_state("End");
        cpu.print_registers();
        cpu.print_memory();

        if (Config::error_count > 0) {
            Logger::instance().error() << fmt::format("Execution failed with {} errors.", Config::error_count) << std::endl;
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
};

int main(int argc, char *argv[]) {
    VirtComp app(argc, argv);
    app.run();
    return 0;
}