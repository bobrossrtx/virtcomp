#include "test_framework.hpp"
#include "../vhardware/cpu_flags.hpp"

// Example unit tests using the new framework

TEST_CASE(cpu_reset, "cpu") {
    // Test that CPU reset works properly
    ctx.cpu.reset();

    // Check that all registers are zero
    for (int i = 0; i < 8; i++) {
        ctx.assert_register_eq(i, 0);
    }

    // Check that PC is zero
    ctx.assert_pc_eq(0);

    // Check that flags are zero
    ctx.assert_flags_eq(0);

    // Check that stack pointer is at end of memory
    ctx.assert_sp_eq(256);
}

TEST_CASE(load_immediate, "instructions") {
    // Test LOAD_IMM instruction
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 5
        0x01, 0x01, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 10
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that registers have correct values
    ctx.assert_register_eq(0, 5);
    ctx.assert_register_eq(1, 10);

    // Check that other registers are still zero
    for (int i = 2; i < 8; i++) {
        ctx.assert_register_eq(i, 0);
    }
}

TEST_CASE(add_instruction, "arithmetic") {
    // Test ADD instruction
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 5
        0x01, 0x01, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 3
        0x02, 0x00, 0x01,                     // ADD R0, R1
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that R0 contains the sum
    ctx.assert_register_eq(0, 8);

    // Check that R1 is unchanged
    ctx.assert_register_eq(1, 3);
}

TEST_CASE(sub_instruction, "arithmetic") {
    // Test SUB instruction
    ctx.load_program({
        0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 10
        0x01, 0x01, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 3
        0x03, 0x00, 0x01,                     // SUB R0, R1
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that R0 contains the difference
    ctx.assert_register_eq(0, 7);

    // Check that R1 is unchanged
    ctx.assert_register_eq(1, 3);
}

TEST_CASE(cmp_instruction_flags, "flags") {
    // Test CMP instruction flag setting
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 5
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 5
        0x0A, 0x00, 0x01,                     // CMP R0, R1
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that zero flag is set (5 - 5 = 0)
    ctx.assert_flag_set(FLAG_ZERO);

    // Check that sign flag is not set
    ctx.assert_flag_clear(FLAG_SIGN);
}

TEST_CASE(cmp_instruction_different, "flags") {
    // Test CMP instruction with R0 < R1
    ctx.load_program({
        0x01, 0x00, 0x03, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 3
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 5
        0x0A, 0x00, 0x01,                     // CMP R0, R1 (3 - 5 = -2)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that zero flag is not set
    ctx.assert_flag_clear(FLAG_ZERO);

    // Check that sign flag is set (negative result)
    ctx.assert_flag_set(FLAG_SIGN);
}

TEST_CASE_EXPECT_ERROR(division_by_zero, "arithmetic") {
    // Test that division by zero throws an error
    ctx.load_program({
        0x01, 0x00, 0x0A, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 10
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 0
        0x11, 0x00, 0x01,                     // DIV R0, R1
        0xFF                                  // HALT
    });

    // This should throw an exception or set error count
    ctx.execute_program();
}

TEST_CASE(memory_operations, "memory") {
    // Test that we can load and store values in memory
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 0x42
        0x07, 0x00, 0x10,                     // STORE R0, 0x10
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 0 (clear R1)
        0x06, 0x01, 0x10,                     // LOAD R1, 0x10
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check that value was stored and loaded correctly
    ctx.assert_register_eq(0, 0x42);
    ctx.assert_register_eq(1, 0x42);
    ctx.assert_memory_eq(0x10, 0x42);
}

TEST_CASE(program_counter_progression, "cpu") {
    // Test that PC advances correctly with simple instructions
    ctx.load_program({
        0x00,  // NOP
        0x00,  // NOP
        0x00,  // NOP
        0xFF   // HALT
    });

    // Execute step by step and check PC
    ctx.assert_pc_eq(0);

    ctx.execute_single_step();  // NOP
    ctx.assert_pc_eq(1);

    ctx.execute_single_step();  // NOP
    ctx.assert_pc_eq(2);

    ctx.execute_single_step();  // NOP
    ctx.assert_pc_eq(3);
}

TEST_CASE(stack_operations, "stack") {
    // Test PUSH and POP operations
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 66
        0x01, 0x01, 0x84, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 132
        0x08, 0x00,                           // PUSH R0
        0x08, 0x01,                           // PUSH R1
        0x09, 0x02,                           // POP R2
        0x09, 0x03,                           // POP R3
        0xFF                                  // HALT
    });

    uint32_t initial_sp = ctx.get_sp();
    ctx.execute_program();

    // Check that values were pushed and popped correctly
    ctx.assert_register_eq(2, 132);  // R2 should have R1's value
    ctx.assert_register_eq(3, 66);   // R3 should have R0's value

    // Stack pointer should be back to original position
    ctx.assert_sp_eq(initial_sp);
}

TEST_CASE(conditional_jumps, "control_flow") {
    // Test conditional jump (JZ after CMP)
    ctx.load_program({
        0x01, 0x00, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 5
        0x01, 0x01, 0x05, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 5
        0x0A, 0x00, 0x01,                     // CMP R0, R1
        0x0B, 0x1C, 0x00, 0x00, 0x00,        // JZ 28 (skip next instruction)
        0x01, 0x02, 0x99, 0x00, 0x00, 0x00,  // LOAD_IMM R2, 153 (should be skipped)
        0x01, 0x03, 0x77, 0x00, 0x00, 0x00,  // LOAD_IMM R3, 119
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // R2 should not be set (jump skipped it)
    ctx.assert_register_eq(2, 0);
    // R3 should be set
    ctx.assert_register_eq(3, 119);
    // Zero flag should be set from the CMP
    ctx.assert_flag_set(FLAG_ZERO);
}

TEST_CASE(memory_load_store, "memory") {
    // Test LOAD and STORE operations
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 66
        0x01, 0x01, 0x64, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 100 (memory address)
        0x07, 0x00, 0x01,                     // STORE R0, R1 (store R0 to address in R1)
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 0 (clear R0)
        0x06, 0x00, 0x01,                     // LOAD R0, R1 (load from address in R1)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // R0 should contain the value we stored and loaded back
    ctx.assert_register_eq(0, 66);
    // Memory at address 100 should contain 66
    ctx.assert_memory_eq(100, 66);
}

TEST_CASE(io_operations, "devices") {
    // Test input/output operations
    ctx.load_program({
        0x01, 0x00, 0x42, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 66
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 1 (console port)
        0x23, 0x00, 0x01,                     // OUT R0, R1 (output to console)
        0x22, 0x02, 0x01,                     // IN R2, R1 (read from console)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // After output, R0 should still contain 66
    ctx.assert_register_eq(0, 66);
    // The console device should have received the output
    // (Note: We can't easily test the actual device interaction in this simple test)
}

TEST_CASE(bitwise_operations, "bitwise") {
    // Test AND, OR, XOR operations
    ctx.load_program({
        0x01, 0x00, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 15 (0x0F)
        0x01, 0x01, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 51 (0x33)
        0x14, 0x00, 0x01,                     // AND R0, R1 (0x0F & 0x33 = 0x03)
        0x01, 0x02, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM R2, 15 (0x0F)
        0x01, 0x03, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM R3, 51 (0x33)
        0x15, 0x02, 0x03,                     // OR R2, R3 (0x0F | 0x33 = 0x3F)
        0x01, 0x04, 0x0F, 0x00, 0x00, 0x00,  // LOAD_IMM R4, 15 (0x0F)
        0x01, 0x05, 0x33, 0x00, 0x00, 0x00,  // LOAD_IMM R5, 51 (0x33)
        0x16, 0x04, 0x05,                     // XOR R4, R5 (0x0F ^ 0x33 = 0x3C)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check the results of bitwise operations
    ctx.assert_register_eq(0, 0x03);  // AND result
    ctx.assert_register_eq(2, 0x3F);  // OR result
    ctx.assert_register_eq(4, 0x3C);  // XOR result
}

TEST_CASE(shift_operations, "bitwise") {
    // Test SHL and SHR operations
    ctx.load_program({
        0x01, 0x00, 0x08, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 8
        0x01, 0x01, 0x02, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 2 (shift amount)
        0x18, 0x00, 0x01,                     // SHL R0, R1 (8 << 2 = 32)
        0x01, 0x02, 0x20, 0x00, 0x00, 0x00,  // LOAD_IMM R2, 32
        0x01, 0x03, 0x02, 0x00, 0x00, 0x00,  // LOAD_IMM R3, 2 (shift amount)
        0x19, 0x02, 0x03,                     // SHR R2, R3 (32 >> 2 = 8)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Check the results of shift operations
    ctx.assert_register_eq(0, 32);  // SHL result
    ctx.assert_register_eq(2, 8);   // SHR result
}

TEST_CASE(flags_comprehensive, "flags") {
    // Test various flag conditions
    ctx.load_program({
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  // LOAD_IMM R0, 0
        0x01, 0x01, 0x01, 0x00, 0x00, 0x00,  // LOAD_IMM R1, 1
        0x05, 0x00, 0x01,                     // CMP R0, R1 (should set carry flag)
        0xFF                                  // HALT
    });

    ctx.execute_program();

    // Comparing 0 with 1 should set the carry flag (0 < 1)
    ctx.assert_flag_set(FLAG_SIGN);
    // Zero flag should be clear (0 != 1)
    ctx.assert_flag_clear(FLAG_ZERO);
}
