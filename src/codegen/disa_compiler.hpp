#pragma once
#include <vector>
#include <unordered_map>
#include "x86_encoder.hpp"
#include "register_allocator.hpp"
#include "../vhardware/cpu.hpp"

namespace CodeGen {

// Translates D-ISA bytecode to native x86-64 machine code
class DISAToX86Compiler {
private:
    X86Encoder encoder;
    RegisterAllocator reg_alloc;
    
    // Jump target resolution
    struct JumpTarget {
        size_t bytecode_address;
        X86Encoder::Label x86_label;
    };
    std::unordered_map<size_t, JumpTarget> jump_targets;
    
    // Function call management
    std::vector<size_t> function_addresses;
    std::unordered_map<size_t, X86Encoder::Label> function_labels;
    
    // Current compilation state
    size_t current_bytecode_pos;
    const std::vector<uint8_t>* current_program;

public:
    DISAToX86Compiler() = default;
    
    // Main compilation interface
    std::vector<uint8_t> compile_program(const std::vector<uint8_t>& disa_bytecode);
    
    // Instruction translation methods
    void translate_instruction(Opcode opcode, const uint8_t* operands);
    
    // Individual instruction handlers
    void translate_nop();
    void translate_load_imm(uint8_t reg, uint64_t immediate);
    void translate_add(uint8_t dst_reg, uint8_t src_reg);
    void translate_sub(uint8_t dst_reg, uint8_t src_reg);
    void translate_mov(uint8_t dst_reg, uint8_t src_reg);
    void translate_cmp(uint8_t reg1, uint8_t reg2);
    void translate_jmp(uint32_t target_address);
    void translate_jz(uint32_t target_address);
    void translate_jnz(uint32_t target_address);
    void translate_load(uint8_t dst_reg, uint8_t addr_reg, int32_t offset);
    void translate_store(uint8_t addr_reg, int32_t offset, uint8_t src_reg);
    void translate_push(uint8_t reg);
    void translate_pop(uint8_t reg);
    void translate_call(uint32_t target_address);
    void translate_ret();
    void translate_halt();
    
    // Memory and stack management
    void setup_function_prologue();
    void setup_function_epilogue();
    void emit_stack_frame_setup(size_t local_vars_size);
    void emit_stack_frame_teardown();
    
    // Jump target management
    void scan_for_jump_targets(const std::vector<uint8_t>& bytecode);
    void resolve_jump_targets();
    X86Encoder::Label& get_or_create_label(size_t bytecode_address);
    
    // Runtime support (for complex operations)
    void emit_runtime_call(const char* function_name);
    void emit_device_io_call(uint16_t device_id, bool is_input);
    
    // Optimization passes
    void optimize_register_usage();
    void eliminate_redundant_moves();
    void fold_constant_operations();
    
    // Debug and analysis
    void print_compilation_stats() const;
    size_t get_code_size() const { return encoder.size(); }
    
private:
    // Helper methods
    uint8_t read_uint8(size_t& pos) const;
    uint16_t read_uint16(size_t& pos) const;
    uint32_t read_uint32(size_t& pos) const;
    uint64_t read_uint64(size_t& pos) const;
    
    // Instruction format parsing
    struct InstructionFormat {
        uint8_t opcode;
        uint8_t reg1;
        uint8_t reg2;
        uint64_t immediate;
        bool has_immediate;
    };
    InstructionFormat decode_instruction(size_t& pos) const;
};

} // namespace CodeGen
