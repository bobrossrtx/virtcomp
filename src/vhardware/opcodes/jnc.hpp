#pragma once
#include "../cpu.hpp"
#include <vector>
#include <cstdint>

void handle_jnc(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
