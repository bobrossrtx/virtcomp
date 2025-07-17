#pragma once
#include "assembler.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace Assembler {

/**
 * Main assembler interface that combines lexing, parsing, and code generation
 */
class VirtCompAssembler {
public:
    VirtCompAssembler() = default;
    
    /**
     * Assemble assembly source code into bytecode
     * @param source Assembly source code
     * @return Bytecode vector (empty if errors occurred)
     */
    std::vector<uint8_t> assemble_string(const std::string& source);
    
    /**
     * Assemble assembly source file into bytecode
     * @param filename Path to assembly source file
     * @return Bytecode vector (empty if errors occurred)
     */
    std::vector<uint8_t> assemble_file(const std::string& filename);
    
    /**
     * Get all errors from the last assembly operation
     */
    const std::vector<std::string>& get_errors() const { return all_errors; }
    
    /**
     * Check if the last assembly operation had errors
     */
    bool has_errors() const { return !all_errors.empty(); }
    
    /**
     * Get symbol table from the last assembly operation
     */
    const std::unordered_map<std::string, Symbol>& get_symbols() const { return symbols; }
    
    /**
     * Clear all errors and reset state
     */
    void clear_errors();

private:
    std::vector<std::string> all_errors;
    std::unordered_map<std::string, Symbol> symbols;
    
    void collect_errors(const std::vector<std::string>& errors);
    std::string read_file(const std::string& filename);
};

/**
 * Convenience function to assemble a string of assembly code
 * @param source Assembly source code
 * @return Bytecode vector (empty if errors occurred)
 */
std::vector<uint8_t> assemble(const std::string& source);

/**
 * Convenience function to assemble an assembly file
 * @param filename Path to assembly source file
 * @return Bytecode vector (empty if errors occurred)
 */
std::vector<uint8_t> assemble_file(const std::string& filename);

} // namespace Assembler
