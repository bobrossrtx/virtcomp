#pragma once
#include "opcode_handler.hpp"

// Extended register Subtract operation: SUBEX reg1, reg2
// Subtracts values between any of the 50 available registers (including R8-R15)
// Uses extended register encoding with 64-bit arithmetic
void handle_subex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
