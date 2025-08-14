#pragma once
#include "opcode_handler.hpp"

// MODE64 opcode handler - Switch CPU to 64-bit mode
void handle_mode64(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
