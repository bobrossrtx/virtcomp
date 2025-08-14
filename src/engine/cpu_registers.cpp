#include "cpu_registers.hpp"
#include <unordered_map>

namespace DemiEngine_Registers {

// Register information table
const RegisterInfo REGISTER_INFO[] = {
    // General Purpose Registers (0-15)
    {Register::RAX, "RAX", "Accumulator register", true, false, false, false, false, false, false},
    {Register::RCX, "RCX", "Counter register", true, false, false, false, false, false, false},
    {Register::RDX, "RDX", "Data register", true, false, false, false, false, false, false},
    {Register::RBX, "RBX", "Base register", true, false, false, false, false, false, false},
    {Register::RSP, "RSP", "Stack pointer", true, false, false, false, false, false, false},
    {Register::RBP, "RBP", "Base pointer", true, false, false, false, false, false, false},
    {Register::RSI, "RSI", "Source index", true, false, false, false, false, false, false},
    {Register::RDI, "RDI", "Destination index", true, false, false, false, false, false, false},
    {Register::R8, "R8", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R9, "R9", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R10, "R10", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R11, "R11", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R12, "R12", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R13, "R13", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R14, "R14", "Extended general purpose", true, false, false, false, false, false, false},
    {Register::R15, "R15", "Extended general purpose", true, false, false, false, false, false, false},

    // Segment Registers (16-21)
    {Register::CS, "CS", "Code segment", false, false, false, false, true, false, false},
    {Register::DS, "DS", "Data segment", false, false, false, false, true, false, false},
    {Register::ES, "ES", "Extra segment", false, false, false, false, true, false, false},
    {Register::FS, "FS", "General segment F", false, false, false, false, true, false, false},
    {Register::GS, "GS", "General segment G", false, false, false, false, true, false, false},
    {Register::SS, "SS", "Stack segment", false, false, false, false, true, false, false},

    // Control Registers (22-30)
    {Register::CR0, "CR0", "Control register 0", true, false, false, false, true, false, false},
    {Register::CR1, "CR1", "Control register 1", true, false, false, false, true, false, false},
    {Register::CR2, "CR2", "Control register 2", true, false, false, false, true, false, false},
    {Register::CR3, "CR3", "Control register 3", true, false, false, false, true, false, false},
    {Register::CR4, "CR4", "Control register 4", true, false, false, false, true, false, false},
    {Register::CR5, "CR5", "Control register 5", true, false, false, false, true, false, false},
    {Register::CR6, "CR6", "Control register 6", true, false, false, false, true, false, false},
    {Register::CR7, "CR7", "Control register 7", true, false, false, false, true, false, false},
    {Register::CR8, "CR8", "Control register 8", true, false, false, false, true, false, false},

    // Debug Registers (31-46)
    {Register::DR0, "DR0", "Debug register 0", true, false, false, false, true, false, false},
    {Register::DR1, "DR1", "Debug register 1", true, false, false, false, true, false, false},
    {Register::DR2, "DR2", "Debug register 2", true, false, false, false, true, false, false},
    {Register::DR3, "DR3", "Debug register 3", true, false, false, false, true, false, false},
    {Register::DR4, "DR4", "Debug register 4", true, false, false, false, true, false, false},
    {Register::DR5, "DR5", "Debug register 5", true, false, false, false, true, false, false},
    {Register::DR6, "DR6", "Debug register 6", true, false, false, false, true, false, false},
    {Register::DR7, "DR7", "Debug register 7", true, false, false, false, true, false, false},
    {Register::DR8, "DR8", "Debug register 8", true, false, false, false, true, false, false},
    {Register::DR9, "DR9", "Debug register 9", true, false, false, false, true, false, false},
    {Register::DR10, "DR10", "Debug register 10", true, false, false, false, true, false, false},
    {Register::DR11, "DR11", "Debug register 11", true, false, false, false, true, false, false},
    {Register::DR12, "DR12", "Debug register 12", true, false, false, false, true, false, false},
    {Register::DR13, "DR13", "Debug register 13", true, false, false, false, true, false, false},
    {Register::DR14, "DR14", "Debug register 14", true, false, false, false, true, false, false},
    {Register::DR15, "DR15", "Debug register 15", true, false, false, false, true, false, false},

    // Special Purpose Registers (47-49)
    {Register::RIP, "RIP", "Instruction pointer", true, false, false, false, false, false, false},
    {Register::RFLAGS, "RFLAGS", "64-bit flags register", true, false, false, false, false, false, false},
    {Register::MSW, "MSW", "Machine status word", false, false, false, false, true, false, false},

    // SIMD XMM Registers (50-81) - 128-bit registers stored as 2x64-bit
    {Register::XMM0, "XMM0", "SIMD register 0 (low)", true, true, false, false, false, true, false},
    {Register::XMM0_HIGH, "XMM0_HIGH", "SIMD register 0 (high)", true, true, false, false, false, true, false},
    {Register::XMM1, "XMM1", "SIMD register 1 (low)", true, true, false, false, false, true, false},
    {Register::XMM1_HIGH, "XMM1_HIGH", "SIMD register 1 (high)", true, true, false, false, false, true, false},
    {Register::XMM2, "XMM2", "SIMD register 2 (low)", true, true, false, false, false, true, false},
    {Register::XMM2_HIGH, "XMM2_HIGH", "SIMD register 2 (high)", true, true, false, false, false, true, false},
    {Register::XMM3, "XMM3", "SIMD register 3 (low)", true, true, false, false, false, true, false},
    {Register::XMM3_HIGH, "XMM3_HIGH", "SIMD register 3 (high)", true, true, false, false, false, true, false},
    {Register::XMM4, "XMM4", "SIMD register 4 (low)", true, true, false, false, false, true, false},
    {Register::XMM4_HIGH, "XMM4_HIGH", "SIMD register 4 (high)", true, true, false, false, false, true, false},
    {Register::XMM5, "XMM5", "SIMD register 5 (low)", true, true, false, false, false, true, false},
    {Register::XMM5_HIGH, "XMM5_HIGH", "SIMD register 5 (high)", true, true, false, false, false, true, false},
    {Register::XMM6, "XMM6", "SIMD register 6 (low)", true, true, false, false, false, true, false},
    {Register::XMM6_HIGH, "XMM6_HIGH", "SIMD register 6 (high)", true, true, false, false, false, true, false},
    {Register::XMM7, "XMM7", "SIMD register 7 (low)", true, true, false, false, false, true, false},
    {Register::XMM7_HIGH, "XMM7_HIGH", "SIMD register 7 (high)", true, true, false, false, false, true, false},
    {Register::XMM8, "XMM8", "SIMD register 8 (low)", true, true, false, false, false, true, false},
    {Register::XMM8_HIGH, "XMM8_HIGH", "SIMD register 8 (high)", true, true, false, false, false, true, false},
    {Register::XMM9, "XMM9", "SIMD register 9 (low)", true, true, false, false, false, true, false},
    {Register::XMM9_HIGH, "XMM9_HIGH", "SIMD register 9 (high)", true, true, false, false, false, true, false},
    {Register::XMM10, "XMM10", "SIMD register 10 (low)", true, true, false, false, false, true, false},
    {Register::XMM10_HIGH, "XMM10_HIGH", "SIMD register 10 (high)", true, true, false, false, false, true, false},
    {Register::XMM11, "XMM11", "SIMD register 11 (low)", true, true, false, false, false, true, false},
    {Register::XMM11_HIGH, "XMM11_HIGH", "SIMD register 11 (high)", true, true, false, false, false, true, false},
    {Register::XMM12, "XMM12", "SIMD register 12 (low)", true, true, false, false, false, true, false},
    {Register::XMM12_HIGH, "XMM12_HIGH", "SIMD register 12 (high)", true, true, false, false, false, true, false},
    {Register::XMM13, "XMM13", "SIMD register 13 (low)", true, true, false, false, false, true, false},
    {Register::XMM13_HIGH, "XMM13_HIGH", "SIMD register 13 (high)", true, true, false, false, false, true, false},
    {Register::XMM14, "XMM14", "SIMD register 14 (low)", true, true, false, false, false, true, false},
    {Register::XMM14_HIGH, "XMM14_HIGH", "SIMD register 14 (high)", true, true, false, false, false, true, false},
    {Register::XMM15, "XMM15", "SIMD register 15 (low)", true, true, false, false, false, true, false},
    {Register::XMM15_HIGH, "XMM15_HIGH", "SIMD register 15 (high)", true, true, false, false, false, true, false},

    // FPU Stack Registers (82-97) - 80-bit floating point stored as 64-bit + metadata
    {Register::ST0, "ST0", "FPU stack register 0", true, false, false, true, false, false, true},
    {Register::ST0_META, "ST0_META", "FPU stack register 0 metadata", true, false, false, true, false, false, true},
    {Register::ST1, "ST1", "FPU stack register 1", true, false, false, true, false, false, true},
    {Register::ST1_META, "ST1_META", "FPU stack register 1 metadata", true, false, false, true, false, false, true},
    {Register::ST2, "ST2", "FPU stack register 2", true, false, false, true, false, false, true},
    {Register::ST2_META, "ST2_META", "FPU stack register 2 metadata", true, false, false, true, false, false, true},
    {Register::ST3, "ST3", "FPU stack register 3", true, false, false, true, false, false, true},
    {Register::ST3_META, "ST3_META", "FPU stack register 3 metadata", true, false, false, true, false, false, true},
    {Register::ST4, "ST4", "FPU stack register 4", true, false, false, true, false, false, true},
    {Register::ST4_META, "ST4_META", "FPU stack register 4 metadata", true, false, false, true, false, false, true},
    {Register::ST5, "ST5", "FPU stack register 5", true, false, false, true, false, false, true},
    {Register::ST5_META, "ST5_META", "FPU stack register 5 metadata", true, false, false, true, false, false, true},
    {Register::ST6, "ST6", "FPU stack register 6", true, false, false, true, false, false, true},
    {Register::ST6_META, "ST6_META", "FPU stack register 6 metadata", true, false, false, true, false, false, true},
    {Register::ST7, "ST7", "FPU stack register 7", true, false, false, true, false, false, true},
    {Register::ST7_META, "ST7_META", "FPU stack register 7 metadata", true, false, false, true, false, false, true},

    // SIMD Control and Status Registers (98-101)
    {Register::MXCSR, "MXCSR", "SIMD Control and Status Register", true, false, false, false, false, false, false},
    {Register::FPU_CONTROL, "FCW", "FPU Control Word", true, false, false, false, false, false, true},
    {Register::FPU_STATUS, "FSW", "FPU Status Word", true, false, false, false, false, false, true},
    {Register::FPU_TAG, "FTW", "FPU Tag Word", true, false, false, false, false, false, true},

    // AVX YMM Upper 128-bit parts (102-133) - 256-bit registers
    {Register::YMM0_HIGH2, "YMM0_HIGH2", "AVX register 0 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM0_HIGH3, "YMM0_HIGH3", "AVX register 0 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM1_HIGH2, "YMM1_HIGH2", "AVX register 1 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM1_HIGH3, "YMM1_HIGH3", "AVX register 1 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM2_HIGH2, "YMM2_HIGH2", "AVX register 2 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM2_HIGH3, "YMM2_HIGH3", "AVX register 2 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM3_HIGH2, "YMM3_HIGH2", "AVX register 3 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM3_HIGH3, "YMM3_HIGH3", "AVX register 3 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM4_HIGH2, "YMM4_HIGH2", "AVX register 4 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM4_HIGH3, "YMM4_HIGH3", "AVX register 4 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM5_HIGH2, "YMM5_HIGH2", "AVX register 5 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM5_HIGH3, "YMM5_HIGH3", "AVX register 5 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM6_HIGH2, "YMM6_HIGH2", "AVX register 6 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM6_HIGH3, "YMM6_HIGH3", "AVX register 6 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM7_HIGH2, "YMM7_HIGH2", "AVX register 7 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM7_HIGH3, "YMM7_HIGH3", "AVX register 7 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM8_HIGH2, "YMM8_HIGH2", "AVX register 8 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM8_HIGH3, "YMM8_HIGH3", "AVX register 8 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM9_HIGH2, "YMM9_HIGH2", "AVX register 9 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM9_HIGH3, "YMM9_HIGH3", "AVX register 9 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM10_HIGH2, "YMM10_HIGH2", "AVX register 10 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM10_HIGH3, "YMM10_HIGH3", "AVX register 10 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM11_HIGH2, "YMM11_HIGH2", "AVX register 11 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM11_HIGH3, "YMM11_HIGH3", "AVX register 11 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM12_HIGH2, "YMM12_HIGH2", "AVX register 12 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM12_HIGH3, "YMM12_HIGH3", "AVX register 12 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM13_HIGH2, "YMM13_HIGH2", "AVX register 13 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM13_HIGH3, "YMM13_HIGH3", "AVX register 13 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM14_HIGH2, "YMM14_HIGH2", "AVX register 14 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM14_HIGH3, "YMM14_HIGH3", "AVX register 14 (bits 192-255)", true, false, true, false, false, true, false},
    {Register::YMM15_HIGH2, "YMM15_HIGH2", "AVX register 15 (bits 128-191)", true, false, true, false, false, true, false},
    {Register::YMM15_HIGH3, "YMM15_HIGH3", "AVX register 15 (bits 192-255)", true, false, true, false, false, true, false},
};

std::string RegisterNames::get_name(Register reg) {
    auto index = static_cast<size_t>(reg);
    if (index < TOTAL_REGISTERS) {
        return REGISTER_INFO[index].name;
    }
    return "UNKNOWN";
}

Register RegisterNames::from_name(const std::string& name) {
    // Create a static map for efficient lookup
    static std::unordered_map<std::string, Register> name_map;
    if (name_map.empty()) {
        for (size_t i = 0; i < TOTAL_REGISTERS; ++i) {
            name_map[REGISTER_INFO[i].name] = static_cast<Register>(i);

            // Add legacy R0-R7 aliases
            if (i < 8) {
                name_map["R" + std::to_string(i)] = static_cast<Register>(i);
            }
        }

        // Add additional common aliases
        name_map["EAX"] = Register::RAX;  // 32-bit alias
        name_map["EBX"] = Register::RBX;
        name_map["ECX"] = Register::RCX;
        name_map["EDX"] = Register::RDX;
        name_map["ESP"] = Register::RSP;
        name_map["EBP"] = Register::RBP;
        name_map["ESI"] = Register::RSI;
        name_map["EDI"] = Register::RDI;

        // 16-bit aliases
        name_map["AX"] = Register::RAX;
        name_map["BX"] = Register::RBX;
        name_map["CX"] = Register::RCX;
        name_map["DX"] = Register::RDX;
        name_map["SP"] = Register::RSP;
        name_map["BP"] = Register::RBP;
        name_map["SI"] = Register::RSI;
        name_map["DI"] = Register::RDI;

        // 8-bit aliases (high and low)
        name_map["AL"] = Register::RAX;
        name_map["AH"] = Register::RAX;
        name_map["BL"] = Register::RBX;
        name_map["BH"] = Register::RBX;
        name_map["CL"] = Register::RCX;
        name_map["CH"] = Register::RCX;
        name_map["DL"] = Register::RDX;
        name_map["DH"] = Register::RDX;

        // MMX aliases (same as FPU ST registers)
        name_map["MM0"] = Register::MM0;
        name_map["MM1"] = Register::MM1;
        name_map["MM2"] = Register::MM2;
        name_map["MM3"] = Register::MM3;
        name_map["MM4"] = Register::MM4;
        name_map["MM5"] = Register::MM5;
        name_map["MM6"] = Register::MM6;
        name_map["MM7"] = Register::MM7;

        // YMM aliases (for AVX - use XMM as lower 128 bits)
        name_map["YMM0"] = Register::XMM0;
        name_map["YMM1"] = Register::XMM1;
        name_map["YMM2"] = Register::XMM2;
        name_map["YMM3"] = Register::XMM3;
        name_map["YMM4"] = Register::XMM4;
        name_map["YMM5"] = Register::XMM5;
        name_map["YMM6"] = Register::XMM6;
        name_map["YMM7"] = Register::XMM7;
        name_map["YMM8"] = Register::XMM8;
        name_map["YMM9"] = Register::XMM9;
        name_map["YMM10"] = Register::XMM10;
        name_map["YMM11"] = Register::XMM11;
        name_map["YMM12"] = Register::XMM12;
        name_map["YMM13"] = Register::XMM13;
        name_map["YMM14"] = Register::XMM14;
        name_map["YMM15"] = Register::XMM15;
    }

    auto it = name_map.find(name);
    if (it != name_map.end()) {
        return it->second;
    }

    return Register::RAX; // Default fallback
}

bool RegisterNames::is_general_purpose(Register reg) {
    return static_cast<size_t>(reg) < GENERAL_PURPOSE_COUNT;
}

bool RegisterNames::is_segment(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 16 && index < 22;
}

bool RegisterNames::is_control(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 22 && index < 31;
}

bool RegisterNames::is_debug(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 31 && index < 47;
}

bool RegisterNames::is_special(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 47 && index < 50;
}

bool RegisterNames::is_simd(Register reg) {
    auto index = static_cast<size_t>(reg);
    return (index >= 50 && index < 82) || (index >= 102 && index < 134); // XMM and YMM upper parts
}

bool RegisterNames::is_fpu(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 82 && index < 98; // ST0-ST7 and metadata
}

bool RegisterNames::is_mmx(Register reg) {
    // MMX registers are aliases to FPU registers
    return reg == Register::MM0 || reg == Register::MM1 || reg == Register::MM2 || reg == Register::MM3 ||
           reg == Register::MM4 || reg == Register::MM5 || reg == Register::MM6 || reg == Register::MM7;
}

bool RegisterNames::is_avx(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 102 && index < 134; // YMM upper 128-bit parts
}

bool RegisterNames::is_simd_control(Register reg) {
    auto index = static_cast<size_t>(reg);
    return index >= 98 && index < 102; // MXCSR, FPU control registers
}

} // namespace DemiEngine_Registers
