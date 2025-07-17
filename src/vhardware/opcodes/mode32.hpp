#pragma once
#include "opcode_handler.hpp"

// MODE32 opcode handler - Switch CPU to 32-bit mode
void handle_mode32(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
