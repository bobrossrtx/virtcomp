#pragma once
#include "opcode_handler.hpp"

void handle_pop_arg(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
