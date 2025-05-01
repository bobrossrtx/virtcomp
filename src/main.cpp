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

// Include the debug framework
#include "debug/logger.hpp"
#include "debug/gui.hpp"

// Include the test framework
#include "test/test.hpp"

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
    // Use TestRunner to run all .hex files in tests/
    TestRunner runner("tests");
    auto results = runner.run_all();
    int passed = 0, failed = 0;
    for (const auto& result : results) {
        // Print test result with neat spacing (fixed width for name)
        constexpr int name_width = 24;
        std::cout << "[" << (result.passed ? "/" : "X") << "] ";
        std::cout.width(name_width);
        std::cout << std::left << result.name;
        std::cout << std::right;
        if ((&result - &results[0] + 1) % 4 == 0)
            std::cout << std::endl;
        else
            std::cout << "    ";
        if (result.passed) ++passed; else ++failed;
    } std::cout << std::endl;
    std::cout << "Tests passed: " << passed << " / " << results.size() << std::endl;
    exit(0);
}

void run_gui(std::string program_file) {
    std::vector<uint8_t> program;
    if (!program_file.empty()) {
        std::ifstream file(program_file);
        if (!file) {
            std::cerr << "Failed to load program file: " << program_file << std::endl;
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
            [this](bool value) { ::debug = value; });

        // Debug File argument
        parser.add_value_arg("debug_file", "--debug-file", "-f", "Debug file path",
            [this](const std::string& value) { debug_file = value; });

        // Program file argument
        parser.add_value_arg("program", "--program", "-p", "Path to program file (hex bytes, space or newline separated)",
            [this](const std::string& value) { program_file = value; });

        // Run tests argument
        parser.add_action_arg("test", "--test", "-t", "Run tests",
            [this]() { run_tests(); });

        // Gui argument
        parser.add_action_arg("gui", "--gui", "-g", "Enable debug GUI",
            [this]() { run_gui(program_file); });
        

        parser.parse(argc, argv);
    }

    void run() {
        if (show_help) return;
        CPU cpu;
        cpu.reset();

        std::vector<uint8_t> program;
        if (!program_file.empty()) {
            if (!load_program_file(program_file, program)) {
                std::cerr << "Failed to load program file: " << program_file << std::endl;
                return;
            }
        } else {
            std::cerr << "No program file specified. Use --program or -p to specify a program file." << std::endl;
            return;
        }

        cpu.execute(program);
        cpu.print_registers();
        cpu.print_memory();

        if (error_count > 0) {
            std::cerr << "Execution failed with " << error_count << " errors." << std::endl;
        } else {
            std::cout << "Execution completed successfully." << std::endl;
        }
    }

  private:
    ArgParser parser;
    std::string data;
    bool show_help = false;
    std::string program_file;

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