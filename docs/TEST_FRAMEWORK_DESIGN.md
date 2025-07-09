# VirtComp Testing Framework Design Document

## Overview

This document outlines the design for a modern, comprehensive testing framework for VirtComp that combines unit testing capabilities with integration testing support for hex programs.

## Current Problems

1. **Limited Assertions**: Only pass/fail based on runtime errors
2. **No Component Isolation**: Can't test CPU, devices, or opcodes independently
3. **Poor Error Reporting**: No detailed failure information
4. **No Test Organization**: Can't group or categorize tests
5. **No Setup/Teardown**: Each test reinitializes everything
6. **No Parameterized Tests**: Can't run same test with different inputs
7. **No Mocking**: Can't isolate components from dependencies

## Design Goals

### Primary Goals
- **Rich Assertions**: Test register values, memory state, flags, device state
- **Component Testing**: Unit tests for CPU, devices, opcodes individually
- **Integration Testing**: Keep hex file tests for full system validation
- **Clear Reporting**: Detailed failure messages with context
- **Test Organization**: Group tests by category/component
- **Easy Extension**: Simple to add new test types and assertions

### Secondary Goals
- **Performance Testing**: Benchmark critical operations
- **Mock Support**: Test components in isolation
- **Parameterized Tests**: Data-driven testing
- **Test Fixtures**: Reusable test setup
- **Educational Value**: Clear, understandable test code

## Architecture

### Core Components

#### 1. Test Framework Core
```cpp
class TestFramework {
    // Test registration and execution
    // Result collection and reporting
    // Test discovery and filtering
};
```

#### 2. Test Context
```cpp
class TestContext {
    CPU cpu;
    std::vector<uint8_t> program;
    std::string test_name;

    // Rich assertion methods
    // State inspection utilities
    // Helper methods for common operations
};
```

#### 3. Assertion System
```cpp
class Assertions {
    // CPU state assertions
    // Memory assertions
    // Device assertions
    // Flag assertions
    // Exception assertions
};
```

#### 4. Test Registration
```cpp
#define TEST_CASE(name, category) \
    // Macro for easy test registration

#define TEST_SUITE(name) \
    // Macro for test suite organization
```

## Test Types

### 1. Unit Tests (C++)
- **Opcode Tests**: Each instruction tested individually
- **CPU Component Tests**: Register, memory, flags, stack operations
- **Device Tests**: Each device type tested independently
- **Utility Tests**: Helper functions and utilities

### 2. Integration Tests (Hex Files)
- **System Tests**: Full programs testing complete workflows
- **Device Integration**: Multi-device interactions
- **Error Handling**: Expected error scenarios

### 3. Performance Tests
- **Benchmark Tests**: Critical operation performance
- **Memory Usage**: Memory allocation and cleanup
- **Execution Speed**: Instruction throughput

## API Design

### Test Registration
```cpp
// Simple test case
TEST_CASE("ADD instruction basic", "arithmetic") {
    ctx.load_program({0x01, 0x00, 0x05,  // LOAD_IMM R0, 5
                     0x01, 0x01, 0x03,   // LOAD_IMM R1, 3
                     0x02, 0x00, 0x01,   // ADD R0, R1
                     0xFF});             // HALT

    ctx.cpu.execute(ctx.program);
    ctx.assert_register_eq(0, 8);
    ctx.assert_register_eq(1, 3);
}

// Test suite
TEST_SUITE("Arithmetic Operations") {
    TEST_CASE("ADD", "arithmetic") { /* ... */ }
    TEST_CASE("SUB", "arithmetic") { /* ... */ }
    TEST_CASE("MUL", "arithmetic") { /* ... */ }
    TEST_CASE("DIV", "arithmetic") { /* ... */ }
}

// Parameterized test
TEST_CASE_PARAM("ADD with different values", "arithmetic",
    std::vector<std::tuple<int, int, int>>{{1,2,3}, {5,7,12}, {0,0,0}}) {
    auto [a, b, expected] = GetParam();
    ctx.load_immediate(0, a);
    ctx.load_immediate(1, b);
    ctx.execute_instruction(Opcode::ADD, 0, 1);
    ctx.assert_register_eq(0, expected);
}
```

### Assertion Methods
```cpp
// Register assertions
void assert_register_eq(int reg, uint32_t expected);
void assert_register_ne(int reg, uint32_t unexpected);
void assert_register_in_range(int reg, uint32_t min, uint32_t max);

// Memory assertions
void assert_memory_eq(uint32_t addr, uint8_t expected);
void assert_memory_range_eq(uint32_t addr, const std::vector<uint8_t>& expected);

// Flag assertions
void assert_flag_set(uint32_t flag);
void assert_flag_clear(uint32_t flag);
void assert_flags_eq(uint32_t expected);

// CPU state assertions
void assert_pc_eq(uint32_t expected);
void assert_sp_eq(uint32_t expected);
void assert_fp_eq(uint32_t expected);

// Device assertions
void assert_device_state(uint8_t port, const std::string& expected_state);
void assert_port_value(uint8_t port, uint8_t expected);

// Exception assertions
void assert_throws(std::function<void()> func);
void assert_no_throw(std::function<void()> func);
void assert_error_count(int expected);

// String and output assertions
void assert_console_output(const std::string& expected);
void assert_console_contains(const std::string& substring);
```

### Helper Methods
```cpp
// Program loading
void load_program(const std::vector<uint8_t>& program);
void load_hex_file(const std::string& filename);

// Quick operations
void load_immediate(int reg, uint32_t value);
void execute_instruction(Opcode op, int reg1, int reg2 = -1);
void execute_single_step();

// State inspection
uint32_t get_register(int reg);
uint8_t get_memory(uint32_t addr);
uint32_t get_flags();
std::string get_console_output();

// Device helpers
void setup_mock_device(uint8_t port);
void expect_device_read(uint8_t port, uint8_t value);
void expect_device_write(uint8_t port, uint8_t value);
```

## Implementation Plan

### Phase 1: Core Framework (Prototype)
1. **Basic TestFramework class** - Test registration and execution
2. **TestContext class** - CPU instance and basic assertions
3. **Simple assertions** - Register, memory, flag checks
4. **Test registration macro** - TEST_CASE macro
5. **Basic reporting** - Pass/fail with detailed messages

### Phase 2: Enhanced Features
1. **Test suites** - Group related tests
2. **More assertions** - Device, exception, string assertions
3. **Helper methods** - Quick operations and state inspection
4. **Parameterized tests** - Data-driven testing
5. **Better reporting** - Detailed failure context

### Phase 3: Integration
1. **Hex file integration** - Keep existing hex tests
2. **Mock devices** - Test isolation
3. **Performance tests** - Benchmarking support
4. **Test fixtures** - Reusable setup/teardown
5. **Continuous integration** - Automated testing

### Phase 4: Advanced Features
1. **Code coverage** - Track test coverage
2. **Fuzz testing** - Random input testing
3. **Regression testing** - Prevent old bugs
4. **Documentation** - Auto-generated test docs
5. **IDE integration** - Test explorer support

## File Structure

```
src/test/
├── framework/
│   ├── test_framework.hpp      # Core framework
│   ├── test_context.hpp        # Test context and assertions
│   ├── test_macros.hpp         # Registration macros
│   └── test_utils.hpp          # Helper utilities
├── unit/
│   ├── cpu_test.cpp            # CPU unit tests
│   ├── opcode_test.cpp         # Individual opcode tests
│   ├── device_test.cpp         # Device unit tests
│   └── memory_test.cpp         # Memory system tests
├── integration/
│   ├── hex_test.cpp            # Hex file integration tests
│   └── system_test.cpp         # Full system tests
├── performance/
│   └── benchmark_test.cpp      # Performance tests
└── test_main.cpp               # Test runner main
```

## Example Usage

```cpp
// cpu_test.cpp
#include "test_framework.hpp"

TEST_SUITE("CPU Basic Operations") {
    TEST_CASE("CPU reset", "cpu") {
        ctx.cpu.reset();
        ctx.assert_pc_eq(0);
        ctx.assert_sp_eq(256);
        ctx.assert_flags_eq(0);
        for (int i = 0; i < 8; i++) {
            ctx.assert_register_eq(i, 0);
        }
    }

    TEST_CASE("Program counter increment", "cpu") {
        ctx.load_program({0x00, 0x00, 0x00});  // NOP, NOP, NOP
        ctx.execute_single_step();
        ctx.assert_pc_eq(1);
        ctx.execute_single_step();
        ctx.assert_pc_eq(2);
    }
}

TEST_SUITE("Arithmetic Instructions") {
    TEST_CASE("ADD instruction", "arithmetic") {
        ctx.load_immediate(0, 10);
        ctx.load_immediate(1, 5);
        ctx.execute_instruction(Opcode::ADD, 0, 1);
        ctx.assert_register_eq(0, 15);
        ctx.assert_register_eq(1, 5);  // R1 unchanged
    }

    TEST_CASE("Division by zero", "arithmetic") {
        ctx.load_immediate(0, 10);
        ctx.load_immediate(1, 0);
        ctx.assert_throws([&]() {
            ctx.execute_instruction(Opcode::DIV, 0, 1);
        });
    }
}
```

## Success Metrics

1. **Test Coverage**: >90% code coverage for core components
2. **Test Speed**: All tests run in <5 seconds
3. **Error Detection**: Catch regressions in existing functionality
4. **Maintainability**: Easy to add new tests for new features
5. **Educational Value**: Clear, understandable test code

## Migration Strategy

1. **Phase 1**: Implement prototype alongside existing tests
2. **Phase 2**: Migrate critical hex tests to unit tests
3. **Phase 3**: Add comprehensive unit test coverage
4. **Phase 4**: Deprecate old test system when new one is complete

This framework will provide a solid foundation for testing VirtComp while maintaining educational value and ease of use.
