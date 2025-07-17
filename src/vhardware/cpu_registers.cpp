#include "cpu_registers.hpp"
#include <unordered_map>

namespace VirtComp_Registers {

// Register information table
const RegisterInfo REGISTER_INFO[] = {
    // General Purpose Registers
    {Register::RAX, "RAX", "Accumulator register", true, false},
    {Register::RCX, "RCX", "Counter register", true, false},
    {Register::RDX, "RDX", "Data register", true, false},
    {Register::RBX, "RBX", "Base register", true, false},
    {Register::RSP, "RSP", "Stack pointer", true, false},
    {Register::RBP, "RBP", "Base pointer", true, false},
    {Register::RSI, "RSI", "Source index", true, false},
    {Register::RDI, "RDI", "Destination index", true, false},
    {Register::R8, "R8", "Extended general purpose", true, false},
    {Register::R9, "R9", "Extended general purpose", true, false},
    {Register::R10, "R10", "Extended general purpose", true, false},
    {Register::R11, "R11", "Extended general purpose", true, false},
    {Register::R12, "R12", "Extended general purpose", true, false},
    {Register::R13, "R13", "Extended general purpose", true, false},
    {Register::R14, "R14", "Extended general purpose", true, false},
    {Register::R15, "R15", "Extended general purpose", true, false},

    // Segment Registers
    {Register::CS, "CS", "Code segment", false, true},
    {Register::DS, "DS", "Data segment", false, true},
    {Register::ES, "ES", "Extra segment", false, true},
    {Register::FS, "FS", "General segment F", false, true},
    {Register::GS, "GS", "General segment G", false, true},
    {Register::SS, "SS", "Stack segment", false, true},

    // Control Registers
    {Register::CR0, "CR0", "Control register 0", true, true},
    {Register::CR1, "CR1", "Control register 1", true, true},
    {Register::CR2, "CR2", "Control register 2", true, true},
    {Register::CR3, "CR3", "Control register 3", true, true},
    {Register::CR4, "CR4", "Control register 4", true, true},
    {Register::CR5, "CR5", "Control register 5", true, true},
    {Register::CR6, "CR6", "Control register 6", true, true},
    {Register::CR7, "CR7", "Control register 7", true, true},
    {Register::CR8, "CR8", "Control register 8", true, true},

    // Debug Registers
    {Register::DR0, "DR0", "Debug register 0", true, true},
    {Register::DR1, "DR1", "Debug register 1", true, true},
    {Register::DR2, "DR2", "Debug register 2", true, true},
    {Register::DR3, "DR3", "Debug register 3", true, true},
    {Register::DR4, "DR4", "Debug register 4", true, true},
    {Register::DR5, "DR5", "Debug register 5", true, true},
    {Register::DR6, "DR6", "Debug register 6", true, true},
    {Register::DR7, "DR7", "Debug register 7", true, true},
    {Register::DR8, "DR8", "Debug register 8", true, true},
    {Register::DR9, "DR9", "Debug register 9", true, true},
    {Register::DR10, "DR10", "Debug register 10", true, true},
    {Register::DR11, "DR11", "Debug register 11", true, true},
    {Register::DR12, "DR12", "Debug register 12", true, true},
    {Register::DR13, "DR13", "Debug register 13", true, true},
    {Register::DR14, "DR14", "Debug register 14", true, true},
    {Register::DR15, "DR15", "Debug register 15", true, true},

    // Special Purpose Registers
    {Register::RIP, "RIP", "Instruction pointer", true, false},
    {Register::RFLAGS, "RFLAGS", "64-bit flags register", true, false},
    {Register::MSW, "MSW", "Machine status word", false, true},
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
    return index >= 47 && index < TOTAL_REGISTERS;
}

} // namespace VirtComp_Registers
