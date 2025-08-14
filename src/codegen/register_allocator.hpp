#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <optional>
#include "x86_encoder.hpp"
#include "../engine/cpu_registers.hpp"

namespace CodeGen {

// Maps VirtComp's 134 registers to x86-64's 16 registers
class RegisterAllocator {
private:
    // Physical x86 registers available for allocation
    static constexpr X86Register ALLOCATABLE_REGS[] = {
        X86Register::RAX, X86Register::RCX, X86Register::RDX, X86Register::RBX,
        X86Register::RSI, X86Register::RDI, X86Register::R8,  X86Register::R9,
        X86Register::R10, X86Register::R11, X86Register::R12, X86Register::R13,
        X86Register::R14, X86Register::R15
        // RSP and RBP reserved for stack operations
    };

    // Current allocation state
    std::unordered_map<uint8_t, X86Register> virt_to_phys;  // VirtComp reg → x86 reg
    std::unordered_set<X86Register> used_regs;              // Currently allocated x86 regs
    std::unordered_map<uint8_t, bool> dirty_regs;           // Registers that need spilling

    // Spill management
    std::unordered_map<uint8_t, int32_t> spill_slots;       // VirtComp reg → stack offset
    int32_t next_spill_offset;

    // Statistics
    size_t spill_count;
    size_t allocation_count;

public:
    RegisterAllocator();

    // Core allocation interface
    X86Register allocate_register(uint8_t virt_reg);
    void free_register(uint8_t virt_reg);
    bool is_allocated(uint8_t virt_reg) const;

    // Get physical register for virtual register (may cause spill/reload)
    X86Register get_physical_register(uint8_t virt_reg, X86Encoder& encoder);

    // Spill management
    void spill_register(uint8_t virt_reg, X86Encoder& encoder);
    void reload_register(uint8_t virt_reg, X86Encoder& encoder);
    void spill_all_dirty(X86Encoder& encoder);

    // Register pressure management
    void mark_dirty(uint8_t virt_reg);
    void mark_clean(uint8_t virt_reg);
    X86Register evict_least_recently_used();

    // Function boundaries
    void reset_for_new_function();
    void save_caller_saved_regs(X86Encoder& encoder);
    void restore_caller_saved_regs(X86Encoder& encoder);

    // Debug and statistics
    void print_allocation_state() const;
    size_t get_spill_count() const { return spill_count; }
    size_t get_allocation_count() const { return allocation_count; }

private:
    std::optional<X86Register> find_free_register();
    int32_t allocate_spill_slot();
};

} // namespace CodeGen
