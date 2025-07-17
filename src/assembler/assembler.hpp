#pragma once
#include "ast.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

namespace Assembler {

struct Symbol {
    std::string name;
    uint32_t address;
    bool defined;
    
    Symbol() : name(""), address(0), defined(false) {}  // Default constructor
    Symbol(const std::string& n, uint32_t addr = 0, bool def = false)
        : name(n), address(addr), defined(def) {}
};

class AssemblerEngine {
public:
    AssemblerEngine();
    
    // Main assembly function
    std::vector<uint8_t> assemble(const Program& program);
    
    // Error handling
    const std::vector<std::string>& get_errors() const { return errors; }
    bool has_errors() const { return !errors.empty(); }
    
    // Get symbol table for debugging
    const std::unordered_map<std::string, Symbol>& get_symbols() const { return symbol_table; }

private:
    std::vector<std::string> errors;
    std::unordered_map<std::string, Symbol> symbol_table;
    std::unordered_map<std::string, uint8_t> mnemonic_to_opcode;
    std::unordered_map<std::string, uint8_t> register_to_number;
    
    // Current assembly state
    uint32_t current_address;
    std::vector<uint8_t> bytecode;
    
    // Forward reference tracking
    struct ForwardRef {
        uint32_t address;      // Where to patch
        std::string symbol;    // Symbol name
        size_t size;          // Size of address field (1, 2, 4 bytes)
        bool relative;        // Is it a relative address?
    };
    std::vector<ForwardRef> forward_refs;
    
    // Assembly passes
    void first_pass(const Program& program);   // Collect symbols
    void second_pass(const Program& program);  // Generate code
    void resolve_forward_references();
    
    // Statement processing
    void process_label(const Label& label);
    void process_directive(const Directive& directive);
    void process_instruction(const Instruction& instruction);
    
    // Expression evaluation
    int64_t evaluate_expression(const Expression& expr, bool& is_symbol_ref, std::string& symbol_name);
    uint8_t get_register_number(const std::string& reg_name);
    uint8_t get_opcode(const std::string& mnemonic);
    
    // Code generation helpers
    void emit_byte(uint8_t byte);
    void emit_word(uint16_t word);
    void emit_dword(uint32_t dword);
    void emit_address(uint32_t address, size_t size = 4);
    void emit_forward_ref(const std::string& symbol, size_t size = 4, bool relative = false);
    
    // Instruction encoding
    void encode_instruction(const Instruction& instruction);
    size_t get_instruction_size(const std::string& mnemonic, const std::vector<std::unique_ptr<Expression>>& operands);
    
    // Data directive handling
    void handle_db_directive(const std::vector<std::unique_ptr<Expression>>& args);
    void handle_dw_directive(const std::vector<std::unique_ptr<Expression>>& args);
    void handle_dd_directive(const std::vector<std::unique_ptr<Expression>>& args);
    void handle_string_directive(const std::vector<std::unique_ptr<Expression>>& args);
    void handle_org_directive(const std::vector<std::unique_ptr<Expression>>& args);
    
    // Utility methods
    void add_error(const std::string& message);
    void add_error(const std::string& message, size_t line, size_t column);
    void init_opcode_table();
    void init_register_table();
};

} // namespace Assembler
