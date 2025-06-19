#pragma once
#include "opcode_handler.hpp"

void handle_push_flag(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
