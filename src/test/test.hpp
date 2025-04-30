#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "../vhardware/cpu.hpp"

class TestRunner {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
    };

    TestRunner(const std::string& test_dir = "tests") : test_dir_(test_dir) {}

    std::vector<TestResult> run_all() {
        std::vector<TestResult> results;
        for (const auto& entry : std::filesystem::directory_iterator(test_dir_)) {
            if (entry.path().extension() == ".hex") {
                std::string test_name = entry.path().filename().string();
                Logger::instance().info() << std::right << std::setw(26) << std::setfill(' ') << "[RUN] | " << test_name << std::endl;
                TestResult result = run_test(entry.path());
                std::ostringstream oss;

                int buffer_size = 15;
                if (!result.passed) buffer_size += 2;

                oss << std::right << std::setw(buffer_size) << std::setfill(' ') << 
                "[" << (result.passed ? "PASS" : "FAIL") << "] | " << test_name;
                if (!result.passed && !result.message.empty())
                    oss << " -- " << result.message;
                    
                if (result.passed) {
                    Logger::instance().success() << oss.str() << std::endl;
                } else {
                    Logger::instance().error() << oss.str() << std::endl;
                }

                results.push_back(result);
            }
        }
        return results;
    }

private:
    std::string test_dir_;

    TestResult run_test(const std::filesystem::path& path) {
        std::vector<uint8_t> prog;
        std::ifstream file(path);
        std::string token;
        std::string comment;
        while (file >> token) {
            if (token[0] == '#') {
                // Read the rest of the line as a comment
                std::string rest_of_line;
                std::getline(file, rest_of_line);
                if (!comment.empty()) comment += "";
                comment += rest_of_line;
                continue;
            }
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                prog.push_back(byte);
            } catch (...) {
                return {path.filename().string(), false, "Invalid hex byte: " + token};
            }
        }

        if (!comment.empty()) {
            Logger::instance().info() << std::right << std::setw(23) << std::setfill('-') << " Comment" << " |" << comment << std::endl;
        }

        CPU cpu;
        cpu.reset();
        error_count = 0; // Reset error count before running
        try {
            cpu.execute(prog);
        } catch (const std::exception& e) {
            return {path.filename().string(), false, std::string("Exception: ") + e.what()};
        } catch (...) {
            return {path.filename().string(), false, "Unknown exception"};
        }
        if (error_count > 0) {
            return {path.filename().string(), false, "Runtime errors detected"};
        }
        return {path.filename().string(), true, ""};
    }
};
