#pragma once
#include "opcode_handler.hpp"

// 64-bit Move operation: MOV64 reg1, reg2
// Moves the 64-bit value from reg2 to reg1
void handle_mov64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
