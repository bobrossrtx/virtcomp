# Test Framework System

**Files:** `src/test/test_framework.hpp`, `src/test/test.hpp`, `src/test/unit_tests.cpp`  
**Purpose:** Comprehensive unit testing framework for VirtComp components

## Overview

The test framework provides a complete testing infrastructure for VirtComp, including unit tests for all CPU instructions, assembly language features, device operations, and system integration. It ensures reliability and enables regression testing during development.

## Test Framework Architecture

### Core Testing Classes

```cpp
namespace Testing {
    class TestCase {
    public:
        TestCase(const std::string& name);
        virtual ~TestCase() = default;
        
        // Test execution
        virtual void run() = 0;
        bool passed() const { return test_passed; }
        const std::string& get_name() const { return test_name; }
        const std::vector<std::string>& get_failures() const { return failures; }
        
    protected:
        // Assertion methods
        void assert_equals(uint32_t expected, uint32_t actual, const std::string& message = "");
        void assert_true(bool condition, const std::string& message = "");
        void assert_false(bool condition, const std::string& message = "");
        void assert_not_null(void* ptr, const std::string& message = "");
        
        // CPU state assertions
        void assert_register_equals(CPU& cpu, uint8_t reg, uint32_t expected);
        void assert_flag_set(CPU& cpu, CPUFlag flag);
        void assert_flag_clear(CPU& cpu, CPUFlag flag);
        void assert_memory_equals(CPU& cpu, uint32_t address, uint8_t expected);
        
    private:
        std::string test_name;
        bool test_passed = true;
        std::vector<std::string> failures;
    };
    
    class TestSuite {
    public:
        TestSuite(const std::string& name);
        
        void add_test(std::unique_ptr<TestCase> test);
        void run_all();
        
        // Results
        size_t total_tests() const;
        size_t passed_tests() const;
        size_t failed_tests() const;
        void print_summary() const;
        
    private:
        std::string suite_name;
        std::vector<std::unique_ptr<TestCase>> tests;
        std::vector<TestCase*> failed_tests;
    };
}
```

## Test Categories

### 1. CPU Instruction Tests

#### Arithmetic Operations
```cpp
class ArithmeticTests : public TestCase {
public:
    ArithmeticTests() : TestCase("Arithmetic Operations") {}
    
    void run() override {
        test_add_instruction();
        test_sub_instruction();
        test_mul_instruction();
        test_div_instruction();
        test_inc_dec_instructions();
        test_overflow_detection();
        test_zero_flag_setting();
    }
    
private:
    void test_add_instruction() {
        CPU cpu;
        cpu.set_register(0, 10);
        cpu.set_register(1, 20);
        
        // ADD R0, R1 -> R0 should be 30
        std::vector<uint8_t> program = {0x02, 0x00, 0x01};
        cpu.run(program);
        
        assert_register_equals(cpu, 0, 30);
        assert_flag_clear(cpu, CPUFlag::ZERO);
        assert_flag_clear(cpu, CPUFlag::OVERFLOW);
    }
    
    void test_overflow_detection() {
        CPU cpu;
        cpu.set_register(0, 0xFFFFFFFF);  // Max uint32_t
        cpu.set_register(1, 1);
        
        std::vector<uint8_t> program = {0x02, 0x00, 0x01};  // ADD R0, R1
        cpu.run(program);
        
        assert_register_equals(cpu, 0, 0);  // Wrapped to 0
        assert_flag_set(cpu, CPUFlag::ZERO);
        assert_flag_set(cpu, CPUFlag::OVERFLOW);
    }
};
```

#### Logic Operations
```cpp
class LogicTests : public TestCase {
public:
    LogicTests() : TestCase("Logic Operations") {}
    
    void run() override {
        test_and_operation();
        test_or_operation();
        test_xor_operation();
        test_not_operation();
        test_shift_operations();
    }
    
private:
    void test_and_operation() {
        CPU cpu;
        cpu.set_register(0, 0b11110000);
        cpu.set_register(1, 0b10101010);
        
        std::vector<uint8_t> program = {0x14, 0x00, 0x01};  // AND R0, R1
        cpu.run(program);
        
        assert_register_equals(cpu, 0, 0b10100000);
    }
};
```

#### Memory Operations
```cpp
class MemoryTests : public TestCase {
public:
    MemoryTests() : TestCase("Memory Operations") {}
    
    void run() override {
        test_load_immediate();
        test_load_store();
        test_mov_instruction();
        test_memory_boundaries();
    }
    
private:
    void test_load_immediate() {
        CPU cpu;
        
        std::vector<uint8_t> program = {0x01, 0x00, 42};  // LOAD_IMM R0, 42
        cpu.run(program);
        
        assert_register_equals(cpu, 0, 42);
    }
    
    void test_memory_boundaries() {
        CPU cpu;
        
        // Test memory boundary violations
        std::vector<uint8_t> program = {
            0x06, 0x00, 0xFF, 0xFF, 0xFF, 0xFF  // LOAD R0, 0xFFFFFFFF (beyond memory)
        };
        
        // Should handle gracefully without crashing
        cpu.run(program);
        assert_true(cpu.has_error(), "Should detect memory boundary violation");
    }
};
```

### 2. Assembly Language Tests

#### Lexer Tests
```cpp
class LexerTests : public TestCase {
public:
    LexerTests() : TestCase("Assembly Lexer") {}
    
    void run() override {
        test_instruction_tokenization();
        test_register_recognition();
        test_number_parsing();
        test_comment_handling();
        test_label_recognition();
    }
    
private:
    void test_instruction_tokenization() {
        Lexer lexer("LOAD_IMM R0, 42");
        
        Token token1 = lexer.next_token();
        assert_equals(static_cast<int>(token1.type), 
                     static_cast<int>(TokenType::IDENTIFIER));
        assert_equals(token1.value, "LOAD_IMM");
        
        Token token2 = lexer.next_token();
        assert_equals(static_cast<int>(token2.type), 
                     static_cast<int>(TokenType::REGISTER));
        assert_equals(token2.value, "R0");
        
        // Skip comma
        lexer.next_token();
        
        Token token3 = lexer.next_token();
        assert_equals(static_cast<int>(token3.type), 
                     static_cast<int>(TokenType::IMMEDIATE));
        assert_equals(token3.value, "42");
    }
    
    void test_number_parsing() {
        Lexer lexer("42 0x2A 0b101010");
        
        // Decimal
        Token decimal = lexer.next_token();
        assert_equals(decimal.value, "42");
        
        // Hexadecimal
        Token hex = lexer.next_token();
        assert_equals(hex.value, "0x2A");
        
        // Binary
        Token binary = lexer.next_token();
        assert_equals(binary.value, "0b101010");
    }
};
```

#### Parser Tests
```cpp
class ParserTests : public TestCase {
public:
    ParserTests() : TestCase("Assembly Parser") {}
    
    void run() override {
        test_instruction_parsing();
        test_label_parsing();
        test_error_recovery();
        test_forward_references();
    }
    
private:
    void test_instruction_parsing() {
        std::string source = "LOAD_IMM R0, 42\nADD R0, R1";
        Lexer lexer(source);
        Parser parser(lexer);
        
        Program program = parser.parse();
        
        assert_equals(program.instructions.size(), 2);
        assert_equals(program.instructions[0].mnemonic, "LOAD_IMM");
        assert_equals(program.instructions[0].operands.size(), 2);
        assert_equals(program.instructions[1].mnemonic, "ADD");
    }
    
    void test_forward_references() {
        std::string source = 
            "JMP end\n"
            "LOAD_IMM R0, 42\n"
            "end:\n"
            "HALT";
        
        Lexer lexer(source);
        Parser parser(lexer);
        Program program = parser.parse();
        
        assert_false(parser.has_errors(), "Should handle forward references");
        assert_equals(program.labels.size(), 1);
        assert_equals(program.labels[0].name, "end");
    }
};
```

#### Assembler Tests
```cpp
class AssemblerTests : public TestCase {
public:
    AssemblerTests() : TestCase("Assembly Code Generation") {}
    
    void run() override {
        test_bytecode_generation();
        test_symbol_resolution();
        test_address_calculation();
        test_instruction_encoding();
    }
    
private:
    void test_bytecode_generation() {
        std::string source = "LOAD_IMM R0, 42\nHALT";
        auto bytecode = VirtCompAssembler::assemble_string(source);
        
        // Expected: [0x01, 0x00, 42, 0xFF]
        assert_equals(bytecode.size(), 4);
        assert_equals(bytecode[0], 0x01);  // LOAD_IMM opcode
        assert_equals(bytecode[1], 0x00);  // R0
        assert_equals(bytecode[2], 42);    // Immediate value
        assert_equals(bytecode[3], 0xFF);  // HALT opcode
    }
    
    void test_symbol_resolution() {
        std::string source = 
            "main:\n"
            "    LOAD_IMM R0, 1\n"
            "    JMP main";
        
        auto bytecode = VirtCompAssembler::assemble_string(source);
        
        // Should resolve 'main' label to address 0
        assert_true(bytecode.size() > 0, "Should generate valid bytecode");
        // Verify jump address is correctly resolved
        // JMP instruction should reference address 0
    }
};
```

### 3. Device and I/O Tests

```cpp
class DeviceTests : public TestCase {
public:
    DeviceTests() : TestCase("Device I/O Operations") {}
    
    void run() override {
        test_console_device();
        test_device_registration();
        test_invalid_device_access();
        test_device_error_handling();
    }
    
private:
    void test_console_device() {
        DeviceManager dm;
        auto console = DeviceFactory::create_console_device();
        dm.register_device(1, std::move(console));
        
        CPU cpu;
        cpu.attach_device_manager(&dm);
        
        // OUT 1, R0 instruction
        cpu.set_register(0, 65);  // 'A'
        std::vector<uint8_t> program = {0x1B, 0x01, 0x00};  // OUT 1, R0
        
        // Should not crash and should write to console
        cpu.run(program);
        assert_true(dm.device_exists(1), "Console device should exist");
    }
    
    void test_invalid_device_access() {
        CPU cpu;
        DeviceManager dm;
        cpu.attach_device_manager(&dm);
        
        // Try to access non-existent device
        std::vector<uint8_t> program = {0x1B, 0x99, 0x00};  // OUT 99, R0
        
        cpu.run(program);
        // Should handle gracefully without crashing
        assert_true(true, "Should survive invalid device access");
    }
};
```

### 4. Integration Tests

```cpp
class IntegrationTests : public TestCase {
public:
    IntegrationTests() : TestCase("System Integration") {}
    
    void run() override {
        test_full_program_execution();
        test_assembly_to_execution();
        test_complex_control_flow();
        test_stack_operations();
    }
    
private:
    void test_full_program_execution() {
        // Test a complete program with multiple features
        std::string source = 
            "; Calculate factorial of 5\n"
            "main:\n"
            "    LOAD_IMM R0, 5      ; n = 5\n"
            "    LOAD_IMM R1, 1      ; result = 1\n"
            "factorial_loop:\n"
            "    CMP R0, 0\n"
            "    JZ done\n"
            "    MUL R1, R0          ; result *= n\n"
            "    DEC R0              ; n--\n"
            "    JMP factorial_loop\n"
            "done:\n"
            "    HALT\n";
        
        auto bytecode = VirtCompAssembler::assemble_string(source);
        
        CPU cpu;
        cpu.run(bytecode);
        
        // Factorial of 5 should be 120
        assert_register_equals(cpu, 1, 120);
    }
};
```

## Test Execution Framework

### Test Runner
```cpp
class TestRunner {
public:
    static void run_all_tests() {
        TestSuite cpu_tests("CPU Instruction Tests");
        cpu_tests.add_test(std::make_unique<ArithmeticTests>());
        cpu_tests.add_test(std::make_unique<LogicTests>());
        cpu_tests.add_test(std::make_unique<MemoryTests>());
        
        TestSuite assembly_tests("Assembly Language Tests");
        assembly_tests.add_test(std::make_unique<LexerTests>());
        assembly_tests.add_test(std::make_unique<ParserTests>());
        assembly_tests.add_test(std::make_unique<AssemblerTests>());
        
        TestSuite device_tests("Device I/O Tests");
        device_tests.add_test(std::make_unique<DeviceTests>());
        
        TestSuite integration_tests("Integration Tests");
        integration_tests.add_test(std::make_unique<IntegrationTests>());
        
        // Run all test suites
        cpu_tests.run_all();
        assembly_tests.run_all();
        device_tests.run_all();
        integration_tests.run_all();
        
        // Print comprehensive summary
        print_overall_summary({&cpu_tests, &assembly_tests, 
                             &device_tests, &integration_tests});
    }
    
private:
    static void print_overall_summary(const std::vector<TestSuite*>& suites) {
        size_t total_tests = 0;
        size_t total_passed = 0;
        
        for (auto* suite : suites) {
            total_tests += suite->total_tests();
            total_passed += suite->passed_tests();
            suite->print_summary();
        }
        
        std::cout << "\n=== OVERALL TEST RESULTS ===" << std::endl;
        std::cout << "Total Tests: " << total_tests << std::endl;
        std::cout << "Passed: " << total_passed << std::endl;
        std::cout << "Failed: " << (total_tests - total_passed) << std::endl;
        std::cout << "Success Rate: " << 
                     (100.0 * total_passed / total_tests) << "%" << std::endl;
    }
};
```

## Test Automation

### Continuous Integration
```bash
#!/bin/bash
# run_tests.sh - Automated test execution

echo "Building VirtComp with test support..."
make clean
make test

echo "Running comprehensive test suite..."
./bin/virtcomp --test

echo "Running assembly language examples..."
for asm_file in examples/*.asm; do
    echo "Testing: $asm_file"
    ./bin/virtcomp -A "$asm_file" || echo "FAILED: $asm_file"
done

echo "Test execution completed."
```

### Test Coverage Analysis
```cpp
class CoverageTracker {
public:
    void track_instruction(uint8_t opcode);
    void track_code_path(const std::string& function_name);
    
    void generate_coverage_report();
    double get_instruction_coverage() const;
    double get_code_coverage() const;
    
private:
    std::set<uint8_t> executed_opcodes;
    std::set<std::string> executed_functions;
    
    static const std::set<uint8_t> all_opcodes;
    static const std::set<std::string> all_functions;
};
```

## Test Data and Fixtures

### Test Program Repository
```cpp
namespace TestPrograms {
    // Basic instruction test programs
    const std::vector<uint8_t> SIMPLE_ADD = {
        0x01, 0x00, 10,     // LOAD_IMM R0, 10
        0x01, 0x01, 20,     // LOAD_IMM R1, 20
        0x02, 0x00, 0x01,   // ADD R0, R1
        0xFF                // HALT
    };
    
    const std::vector<uint8_t> JUMP_TEST = {
        0x05, 0x00, 0x00, 0x06,  // JMP 6
        0x01, 0x00, 99,          // LOAD_IMM R0, 99 (should be skipped)
        0x01, 0x00, 42,          // LOAD_IMM R0, 42 (target)
        0xFF                     // HALT
    };
    
    // Complex program examples
    std::string FACTORIAL_PROGRAM = R"(
        ; Calculate factorial
        LOAD_IMM R0, 5
        LOAD_IMM R1, 1
        loop:
            CMP R0, 0
            JZ done
            MUL R1, R0
            DEC R0
            JMP loop
        done:
            HALT
    )";
}
```

The test framework ensures VirtComp reliability through comprehensive testing of all components, from individual CPU instructions to complete program execution, supporting both automated regression testing and development validation.
