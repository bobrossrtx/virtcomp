#pragma once
#include "opcode_handler.hpp"

void handle_outstr(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
