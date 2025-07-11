#pragma once
#include "../cpu.hpp"
#include <vector>

// JO - Jump if overflow flag set
void handle_jo(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
