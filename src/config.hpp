#pragma once
#include <string>

class Config {
public:
    inline static bool debug = false;
    inline static bool verbose = false;  // Default to showing info messages
    inline static bool running_tests = false;
    inline static bool compile_only = false;  // Run without debug outputs
    inline static std::string debug_file = "debug.log";
    inline static std::string program_file = "";
    inline static int error_count = 0;
};

// Declare device initialization for use in tests and main
void initialize_devices();