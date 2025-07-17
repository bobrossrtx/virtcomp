#pragma once
#include "opcode_handler.hpp"

// 64-bit Load Immediate operation: LOAD_IMM64 reg, imm64
// Loads a 64-bit immediate value into the specified register
// Format: opcode(1) + reg(1) + imm64(8) = 10 bytes total
void handle_load_imm64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
