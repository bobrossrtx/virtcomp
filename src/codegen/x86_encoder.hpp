#pragma once
#include <vector>
#include <cstdint>
#include <string>

namespace CodeGen {

// x86-64 Register encoding
enum class X86Register : uint8_t {
    RAX = 0, RCX = 1, RDX = 2, RBX = 3,
    RSP = 4, RBP = 5, RSI = 6, RDI = 7,
    R8  = 8, R9  = 9, R10 = 10, R11 = 11,
    R12 = 12, R13 = 13, R14 = 14, R15 = 15
};

// x86-64 Instruction encoder
class X86Encoder {
private:
    std::vector<uint8_t> code_buffer;

    // REX prefix helpers
    void emit_rex(bool w, bool r, bool x, bool b);
    void emit_rex_if_needed(X86Register reg1, X86Register reg2 = X86Register::RAX);

    // ModR/M and SIB helpers
    void emit_modrm(uint8_t mod, uint8_t reg, uint8_t rm);
    uint8_t reg_to_modrm(X86Register reg);

public:
    X86Encoder() = default;

    // Basic instruction emission
    void emit_mov_reg_reg(X86Register dst, X86Register src);
    void emit_mov_reg_imm64(X86Register dst, uint64_t imm);
    void emit_add_reg_reg(X86Register dst, X86Register src);
    void emit_sub_reg_reg(X86Register dst, X86Register src);
    void emit_cmp_reg_reg(X86Register left, X86Register right);

    // Memory operations
    void emit_mov_reg_mem(X86Register dst, X86Register base, int32_t offset = 0);
    void emit_mov_mem_reg(X86Register base, int32_t offset, X86Register src);

    // Stack operations
    void emit_push_reg(X86Register reg);
    void emit_pop_reg(X86Register reg);

    // Control flow
    void emit_jmp_rel32(int32_t offset);
    void emit_jz_rel32(int32_t offset);
    void emit_jnz_rel32(int32_t offset);
    void emit_call_rel32(int32_t offset);
    void emit_ret();

    // Program structure
    void emit_nop();
    void emit_int3();  // Breakpoint for debugging

    // Code buffer management
    const std::vector<uint8_t>& get_code() const { return code_buffer; }
    void clear() { code_buffer.clear(); }
    size_t size() const { return code_buffer.size(); }

    // Label/jump management for forward references
    struct Label {
        size_t position;
        bool bound;
        std::vector<size_t> unresolved_jumps;
    };

    Label create_label();
    void bind_label(Label& label);
    void emit_jmp_label(Label& label);
    void emit_jz_label(Label& label);
    void emit_jnz_label(Label& label);
};

} // namespace CodeGen
