#pragma once
#include "opcode_handler.hpp"

void handle_pop(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
