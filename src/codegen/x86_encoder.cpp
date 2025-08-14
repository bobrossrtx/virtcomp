#include "x86_encoder.hpp"
#include <cassert>

namespace CodeGen {

void X86Encoder::emit_rex(bool w, bool r, bool x, bool b) {
    uint8_t rex = 0x40;
    if (w) rex |= 0x08;  // 64-bit operand
    if (r) rex |= 0x04;  // Extension of ModR/M reg field
    if (x) rex |= 0x02;  // Extension of SIB index field  
    if (b) rex |= 0x01;  // Extension of ModR/M r/m field
    code_buffer.push_back(rex);
}

void X86Encoder::emit_rex_if_needed(X86Register reg1, X86Register reg2) {
    bool need_rex = (static_cast<uint8_t>(reg1) >= 8) || 
                    (static_cast<uint8_t>(reg2) >= 8);
    if (need_rex) {
        bool r = static_cast<uint8_t>(reg1) >= 8;
        bool b = static_cast<uint8_t>(reg2) >= 8;
        emit_rex(true, r, false, b);  // Always use 64-bit mode
    } else {
        emit_rex(true, false, false, false);  // Standard 64-bit prefix
    }
}

void X86Encoder::emit_modrm(uint8_t mod, uint8_t reg, uint8_t rm) {
    uint8_t modrm = (mod << 6) | ((reg & 0x7) << 3) | (rm & 0x7);
    code_buffer.push_back(modrm);
}

uint8_t X86Encoder::reg_to_modrm(X86Register reg) {
    return static_cast<uint8_t>(reg) & 0x7;  // Lower 3 bits
}

// Basic MOV reg, reg
void X86Encoder::emit_mov_reg_reg(X86Register dst, X86Register src) {
    emit_rex_if_needed(src, dst);  // REX.R for src, REX.B for dst
    code_buffer.push_back(0x89);   // MOV r/m64, r64
    emit_modrm(0b11, reg_to_modrm(src), reg_to_modrm(dst));
}

// MOV reg, imm64
void X86Encoder::emit_mov_reg_imm64(X86Register dst, uint64_t imm) {
    // REX prefix for 64-bit immediate
    emit_rex(true, false, false, static_cast<uint8_t>(dst) >= 8);
    
    // MOV opcode + register encoding
    code_buffer.push_back(0xB8 + (static_cast<uint8_t>(dst) & 0x7));
    
    // 64-bit immediate (little endian)
    for (int i = 0; i < 8; i++) {
        code_buffer.push_back((imm >> (i * 8)) & 0xFF);
    }
}

// ADD reg, reg
void X86Encoder::emit_add_reg_reg(X86Register dst, X86Register src) {
    emit_rex_if_needed(src, dst);
    code_buffer.push_back(0x01);   // ADD r/m64, r64
    emit_modrm(0b11, reg_to_modrm(src), reg_to_modrm(dst));
}

// SUB reg, reg  
void X86Encoder::emit_sub_reg_reg(X86Register dst, X86Register src) {
    emit_rex_if_needed(src, dst);
    code_buffer.push_back(0x29);   // SUB r/m64, r64
    emit_modrm(0b11, reg_to_modrm(src), reg_to_modrm(dst));
}

// CMP reg, reg
void X86Encoder::emit_cmp_reg_reg(X86Register left, X86Register right) {
    emit_rex_if_needed(right, left);
    code_buffer.push_back(0x39);   // CMP r/m64, r64
    emit_modrm(0b11, reg_to_modrm(right), reg_to_modrm(left));
}

// Memory operations
void X86Encoder::emit_mov_reg_mem(X86Register dst, X86Register base, int32_t offset) {
    emit_rex_if_needed(dst, base);
    code_buffer.push_back(0x8B);   // MOV r64, r/m64
    
    if (offset == 0 && base != X86Register::RBP) {
        // [reg] - no displacement
        emit_modrm(0b00, reg_to_modrm(dst), reg_to_modrm(base));
    } else if (offset >= -128 && offset <= 127) {
        // [reg + disp8]
        emit_modrm(0b01, reg_to_modrm(dst), reg_to_modrm(base));
        code_buffer.push_back(static_cast<uint8_t>(offset));
    } else {
        // [reg + disp32]
        emit_modrm(0b10, reg_to_modrm(dst), reg_to_modrm(base));
        for (int i = 0; i < 4; i++) {
            code_buffer.push_back((offset >> (i * 8)) & 0xFF);
        }
    }
}

void X86Encoder::emit_mov_mem_reg(X86Register base, int32_t offset, X86Register src) {
    emit_rex_if_needed(src, base);
    code_buffer.push_back(0x89);   // MOV r/m64, r64
    
    if (offset == 0 && base != X86Register::RBP) {
        emit_modrm(0b00, reg_to_modrm(src), reg_to_modrm(base));
    } else if (offset >= -128 && offset <= 127) {
        emit_modrm(0b01, reg_to_modrm(src), reg_to_modrm(base));
        code_buffer.push_back(static_cast<uint8_t>(offset));
    } else {
        emit_modrm(0b10, reg_to_modrm(src), reg_to_modrm(base));
        for (int i = 0; i < 4; i++) {
            code_buffer.push_back((offset >> (i * 8)) & 0xFF);
        }
    }
}

// Stack operations
void X86Encoder::emit_push_reg(X86Register reg) {
    if (static_cast<uint8_t>(reg) >= 8) {
        emit_rex(false, false, false, true);  // REX.B for extended registers
    }
    code_buffer.push_back(0x50 + (static_cast<uint8_t>(reg) & 0x7));
}

void X86Encoder::emit_pop_reg(X86Register reg) {
    if (static_cast<uint8_t>(reg) >= 8) {
        emit_rex(false, false, false, true);
    }
    code_buffer.push_back(0x58 + (static_cast<uint8_t>(reg) & 0x7));
}

// Control flow
void X86Encoder::emit_jmp_rel32(int32_t offset) {
    code_buffer.push_back(0xE9);   // JMP rel32
    for (int i = 0; i < 4; i++) {
        code_buffer.push_back((offset >> (i * 8)) & 0xFF);
    }
}

void X86Encoder::emit_jz_rel32(int32_t offset) {
    code_buffer.push_back(0x0F);   // Two-byte opcode prefix
    code_buffer.push_back(0x84);   // JZ rel32
    for (int i = 0; i < 4; i++) {
        code_buffer.push_back((offset >> (i * 8)) & 0xFF);
    }
}

void X86Encoder::emit_jnz_rel32(int32_t offset) {
    code_buffer.push_back(0x0F);   // Two-byte opcode prefix  
    code_buffer.push_back(0x85);   // JNZ rel32
    for (int i = 0; i < 4; i++) {
        code_buffer.push_back((offset >> (i * 8)) & 0xFF);
    }
}

void X86Encoder::emit_call_rel32(int32_t offset) {
    code_buffer.push_back(0xE8);   // CALL rel32
    for (int i = 0; i < 4; i++) {
        code_buffer.push_back((offset >> (i * 8)) & 0xFF);
    }
}

void X86Encoder::emit_ret() {
    code_buffer.push_back(0xC3);   // RET
}

// Utility
void X86Encoder::emit_nop() {
    code_buffer.push_back(0x90);   // NOP
}

void X86Encoder::emit_int3() {
    code_buffer.push_back(0xCC);   // INT 3 (breakpoint)
}

// Label management (simplified for now)
X86Encoder::Label X86Encoder::create_label() {
    return Label{0, false, {}};
}

void X86Encoder::bind_label(Label& label) {
    label.position = code_buffer.size();
    label.bound = true;
    
    // Resolve all pending jumps to this label
    for (size_t jump_pos : label.unresolved_jumps) {
        int32_t offset = static_cast<int32_t>(label.position - (jump_pos + 4));
        // Patch the 4-byte offset at jump_pos
        for (int i = 0; i < 4; i++) {
            code_buffer[jump_pos + i] = (offset >> (i * 8)) & 0xFF;
        }
    }
    label.unresolved_jumps.clear();
}

void X86Encoder::emit_jmp_label(Label& label) {
    if (label.bound) {
        int32_t offset = static_cast<int32_t>(label.position - (code_buffer.size() + 5));
        emit_jmp_rel32(offset);
    } else {
        // Forward reference - save position for later patching
        label.unresolved_jumps.push_back(code_buffer.size() + 1);  // +1 to skip opcode
        emit_jmp_rel32(0);  // Placeholder offset
    }
}

void X86Encoder::emit_jz_label(Label& label) {
    if (label.bound) {
        int32_t offset = static_cast<int32_t>(label.position - (code_buffer.size() + 6));
        emit_jz_rel32(offset);
    } else {
        label.unresolved_jumps.push_back(code_buffer.size() + 2);  // +2 for two-byte opcode
        emit_jz_rel32(0);
    }
}

void X86Encoder::emit_jnz_label(Label& label) {
    if (label.bound) {
        int32_t offset = static_cast<int32_t>(label.position - (code_buffer.size() + 6));
        emit_jnz_rel32(offset);
    } else {
        label.unresolved_jumps.push_back(code_buffer.size() + 2);
        emit_jnz_rel32(0);
    }
}

} // namespace CodeGen
