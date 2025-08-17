#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <fmt/core.h>
#include "../engine/cpu.hpp"

class TestRunner {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
    };

    TestRunner(const std::string& test_dir = "tests/hex") : test_dir_(test_dir) {}

    std::vector<TestResult> run_all() {
        std::vector<TestResult> results;
        for (const auto& entry : std::filesystem::directory_iterator(test_dir_)) {
            if (entry.path().extension() == ".hex") {
                std::string test_name = entry.path().filename().string();
                Logger::instance().running()
                    << fmt::format("[RUN] │ {}", test_name) << std::endl;
                TestResult result = run_test(entry.path());
                std::ostringstream oss;

                oss << "[" << (result.passed ? "PASS" : "FAIL") << "] │ " << test_name;
                if (!result.passed && !result.message.empty())
                    oss << " ── " << result.message;
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
        bool expect_error = false;

        while (file >> token) {
            if (token[0] == '#') {
                // Read the rest of the line as a comment
                std::string rest_of_line;
                std::getline(file, rest_of_line);
                if (!comment.empty()) comment += "";
                comment += rest_of_line;

                // Check if this test expects an error
                if (comment.find("(error expected)") != std::string::npos ||
                    comment.find("error expected") != std::string::npos ||
                    comment.find("Invalid opcode") != std::string::npos ||
                    comment.find("Division by zero") != std::string::npos) {
                    expect_error = true;
                }
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
            Logger::instance().info() << fmt::format("[COMMENT] │{}", comment) << std::endl;
        }

        // Check for empty program
        if (prog.empty()) {
            return {path.filename().string(), false, "Empty test file - no program to execute"};
        }

        CPU cpu;
        cpu.reset();
        initialize_devices();
        Config::error_count = 0; // Reset error count before running

        try {
            cpu.execute(prog);
        } catch (const std::exception& e) {
            // If we expect an error, this is a pass
            if (expect_error) {
                return {path.filename().string(), true, "Expected error occurred: " + std::string(e.what())};
            }
            return {path.filename().string(), false, std::string("Exception: ") + e.what()};
        } catch (...) {
            // If we expect an error, this is a pass
            if (expect_error) {
                return {path.filename().string(), true, "Expected error occurred: Unknown exception"};
            }
            return {path.filename().string(), false, "Unknown exception"};
        }

        // Check error count after execution
        if (Config::error_count > 0) {
            // If we expect an error, this is a pass
            if (expect_error) {
                return {path.filename().string(), true, "Expected runtime errors detected"};
            }
            return {path.filename().string(), false, "Runtime errors detected"};
        }

        // If we expected an error but didn't get one, this is a failure
        if (expect_error) {
            return {path.filename().string(), false, "Expected error but execution succeeded"};
        }

        return {path.filename().string(), true, ""};
    }
};
