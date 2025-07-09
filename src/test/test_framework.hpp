#pragma once

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <chrono>
#include <limits>
#include <fmt/format.h>
#include "../vhardware/cpu.hpp"
#include "../vhardware/device_factory.hpp"
#include "../config.hpp"
#include "../debug/logger.hpp"

// Forward declarations
class TestContext;
class TestFramework;

// Test case structure
struct TestCase {
    std::string name;
    std::string category;
    std::function<void(TestContext&)> test_func;
    bool expect_error;

    TestCase(const std::string& n, const std::string& cat,
             std::function<void(TestContext&)> func, bool expect_err = false)
        : name(n), category(cat), test_func(func), expect_error(expect_err) {}
};

// Test result structure
struct TestResult {
    std::string name;
    std::string category;
    bool passed;
    std::string message;
    double duration_ms;

    TestResult(const std::string& n, const std::string& cat, bool p,
               const std::string& msg = "", double dur = 0.0)
        : name(n), category(cat), passed(p), message(msg), duration_ms(dur) {}
};

// Test suite for organizing tests
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}

    void add_test(const TestCase& test) {
        tests_.push_back(test);
    }

    const std::vector<TestCase>& get_tests() const { return tests_; }
    const std::string& get_name() const { return name_; }

private:
    std::string name_;
    std::vector<TestCase> tests_;
};

// Exception for test assertion failures
class AssertionFailure : public std::exception {
public:
    explicit AssertionFailure(const std::string& message) : message_(message) {}
    const char* what() const noexcept override { return message_.c_str(); }

private:
    std::string message_;
};

// Test context - provides testing environment and assertions
class TestContext {
public:
    TestContext() : cpu(), program() {
        cpu.reset();
        initialize_devices();
    }

    ~TestContext() = default;

    // Program loading
    void load_program(const std::vector<uint8_t>& prog) {
        program = prog;
    }

    void load_hex_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot open hex file: " + filename);
        }

        program.clear();
        std::string token;
        while (file >> token) {
            if (token[0] == '#') {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            try {
                uint8_t byte = static_cast<uint8_t>(std::stoul(token, nullptr, 16));
                program.push_back(byte);
            } catch (...) {
                throw std::runtime_error("Invalid hex byte: " + token);
            }
        }
    }

    // Quick operations
    void load_immediate(int reg, uint32_t value) {
        if (reg < 0 || reg >= 8) {
            throw std::runtime_error("Invalid register: " + std::to_string(reg));
        }
        cpu.get_registers()[reg] = value;
    }

    void execute_single_step() {
        if (program.empty()) {
            throw std::runtime_error("No program loaded");
        }
        cpu.step(program);
    }

    void execute_program() {
        if (program.empty()) {
            throw std::runtime_error("No program loaded");
        }

        // Add a safety limit to prevent infinite loops in tests
        const size_t MAX_STEPS = 10000;
        cpu.reset();

        // Copy program into memory
        std::copy(program.begin(), program.end(), cpu.get_memory().begin());
        cpu.set_pc(0);
        cpu.set_sp(cpu.get_memory().size() - 4);

        size_t step_count = 0;

        while (step_count < MAX_STEPS) {
            bool continue_execution = cpu.step(program);
            step_count++;

            if (!continue_execution) {
                break; // Program halted or error occurred
            }
        }

        if (step_count >= MAX_STEPS) {
            throw std::runtime_error("Test exceeded maximum execution steps (possible infinite loop)");
        }
    }

    // State inspection
    uint32_t get_register(int reg) const {
        if (reg < 0 || reg >= 8) {
            throw std::runtime_error("Invalid register: " + std::to_string(reg));
        }
        return cpu.get_registers()[reg];
    }

    uint8_t get_memory(uint32_t addr) const {
        auto& memory = const_cast<CPU&>(cpu).get_memory();
        if (addr >= memory.size()) {
            throw std::runtime_error("Memory address out of bounds: " + std::to_string(addr));
        }
        return memory[addr];
    }

    uint32_t get_flags() const {
        return cpu.get_flags();
    }

    uint32_t get_pc() const {
        return cpu.get_pc();
    }

    uint32_t get_sp() const {
        return cpu.get_sp();
    }

    // Register assertions
    void assert_register_eq(int reg, uint32_t expected) {
        uint32_t actual = get_register(reg);
        if (actual != expected) {
            throw AssertionFailure(fmt::format(
                "Register R{} assertion failed: expected {}, got {}",
                reg, expected, actual));
        }
    }

    void assert_register_ne(int reg, uint32_t unexpected) {
        uint32_t actual = get_register(reg);
        if (actual == unexpected) {
            throw AssertionFailure(fmt::format(
                "Register R{} assertion failed: expected NOT {}, but got {}",
                reg, unexpected, actual));
        }
    }

    // Memory assertions
    void assert_memory_eq(uint32_t addr, uint8_t expected) {
        uint8_t actual = get_memory(addr);
        if (actual != expected) {
            throw AssertionFailure(fmt::format(
                "Memory[0x{:02X}] assertion failed: expected 0x{:02X}, got 0x{:02X}",
                addr, expected, actual));
        }
    }

    void assert_memory_range_eq(uint32_t addr, const std::vector<uint8_t>& expected) {
        for (size_t i = 0; i < expected.size(); i++) {
            assert_memory_eq(addr + i, expected[i]);
        }
    }

    // Flag assertions
    void assert_flag_set(uint32_t flag) {
        uint32_t flags = get_flags();
        if (!(flags & flag)) {
            throw AssertionFailure(fmt::format(
                "Flag assertion failed: expected flag 0x{:08X} to be set, but flags are 0x{:08X}",
                flag, flags));
        }
    }

    void assert_flag_clear(uint32_t flag) {
        uint32_t flags = get_flags();
        if (flags & flag) {
            throw AssertionFailure(fmt::format(
                "Flag assertion failed: expected flag 0x{:08X} to be clear, but flags are 0x{:08X}",
                flag, flags));
        }
    }

    void assert_flags_eq(uint32_t expected) {
        uint32_t actual = get_flags();
        if (actual != expected) {
            throw AssertionFailure(fmt::format(
                "Flags assertion failed: expected 0x{:08X}, got 0x{:08X}",
                expected, actual));
        }
    }

    // CPU state assertions
    void assert_pc_eq(uint32_t expected) {
        uint32_t actual = get_pc();
        if (actual != expected) {
            throw AssertionFailure(fmt::format(
                "PC assertion failed: expected {}, got {}",
                expected, actual));
        }
    }

    void assert_sp_eq(uint32_t expected) {
        uint32_t actual = get_sp();
        if (actual != expected) {
            throw AssertionFailure(fmt::format(
                "SP assertion failed: expected {}, got {}",
                expected, actual));
        }
    }

    // Exception assertions
    void assert_throws(std::function<void()> func) {
        bool threw = false;
        try {
            func();
        } catch (...) {
            threw = true;
        }
        if (!threw) {
            throw AssertionFailure("Expected exception but none was thrown");
        }
    }

    void assert_no_throw(std::function<void()> func) {
        try {
            func();
        } catch (const std::exception& e) {
            throw AssertionFailure(fmt::format(
                "Expected no exception but got: {}", e.what()));
        } catch (...) {
            throw AssertionFailure("Expected no exception but got unknown exception");
        }
    }

    void assert_error_count(int expected) {
        if (Config::error_count != expected) {
            throw AssertionFailure(fmt::format(
                "Error count assertion failed: expected {}, got {}",
                expected, Config::error_count));
        }
    }

    // Generic equality assertion
    template<typename T>
    void assert_eq(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            std::string msg = message.empty() ?
                fmt::format("Assertion failed: expected {}, got {}", expected, actual) :
                fmt::format("{}: expected {}, got {}", message, expected, actual);
            throw AssertionFailure(msg);
        }
    }

    // CPU instance for direct access if needed
    CPU cpu;
    std::vector<uint8_t> program;
};

// Main test framework
class TestFramework {
public:
    static TestFramework& instance() {
        static TestFramework instance;
        return instance;
    }

    void register_test(const TestCase& test) {
        tests_.push_back(test);
    }

    void register_suite(const TestSuite& suite) {
        for (const auto& test : suite.get_tests()) {
            register_test(test);
        }
    }

    std::vector<TestResult> run_all() {
        return run_filtered("");
    }

    std::vector<TestResult> run_category(const std::string& category) {
        return run_filtered(category);
    }

    std::vector<TestResult> run_single(const std::string& name) {
        std::vector<TestResult> results;
        for (const auto& test : tests_) {
            if (test.name == name) {
                results.push_back(run_test(test));
                break;
            }
        }
        return results;
    }

    void print_results(const std::vector<TestResult>& results) {
        int passed = 0, failed = 0;

        // Print header
        std::cout << fmt::format("{}┌────────────────────────────────────────────────────────────┐{}\n",
                                "\033[36m", "\033[0m");
        std::cout << fmt::format("{}│     VirtComp Unit Test Results                             │{}\n",
                                "\033[36m", "\033[0m");
        std::cout << fmt::format("{}└────────────────────────────────────────────────────────────┘{}\n",
                                "\033[36m", "\033[0m");

        // Print individual results
        for (const auto& result : results) {
            const char* color = result.passed ? "\033[32m" : "\033[31m";
            const char* reset = "\033[0m";
            const char* status = result.passed ? "PASS" : "FAIL";

            std::cout << fmt::format("{}[{}]{} {} [{:.1f}ms]",
                                   color, status, reset, result.name, result.duration_ms);

            if (!result.passed && !result.message.empty()) {
                std::cout << fmt::format(" ── {}", result.message);
            }
            std::cout << std::endl;

            if (result.passed) ++passed;
            else ++failed;
        }

        // Print summary
        const char* summary_color = (failed == 0) ? "\033[32m" : "\033[33m";
        std::cout << fmt::format("\n{}Tests passed: {} / {}{}\n",
                                summary_color, passed, results.size(), "\033[0m");
    }

private:
    TestFramework() = default;
    std::vector<TestCase> tests_;

    std::vector<TestResult> run_filtered(const std::string& category_filter) {
        std::vector<TestResult> results;

        for (const auto& test : tests_) {
            if (category_filter.empty() || test.category == category_filter) {
                results.push_back(run_test(test));
            }
        }

        return results;
    }

    TestResult run_test(const TestCase& test) {
        auto start = std::chrono::high_resolution_clock::now();

        try {
            // Show which test is currently running using logger
            Logger::instance().running() << fmt::format("{:>11}Unit Test │ {} ({})", " ", test.name, test.category) << std::endl;

            // Reset global state
            Config::error_count = 0;

            // Create test context
            TestContext context;

            // Run the test
            test.test_func(context);

            // Check if we expected an error but didn't get one
            if (test.expect_error && Config::error_count == 0) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return TestResult(test.name, test.category, false,
                                "Expected error but test passed", duration.count() / 1000.0);
            }

            // Test passed
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return TestResult(test.name, test.category, true, "", duration.count() / 1000.0);

        } catch (const AssertionFailure& e) {
            // If we expected an error and got an assertion failure, that might be OK
            if (test.expect_error) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return TestResult(test.name, test.category, true,
                                "Expected error: " + std::string(e.what()), duration.count() / 1000.0);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return TestResult(test.name, test.category, false, e.what(), duration.count() / 1000.0);

        } catch (const std::exception& e) {
            // If we expected an error and got a standard exception, that's probably OK
            if (test.expect_error) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return TestResult(test.name, test.category, true,
                                "Expected error: " + std::string(e.what()), duration.count() / 1000.0);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return TestResult(test.name, test.category, false,
                            "Exception: " + std::string(e.what()), duration.count() / 1000.0);

        } catch (...) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return TestResult(test.name, test.category, false,
                            "Unknown exception", duration.count() / 1000.0);
        }
    }
};

// Test registration helper
class TestRegistrar {
public:
    TestRegistrar(const std::string& name, const std::string& category,
                  std::function<void(TestContext&)> test_func, bool expect_error = false) {
        TestFramework::instance().register_test(TestCase(name, category, test_func, expect_error));
    }
};

// Macros for easy test registration
#define TEST_CASE(test_name, category) \
    static void test_function_##test_name(TestContext& ctx); \
    static TestRegistrar test_registrar_##test_name(#test_name, category, test_function_##test_name); \
    static void test_function_##test_name(TestContext& ctx)

#define TEST_CASE_EXPECT_ERROR(test_name, category) \
    static void test_function_##test_name(TestContext& ctx); \
    static TestRegistrar test_registrar_##test_name(#test_name, category, test_function_##test_name, true); \
    static void test_function_##test_name(TestContext& ctx)

// Function to run all tests (for integration with existing main)
inline void run_unit_tests() {
    auto& framework = TestFramework::instance();
    auto results = framework.run_all();
    framework.print_results(results);
}

// Function to run specific category
inline void run_unit_tests_category(const std::string& category) {
    auto& framework = TestFramework::instance();
    auto results = framework.run_category(category);
    framework.print_results(results);
}
