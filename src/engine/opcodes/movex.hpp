#pragma once
#include "opcode_handler.hpp"

// Extended register Move operation: MOVEX reg1, reg2
// Moves values between any of the 50 available registers (including R8-R15)
// Uses extended register encoding to access the full register set
void handle_movex(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
