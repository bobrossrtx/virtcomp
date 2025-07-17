#pragma once
#include "token.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>

namespace Assembler {

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();
    const std::vector<std::string>& get_errors() const { return errors; }
    bool has_errors() const { return !errors.empty(); }

private:
    const std::string& source;
    size_t pos;
    size_t line;
    size_t column;
    std::vector<std::string> errors;

    // Keyword and mnemonic tables
    static std::unordered_map<std::string, TokenType> keywords;
    static std::unordered_map<std::string, TokenType> mnemonics;
    static std::unordered_map<std::string, TokenType> registers;

    // Initialize static tables
    static void init_tables();
    static bool tables_initialized;

    // Helper methods
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    void skip_comment();

    // Token parsing methods
    Token parse_identifier();
    Token parse_number();
    Token parse_string();
    Token parse_directive();

    // Utility methods
    bool is_identifier_start(char c) const;
    bool is_identifier_part(char c) const;
    bool is_digit(char c) const;
    bool is_hex_digit(char c) const;
    bool is_binary_digit(char c) const;

    void add_error(const std::string& message);
};

} // namespace Assembler
