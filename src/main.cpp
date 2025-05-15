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

    // Create and register standard devices
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
            std::cout << def.arg << ", " << def.alias << "\t" << def.help << std::endl;
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
    fmt::print("\033[36m{:─>55}┴{:─<69}\033[0m\n", "", "");
    // Cyan color for header
    std::cout << "\033[36mVirtComp Test Results\033[0m" << std::endl;
    fmt::print("\033[36m{:─^{}}\033[0m\n", "", 125);

    for (const auto& result : results) {
        // Print test result with neat spacing (fixed width for name)
        constexpr int name_width = 24;
        // ANSI color codes: green for pass, red for fail
        const char* color = result.passed ? "\033[32m" : "\033[31m";
        const char* reset = "\033[0m";
        std::cout << color << "[" << (result.passed ? "/" : "X") << "] " << reset;
        std::cout.width(name_width);
        std::cout << std::left << result.name;
        std::cout << std::right;
        std::cout << std::setw(4) << std::setfill(' ') << "";
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


        parser.parse(argc, argv);
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