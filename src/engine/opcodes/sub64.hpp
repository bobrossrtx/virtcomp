#pragma once
#include "opcode_handler.hpp"

// 64-bit Subtract operation: SUB64 reg1, reg2
// Subtracts the 64-bit value in reg2 from reg1, storing result in reg1
// Sets carry and overflow flags for 64-bit arithmetic
void handle_sub64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
