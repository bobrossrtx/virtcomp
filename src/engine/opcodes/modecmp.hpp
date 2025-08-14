#pragma once
#include "opcode_handler.hpp"

// MODECMP opcode handler - Compare current CPU mode
void handle_modecmp(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
