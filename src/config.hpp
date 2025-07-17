#pragma once
#include <string>

class Config {
public:    inline static bool debug = false;
    inline static bool verbose = false;  // Default to showing info messages
    inline static bool running_tests = false;
    inline static bool compile_only = false;  // Run without debug outputs
    inline static bool extended_registers = false;  // Show extended register output
    inline static bool assembly_mode = false;  // Assembly mode enabled
    inline static std::string debug_file = "debug.log";
    inline static std::string program_file = "";
    inline static std::string assembly_file = "";  // Assembly source file
    inline static std::string output_name = "";  // Output name for compiled executable
    inline static int error_count = 0;
};

// Declare device initialization for use in tests and main
void initialize_devices();