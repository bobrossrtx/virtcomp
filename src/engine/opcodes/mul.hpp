#pragma once
#include "opcode_handler.hpp"

void handle_mul(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
