#pragma once
#include <vector>
#include <cstdint>

// Forward declaration
class CPU;

// Base signature for all opcode handlers
// Each handler takes the CPU instance, program, and running flag by reference
// This allows handlers to modify CPU state and control execution flow
using OpcodeHandler = void(*)(CPU& cpu, const std::vector<uint8_t>& program, bool& running);
