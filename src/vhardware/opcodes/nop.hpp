#pragma once
#include "opcode_handler.hpp"

void handle_nop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
