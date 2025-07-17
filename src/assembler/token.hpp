#pragma once
#include <string>
#include <variant>
#include <cstdint>

namespace Assembler {

enum class TokenType {
    // Literals
    IDENTIFIER,      // labels, symbols
    NUMBER,          // numeric literals (decimal, hex, binary)
    STRING,          // string literals
    REGISTER,        // register names (R0, RAX, etc.)
    
    // Instruction mnemonics
    MNEMONIC,        // instruction names (ADD, MOV, JMP, etc.)
    
    // Symbols and operators
    COMMA,           // ,
    COLON,           // :
    SEMICOLON,       // ;
    DOT,             // .
    HASH,            // #
    
    // Addressing modes
    LBRACKET,        // [
    RBRACKET,        // ]
    PLUS,            // +
    MINUS,           // -
    ASTERISK,        // *
    
    // Directives
    DIRECTIVE,       // .data, .text, .org, etc.
    
    // Special
    NEWLINE,         // \n
    END_OF_FILE,     // EOF
    INVALID          // invalid token
};

struct Token {
    TokenType type;
    std::string text;
    std::variant<int64_t, uint64_t, double, std::string> value;
    size_t line;
    size_t column;
    
    Token(TokenType t, const std::string& txt, size_t ln = 0, size_t col = 0)
        : type(t), text(txt), line(ln), column(col) {}
    
    // Helper methods for value access
    bool is_number() const { return std::holds_alternative<int64_t>(value) || std::holds_alternative<uint64_t>(value); }
    bool is_string() const { return std::holds_alternative<std::string>(value); }
    
    int64_t as_int() const { 
        if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value);
        if (std::holds_alternative<uint64_t>(value)) return static_cast<int64_t>(std::get<uint64_t>(value));
        return 0;
    }
    
    uint64_t as_uint() const { 
        if (std::holds_alternative<uint64_t>(value)) return std::get<uint64_t>(value);
        if (std::holds_alternative<int64_t>(value)) return static_cast<uint64_t>(std::get<int64_t>(value));
        return 0;
    }
    
    std::string as_string() const {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        return text;
    }
};

} // namespace Assembler
