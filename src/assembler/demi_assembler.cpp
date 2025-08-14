#include "demi_assembler.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Assembler {

std::vector<uint8_t> DemiAssembler::assemble_string(const std::string& source) {
    clear_errors();
    
    // Lexical analysis
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.has_errors()) {
        collect_errors(lexer.get_errors());
        return {};
    }
    
    // Parsing
    Parser parser(tokens);
    auto program = parser.parse();
    
    if (parser.has_errors()) {
        collect_errors(parser.get_errors());
        return {};
    }
    
    // Code generation
    AssemblerEngine assembler;
    auto bytecode = assembler.assemble(*program);
    
    if (assembler.has_errors()) {
        collect_errors(assembler.get_errors());
        return {};
    }
    
    // Store symbols for debugging
    symbols = assembler.get_symbols();
    
    return bytecode;
}

std::vector<uint8_t> DemiAssembler::assemble_file(const std::string& filename) {
    std::string source = read_file(filename);
    if (source.empty() && has_errors()) {
        return {};
    }
    
    return assemble_string(source);
}

void DemiAssembler::clear_errors() {
    all_errors.clear();
    symbols.clear();
}

void DemiAssembler::collect_errors(const std::vector<std::string>& errors) {
    all_errors.insert(all_errors.end(), errors.begin(), errors.end());
}

std::string DemiAssembler::read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        all_errors.push_back("Cannot open file: " + filename);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Convenience functions
std::vector<uint8_t> assemble(const std::string& source) {
    DemiAssembler assembler;
    auto bytecode = assembler.assemble_string(source);
    
    if (assembler.has_errors()) {
        std::cerr << "Assembly errors:\n";
        for (const auto& error : assembler.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
    }
    
    return bytecode;
}

std::vector<uint8_t> assemble_file(const std::string& filename) {
    DemiAssembler assembler;
    auto bytecode = assembler.assemble_file(filename);
    
    if (assembler.has_errors()) {
        std::cerr << "Assembly errors:\n";
        for (const auto& error : assembler.get_errors()) {
            std::cerr << "  " << error << "\n";
        }
    }
    
    return bytecode;
}

} // namespace Assembler
