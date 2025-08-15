#include "lexer.hpp"
#include <algorithm>

namespace Assembler {

// Static member initialization
std::unordered_map<std::string, TokenType> Lexer::keywords;
std::unordered_map<std::string, TokenType> Lexer::mnemonics;
std::unordered_map<std::string, TokenType> Lexer::registers;
bool Lexer::tables_initialized = false;

void Lexer::init_tables() {
    if (tables_initialized) return;
    
    // Assembly directives
    keywords[".data"] = TokenType::DIRECTIVE;
    keywords[".text"] = TokenType::DIRECTIVE;
    keywords[".org"] = TokenType::DIRECTIVE;
    keywords[".equ"] = TokenType::DIRECTIVE;
    keywords[".include"] = TokenType::DIRECTIVE;
    keywords[".db"] = TokenType::DIRECTIVE;
    keywords[".dw"] = TokenType::DIRECTIVE;
    keywords[".dd"] = TokenType::DIRECTIVE;
    keywords[".string"] = TokenType::DIRECTIVE;
    keywords[".end"] = TokenType::DIRECTIVE;
    
    // DemiEngine instruction mnemonics (matching the CPU opcodes)
    mnemonics["NOP"] = TokenType::MNEMONIC;
    mnemonics["LOAD_IMM"] = TokenType::MNEMONIC;
    mnemonics["ADD"] = TokenType::MNEMONIC;
    mnemonics["SUB"] = TokenType::MNEMONIC;
    mnemonics["MOV"] = TokenType::MNEMONIC;
    mnemonics["JMP"] = TokenType::MNEMONIC;
    mnemonics["LOAD"] = TokenType::MNEMONIC;
    mnemonics["STORE"] = TokenType::MNEMONIC;
    mnemonics["PUSH"] = TokenType::MNEMONIC;
    mnemonics["POP"] = TokenType::MNEMONIC;
    mnemonics["CMP"] = TokenType::MNEMONIC;
    mnemonics["JZ"] = TokenType::MNEMONIC;
    mnemonics["JNZ"] = TokenType::MNEMONIC;
    mnemonics["JS"] = TokenType::MNEMONIC;
    mnemonics["JNS"] = TokenType::MNEMONIC;
    mnemonics["JC"] = TokenType::MNEMONIC;
    mnemonics["JNC"] = TokenType::MNEMONIC;
    mnemonics["JO"] = TokenType::MNEMONIC;
    mnemonics["JNO"] = TokenType::MNEMONIC;
    mnemonics["JG"] = TokenType::MNEMONIC;
    mnemonics["JL"] = TokenType::MNEMONIC;
    mnemonics["JGE"] = TokenType::MNEMONIC;
    mnemonics["JLE"] = TokenType::MNEMONIC;
    mnemonics["MUL"] = TokenType::MNEMONIC;
    mnemonics["DIV"] = TokenType::MNEMONIC;
    mnemonics["INC"] = TokenType::MNEMONIC;
    mnemonics["DEC"] = TokenType::MNEMONIC;
    mnemonics["AND"] = TokenType::MNEMONIC;
    mnemonics["OR"] = TokenType::MNEMONIC;
    mnemonics["XOR"] = TokenType::MNEMONIC;
    mnemonics["NOT"] = TokenType::MNEMONIC;
    mnemonics["SHL"] = TokenType::MNEMONIC;
    mnemonics["SHR"] = TokenType::MNEMONIC;
    mnemonics["CALL"] = TokenType::MNEMONIC;
    mnemonics["RET"] = TokenType::MNEMONIC;
    mnemonics["PUSH_ARG"] = TokenType::MNEMONIC;
    mnemonics["POP_ARG"] = TokenType::MNEMONIC;
    mnemonics["PUSH_FLAG"] = TokenType::MNEMONIC;
    mnemonics["POP_FLAG"] = TokenType::MNEMONIC;
    mnemonics["LEA"] = TokenType::MNEMONIC;
    mnemonics["SWAP"] = TokenType::MNEMONIC;
    mnemonics["IN"] = TokenType::MNEMONIC;
    mnemonics["OUT"] = TokenType::MNEMONIC;
    mnemonics["INB"] = TokenType::MNEMONIC;
    mnemonics["OUTB"] = TokenType::MNEMONIC;
    mnemonics["INW"] = TokenType::MNEMONIC;
    mnemonics["OUTW"] = TokenType::MNEMONIC;
    mnemonics["INL"] = TokenType::MNEMONIC;
    mnemonics["OUTL"] = TokenType::MNEMONIC;
    mnemonics["INSTR"] = TokenType::MNEMONIC;
    mnemonics["OUTSTR"] = TokenType::MNEMONIC;
    mnemonics["DB"] = TokenType::MNEMONIC;
    mnemonics["HALT"] = TokenType::MNEMONIC;
    
    // Extended 64-bit operations
    mnemonics["ADD64"] = TokenType::MNEMONIC;
    mnemonics["SUB64"] = TokenType::MNEMONIC;
    mnemonics["MOV64"] = TokenType::MNEMONIC;
    mnemonics["LOAD_IMM64"] = TokenType::MNEMONIC;
    mnemonics["MUL64"] = TokenType::MNEMONIC;
    mnemonics["DIV64"] = TokenType::MNEMONIC;
    mnemonics["INC64"] = TokenType::MNEMONIC;
    mnemonics["DEC64"] = TokenType::MNEMONIC;
    
    // Extended register operations
    mnemonics["MOVEX"] = TokenType::MNEMONIC;
    mnemonics["ADDEX"] = TokenType::MNEMONIC;
    mnemonics["SUBEX"] = TokenType::MNEMONIC;
    mnemonics["MULEX"] = TokenType::MNEMONIC;
    mnemonics["DIVEX"] = TokenType::MNEMONIC;
    mnemonics["CMPEX"] = TokenType::MNEMONIC;
    mnemonics["LOADEX"] = TokenType::MNEMONIC;
    mnemonics["STOREX"] = TokenType::MNEMONIC;
    mnemonics["PUSHEX"] = TokenType::MNEMONIC;
    mnemonics["POPEX"] = TokenType::MNEMONIC;
    
    // Mode control
    mnemonics["MODE32"] = TokenType::MNEMONIC;
    mnemonics["MODE64"] = TokenType::MNEMONIC;
    mnemonics["MODECMP"] = TokenType::MNEMONIC;
    
    // Legacy 8-register names (R0-R7)
    for (int i = 0; i < 8; ++i) {
        registers["R" + std::to_string(i)] = TokenType::REGISTER;
    }
    
    // x64-style register names (based on the extended register system)
    registers["RAX"] = TokenType::REGISTER; // R0
    registers["RBX"] = TokenType::REGISTER; // R1  
    registers["RCX"] = TokenType::REGISTER; // R2
    registers["RDX"] = TokenType::REGISTER; // R3
    registers["RSI"] = TokenType::REGISTER; // R4
    registers["RDI"] = TokenType::REGISTER; // R5
    registers["RSP"] = TokenType::REGISTER; // Stack pointer
    registers["RBP"] = TokenType::REGISTER; // Base pointer
    
    // Extended registers R8-R15
    for (int i = 8; i < 16; ++i) {
        registers["R" + std::to_string(i)] = TokenType::REGISTER;
    }
    
    // Special registers
    registers["RIP"] = TokenType::REGISTER; // Instruction pointer (PC)
    registers["RFLAGS"] = TokenType::REGISTER; // Flags register
    
    tables_initialized = true;
}

Lexer::Lexer(const std::string& source) 
    : source(source), pos(0), line(1), column(1) {
    init_tables();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (pos < source.length()) {
        skip_whitespace();
        
        if (pos >= source.length()) break;
        
        char c = current_char();
        
        // Handle comments
        if (c == ';' || c == '#') {
            skip_comment();
            continue;
        }
        
        // Handle newlines
        if (c == '\n') {
            tokens.emplace_back(TokenType::NEWLINE, "\\n", line, column);
            advance();
            line++;
            column = 1;
            continue;
        }
        
        // Handle directives
        if (c == '.') {
            tokens.push_back(parse_directive());
            continue;
        }
        
        // Handle strings
        if (c == '"' || c == '\'') {
            tokens.push_back(parse_string());
            continue;
        }
        
        // Handle numbers
        if (is_digit(c) || (c == '0' && pos + 1 < source.length() && 
                           (peek_char() == 'x' || peek_char() == 'b'))) {
            tokens.push_back(parse_number());
            continue;
        }
        
        // Handle identifiers and mnemonics
        if (is_identifier_start(c)) {
            tokens.push_back(parse_identifier());
            continue;
        }
        
        // Handle single-character tokens
        switch (c) {
            case ',': tokens.emplace_back(TokenType::COMMA, ",", line, column); break;
            case ':': tokens.emplace_back(TokenType::COLON, ":", line, column); break;
            case '[': tokens.emplace_back(TokenType::LBRACKET, "[", line, column); break;
            case ']': tokens.emplace_back(TokenType::RBRACKET, "]", line, column); break;
            case '+': tokens.emplace_back(TokenType::PLUS, "+", line, column); break;
            case '-': tokens.emplace_back(TokenType::MINUS, "-", line, column); break;
            case '*': tokens.emplace_back(TokenType::ASTERISK, "*", line, column); break;
            default:
                add_error("Unexpected character: '" + std::string(1, c) + "'");
                tokens.emplace_back(TokenType::INVALID, std::string(1, c), line, column);
                break;
        }
        advance();
    }
    
    tokens.emplace_back(TokenType::END_OF_FILE, "", line, column);
    return tokens;
}

char Lexer::current_char() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::peek_char(size_t offset) const {
    size_t peek_pos = pos + offset;
    if (peek_pos >= source.length()) return '\0';
    return source[peek_pos];
}

void Lexer::advance() {
    if (pos < source.length()) {
        pos++;
        column++;
    }
}

void Lexer::skip_whitespace() {
    while (pos < source.length() && std::isspace(current_char()) && current_char() != '\n') {
        advance();
    }
}

void Lexer::skip_comment() {
    // Skip to end of line
    while (pos < source.length() && current_char() != '\n') {
        advance();
    }
}

Token Lexer::parse_identifier() {
    size_t start_line = line;
    size_t start_column = column;
    std::string text;
    
    while (pos < source.length() && is_identifier_part(current_char())) {
        text += current_char();
        advance();
    }
    
    // Convert to uppercase for case-insensitive matching
    std::string upper_text = text;
    std::transform(upper_text.begin(), upper_text.end(), upper_text.begin(), ::toupper);
    
    // Check if it's a mnemonic
    if (mnemonics.find(upper_text) != mnemonics.end()) {
        return Token(TokenType::MNEMONIC, upper_text, start_line, start_column);
    }
    
    // Check if it's a register
    if (registers.find(upper_text) != registers.end()) {
        return Token(TokenType::REGISTER, upper_text, start_line, start_column);
    }
    
    // Otherwise it's an identifier (label, symbol, etc.)
    return Token(TokenType::IDENTIFIER, text, start_line, start_column);
}

Token Lexer::parse_number() {
    size_t start_line = line;
    size_t start_column = column;
    std::string text;
    uint64_t value = 0;
    int base = 10;
    
    // Check for hex (0x) or binary (0b) prefix
    if (current_char() == '0' && pos + 1 < source.length()) {
        char next = peek_char();
        if (next == 'x' || next == 'X') {
            base = 16;
            text += current_char(); advance(); // '0'
            text += current_char(); advance(); // 'x'
        } else if (next == 'b' || next == 'B') {
            base = 2;
            text += current_char(); advance(); // '0'
            text += current_char(); advance(); // 'b'
        }
    }
    
    // Parse digits
    while (pos < source.length()) {
        char c = current_char();
        bool valid_digit = false;
        
        if (base == 16 && is_hex_digit(c)) {
            valid_digit = true;
        } else if (base == 2 && is_binary_digit(c)) {
            valid_digit = true;
        } else if (base == 10 && is_digit(c)) {
            valid_digit = true;
        }
        
        if (!valid_digit) break;
        
        text += c;
        
        // Calculate value
        if (base == 16) {
            value = value * 16 + (is_digit(c) ? (c - '0') : (std::toupper(c) - 'A' + 10));
        } else if (base == 2) {
            value = value * 2 + (c - '0');
        } else {
            value = value * 10 + (c - '0');
        }
        
        advance();
    }
    
    Token token(TokenType::NUMBER, text, start_line, start_column);
    token.value = value;
    return token;
}

Token Lexer::parse_string() {
    size_t start_line = line;
    size_t start_column = column;
    char quote_char = current_char();
    std::string text;
    std::string value;
    
    text += quote_char;
    advance(); // Skip opening quote
    
    while (pos < source.length() && current_char() != quote_char) {
        char c = current_char();
        
        if (c == '\\' && pos + 1 < source.length()) {
            // Handle escape sequences
            advance(); // Skip backslash
            char escaped = current_char();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                case '0': value += '\0'; break;
                default: 
                    value += escaped; 
                    break;
            }
            text += '\\';
            text += escaped;
        } else if (c == '\n') {
            add_error("Unterminated string literal");
            break;
        } else {
            value += c;
            text += c;
        }
        advance();
    }
    
    if (pos < source.length() && current_char() == quote_char) {
        text += quote_char;
        advance(); // Skip closing quote
    } else {
        add_error("Unterminated string literal");
    }
    
    Token token(TokenType::STRING, text, start_line, start_column);
    token.value = value;
    return token;
}

Token Lexer::parse_directive() {
    size_t start_line = line;
    size_t start_column = column;
    std::string text;
    
    text += current_char(); // '.'
    advance();
    
    while (pos < source.length() && is_identifier_part(current_char())) {
        text += current_char();
        advance();
    }
    
    std::string upper_text = text;
    std::transform(upper_text.begin(), upper_text.end(), upper_text.begin(), ::tolower);
    
    if (keywords.find(upper_text) != keywords.end()) {
        return Token(TokenType::DIRECTIVE, upper_text, start_line, start_column);
    }
    
    add_error("Unknown directive: " + text);
    return Token(TokenType::INVALID, text, start_line, start_column);
}

bool Lexer::is_identifier_start(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::is_identifier_part(char c) const {
    return std::isalnum(c) || c == '_';
}

bool Lexer::is_digit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::is_hex_digit(char c) const {
    return is_digit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

bool Lexer::is_binary_digit(char c) const {
    return c == '0' || c == '1';
}

void Lexer::add_error(const std::string& message) {
    errors.push_back("Line " + std::to_string(line) + ", Column " + std::to_string(column) + ": " + message);
}

} // namespace Assembler
