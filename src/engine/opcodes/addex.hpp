#pragma once
#include "opcode_handler.hpp"

// Extended register Add operation: ADDEX reg1, reg2
// Adds values between any of the 50 available registers (including R8-R15)
// Uses extended register encoding with 64-bit arithmetic
void handle_addex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
