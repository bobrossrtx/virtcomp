#pragma once
#include "../cpu.hpp"
#include <vector>

// JNO - Jump if overflow flag not set
void handle_jno(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
