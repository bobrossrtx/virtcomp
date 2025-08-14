#pragma once
#include "opcode_handler.hpp"

// 64-bit Add operation: ADD64 reg1, reg2
// Adds the 64-bit value in reg2 to reg1, storing result in reg1
// Sets carry and overflow flags for 64-bit arithmetic
void handle_add64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
