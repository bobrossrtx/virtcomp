#include "assembler.hpp"
#include "opcodes.hpp"

namespace Assembler {

AssemblerEngine::AssemblerEngine() : current_address(0) {
    init_opcode_table();
    init_register_table();
}

void AssemblerEngine::init_opcode_table() {
    // Map mnemonics to opcodes based on VirtComp CPU instruction set
    mnemonic_to_opcode["NOP"] = static_cast<uint8_t>(Opcode::NOP);
    mnemonic_to_opcode["LOAD_IMM"] = static_cast<uint8_t>(Opcode::LOAD_IMM);
    mnemonic_to_opcode["ADD"] = static_cast<uint8_t>(Opcode::ADD);
    mnemonic_to_opcode["SUB"] = static_cast<uint8_t>(Opcode::SUB);
    mnemonic_to_opcode["MOV"] = static_cast<uint8_t>(Opcode::MOV);
    mnemonic_to_opcode["JMP"] = static_cast<uint8_t>(Opcode::JMP);
    mnemonic_to_opcode["LOAD"] = static_cast<uint8_t>(Opcode::LOAD);
    mnemonic_to_opcode["STORE"] = static_cast<uint8_t>(Opcode::STORE);
    mnemonic_to_opcode["PUSH"] = static_cast<uint8_t>(Opcode::PUSH);
    mnemonic_to_opcode["POP"] = static_cast<uint8_t>(Opcode::POP);
    mnemonic_to_opcode["CMP"] = static_cast<uint8_t>(Opcode::CMP);
    mnemonic_to_opcode["JZ"] = static_cast<uint8_t>(Opcode::JZ);
    mnemonic_to_opcode["JNZ"] = static_cast<uint8_t>(Opcode::JNZ);
    mnemonic_to_opcode["JS"] = static_cast<uint8_t>(Opcode::JS);
    mnemonic_to_opcode["JNS"] = static_cast<uint8_t>(Opcode::JNS);
    mnemonic_to_opcode["JC"] = static_cast<uint8_t>(Opcode::JC);
    mnemonic_to_opcode["JNC"] = static_cast<uint8_t>(Opcode::JNC);
    mnemonic_to_opcode["JO"] = static_cast<uint8_t>(Opcode::JO);
    mnemonic_to_opcode["JNO"] = static_cast<uint8_t>(Opcode::JNO);
    mnemonic_to_opcode["JG"] = static_cast<uint8_t>(Opcode::JG);
    mnemonic_to_opcode["JL"] = static_cast<uint8_t>(Opcode::JL);
    mnemonic_to_opcode["JGE"] = static_cast<uint8_t>(Opcode::JGE);
    mnemonic_to_opcode["JLE"] = static_cast<uint8_t>(Opcode::JLE);
    mnemonic_to_opcode["MUL"] = static_cast<uint8_t>(Opcode::MUL);
    mnemonic_to_opcode["DIV"] = static_cast<uint8_t>(Opcode::DIV);
    mnemonic_to_opcode["INC"] = static_cast<uint8_t>(Opcode::INC);
    mnemonic_to_opcode["DEC"] = static_cast<uint8_t>(Opcode::DEC);
    mnemonic_to_opcode["AND"] = static_cast<uint8_t>(Opcode::AND);
    mnemonic_to_opcode["OR"] = static_cast<uint8_t>(Opcode::OR);
    mnemonic_to_opcode["XOR"] = static_cast<uint8_t>(Opcode::XOR);
    mnemonic_to_opcode["NOT"] = static_cast<uint8_t>(Opcode::NOT);
    mnemonic_to_opcode["SHL"] = static_cast<uint8_t>(Opcode::SHL);
    mnemonic_to_opcode["SHR"] = static_cast<uint8_t>(Opcode::SHR);
    mnemonic_to_opcode["CALL"] = static_cast<uint8_t>(Opcode::CALL);
    mnemonic_to_opcode["RET"] = static_cast<uint8_t>(Opcode::RET);
    mnemonic_to_opcode["PUSH_ARG"] = static_cast<uint8_t>(Opcode::PUSH_ARG);
    mnemonic_to_opcode["POP_ARG"] = static_cast<uint8_t>(Opcode::POP_ARG);
    mnemonic_to_opcode["PUSH_FLAG"] = static_cast<uint8_t>(Opcode::PUSH_FLAG);
    mnemonic_to_opcode["POP_FLAG"] = static_cast<uint8_t>(Opcode::POP_FLAG);
    mnemonic_to_opcode["LEA"] = static_cast<uint8_t>(Opcode::LEA);
    mnemonic_to_opcode["SWAP"] = static_cast<uint8_t>(Opcode::SWAP);
    mnemonic_to_opcode["IN"] = static_cast<uint8_t>(Opcode::IN);
    mnemonic_to_opcode["OUT"] = static_cast<uint8_t>(Opcode::OUT);
    mnemonic_to_opcode["INB"] = static_cast<uint8_t>(Opcode::INB);
    mnemonic_to_opcode["OUTB"] = static_cast<uint8_t>(Opcode::OUTB);
    mnemonic_to_opcode["INW"] = static_cast<uint8_t>(Opcode::INW);
    mnemonic_to_opcode["OUTW"] = static_cast<uint8_t>(Opcode::OUTW);
    mnemonic_to_opcode["INL"] = static_cast<uint8_t>(Opcode::INL);
    mnemonic_to_opcode["OUTL"] = static_cast<uint8_t>(Opcode::OUTL);
    mnemonic_to_opcode["INSTR"] = static_cast<uint8_t>(Opcode::INSTR);
    mnemonic_to_opcode["OUTSTR"] = static_cast<uint8_t>(Opcode::OUTSTR);
    mnemonic_to_opcode["DB"] = static_cast<uint8_t>(Opcode::DB);
    mnemonic_to_opcode["HALT"] = static_cast<uint8_t>(Opcode::HALT);

    // Extended operations
    mnemonic_to_opcode["ADD64"] = static_cast<uint8_t>(Opcode::ADD64);
    mnemonic_to_opcode["SUB64"] = static_cast<uint8_t>(Opcode::SUB64);
    mnemonic_to_opcode["MOV64"] = static_cast<uint8_t>(Opcode::MOV64);
    mnemonic_to_opcode["LOAD_IMM64"] = static_cast<uint8_t>(Opcode::LOAD_IMM64);
    mnemonic_to_opcode["MOVEX"] = static_cast<uint8_t>(Opcode::MOVEX);
    mnemonic_to_opcode["ADDEX"] = static_cast<uint8_t>(Opcode::ADDEX);
    mnemonic_to_opcode["MODE32"] = static_cast<uint8_t>(Opcode::MODE32);
    mnemonic_to_opcode["MODE64"] = static_cast<uint8_t>(Opcode::MODE64);
    mnemonic_to_opcode["MODECMP"] = static_cast<uint8_t>(Opcode::MODECMP);
}

void AssemblerEngine::init_register_table() {
    // Legacy register names (R0-R7)
    for (int i = 0; i < 8; ++i) {
        register_to_number["R" + std::to_string(i)] = i;
    }

    // x64-style register names
    register_to_number["RAX"] = 0;
    register_to_number["RBX"] = 1;
    register_to_number["RCX"] = 2;
    register_to_number["RDX"] = 3;
    register_to_number["RSI"] = 4;
    register_to_number["RDI"] = 5;
    register_to_number["RSP"] = 6; // Stack pointer
    register_to_number["RBP"] = 7; // Base pointer

    // Extended registers R8-R15
    for (int i = 8; i < 16; ++i) {
        register_to_number["R" + std::to_string(i)] = i;
    }

    // Special registers (mapped to extended register numbers)
    register_to_number["RIP"] = 16; // Instruction pointer
    register_to_number["RFLAGS"] = 17; // Flags register
}

std::vector<uint8_t> AssemblerEngine::assemble(const Program& program) {
    // Clear previous state
    errors.clear();
    symbol_table.clear();
    forward_refs.clear();
    bytecode.clear();
    current_address = 0;

    // Two-pass assembly
    first_pass(program);
    if (has_errors()) return {};

    second_pass(program);
    if (has_errors()) return {};

    resolve_forward_references();
    if (has_errors()) return {};

    return bytecode;
}

void AssemblerEngine::first_pass(const Program& program) {
    current_address = 0;

    for (const auto& stmt : program.statements) {
        switch (stmt->type) {
            case ASTNodeType::LABEL: {
                const auto& label = static_cast<const Label&>(*stmt);
                process_label(label);
                break;
            }

            case ASTNodeType::INSTRUCTION: {
                const auto& instruction = static_cast<const Instruction&>(*stmt);
                // Calculate instruction size and advance address
                size_t size = get_instruction_size(instruction.mnemonic, instruction.operands);
                current_address += size;
                break;
            }

            case ASTNodeType::DIRECTIVE: {
                const auto& directive = static_cast<const Directive&>(*stmt);
                // Handle directives that affect address calculation
                if (directive.name == ".org") {
                    handle_org_directive(directive.arguments);
                } else if (directive.name == ".db") {
                    current_address += directive.arguments.size();
                } else if (directive.name == ".dw") {
                    current_address += directive.arguments.size() * 2;
                } else if (directive.name == ".dd") {
                    current_address += directive.arguments.size() * 4;
                } else if (directive.name == ".string") {
                    // Calculate string length including null terminator
                    for (const auto& arg : directive.arguments) {
                        if (auto str_lit = dynamic_cast<const StringLiteralExpression*>(arg.get())) {
                            current_address += str_lit->value.length() + 1; // +1 for null terminator
                        }
                    }
                }
                break;
            }

            default:
                // Ignore other node types in first pass
                break;
        }
    }
}

void AssemblerEngine::second_pass(const Program& program) {
    current_address = 0;
    bytecode.clear();

    for (const auto& stmt : program.statements) {
        switch (stmt->type) {
            case ASTNodeType::LABEL:
                // Labels don't generate code
                break;

            case ASTNodeType::INSTRUCTION: {
                const auto& instruction = static_cast<const Instruction&>(*stmt);
                process_instruction(instruction);
                break;
            }

            case ASTNodeType::DIRECTIVE: {
                const auto& directive = static_cast<const Directive&>(*stmt);
                process_directive(directive);
                break;
            }

            default:
                // Ignore other node types in second pass
                break;
        }
    }
}

void AssemblerEngine::process_label(const Label& label) {
    if (symbol_table.find(label.name) != symbol_table.end()) {
        add_error("Label '" + label.name + "' already defined", label.line, label.column);
        return;
    }

    symbol_table[label.name] = Symbol(label.name, current_address, true);
}

void AssemblerEngine::process_directive(const Directive& directive) {
    if (directive.name == ".db") {
        handle_db_directive(directive.arguments);
    } else if (directive.name == ".dw") {
        handle_dw_directive(directive.arguments);
    } else if (directive.name == ".dd") {
        handle_dd_directive(directive.arguments);
    } else if (directive.name == ".string") {
        handle_string_directive(directive.arguments);
    } else if (directive.name == ".org") {
        handle_org_directive(directive.arguments);
    } else {
        add_error("Unknown directive: " + directive.name, directive.line, directive.column);
    }
}

void AssemblerEngine::process_instruction(const Instruction& instruction) {
    encode_instruction(instruction);
}

void AssemblerEngine::encode_instruction(const Instruction& instruction) {
    uint8_t opcode = get_opcode(instruction.mnemonic);
    if (opcode == 0xFF && instruction.mnemonic != "HALT") {
        add_error("Unknown instruction: " + instruction.mnemonic, instruction.line, instruction.column);
        return;
    }

    emit_byte(opcode);

    // Encode operands based on instruction type
    if (instruction.mnemonic == "NOP" || instruction.mnemonic == "HALT" ||
        instruction.mnemonic == "RET" || instruction.mnemonic == "PUSH_FLAG" ||
        instruction.mnemonic == "POP_FLAG") {
        // No operands
        return;
    }

    // Handle different instruction formats
    if (instruction.mnemonic == "LOAD_IMM") {
        // Format: LOAD_IMM reg, immediate
        if (instruction.operands.size() != 2) {
            add_error("LOAD_IMM requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Immediate operand
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 1); // VirtComp LOAD_IMM uses 1-byte immediate
        } else {
            emit_byte(static_cast<uint8_t>(value)); // 1-byte immediate, not 4-byte
        }
    } else if (instruction.mnemonic == "ADD" || instruction.mnemonic == "SUB" ||
               instruction.mnemonic == "MOV" || instruction.mnemonic == "CMP" ||
               instruction.mnemonic == "MUL" || instruction.mnemonic == "DIV" ||
               instruction.mnemonic == "AND" || instruction.mnemonic == "OR" ||
               instruction.mnemonic == "XOR") {
        // Format: INSTRUCTION reg1, reg2
        if (instruction.operands.size() != 2) {
            add_error(instruction.mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        for (const auto& operand : instruction.operands) {
            if (auto reg_expr = dynamic_cast<const RegisterExpression*>(operand.get())) {
                emit_byte(get_register_number(reg_expr->name));
            } else {
                add_error("Operand must be a register", instruction.line, instruction.column);
                return;
            }
        }
    } else if (instruction.mnemonic == "JMP" || instruction.mnemonic == "JZ" ||
               instruction.mnemonic == "JNZ" || instruction.mnemonic == "JS" ||
               instruction.mnemonic == "JNS" || instruction.mnemonic == "JC" ||
               instruction.mnemonic == "JNC" || instruction.mnemonic == "JO" ||
               instruction.mnemonic == "JNO" || instruction.mnemonic == "JG" ||
               instruction.mnemonic == "JL" || instruction.mnemonic == "JGE" ||
               instruction.mnemonic == "JLE" || instruction.mnemonic == "CALL") {
        // Format: JUMP address
        if (instruction.operands.size() != 1) {
            add_error(instruction.mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[0], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 1); // VirtComp uses 8-bit addresses for jumps
        } else {
            emit_byte(static_cast<uint8_t>(value));
        }
    } else if (instruction.mnemonic == "PUSH" || instruction.mnemonic == "POP" ||
               instruction.mnemonic == "INC" || instruction.mnemonic == "DEC" ||
               instruction.mnemonic == "NOT") {
        // Format: INSTRUCTION reg
        if (instruction.operands.size() != 1) {
            add_error(instruction.mnemonic + " requires 1 operand", instruction.line, instruction.column);
            return;
        }

        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("Operand must be a register", instruction.line, instruction.column);
            return;
        }
    } else if (instruction.mnemonic == "OUT" || instruction.mnemonic == "IN" ||
               instruction.mnemonic == "OUTB" || instruction.mnemonic == "INB" ||
               instruction.mnemonic == "OUTW" || instruction.mnemonic == "INW" ||
               instruction.mnemonic == "OUTL" || instruction.mnemonic == "INL" ||
               instruction.mnemonic == "OUTSTR" || instruction.mnemonic == "INSTR") {
        // Format: OUT reg, port  or  IN reg, port
        if (instruction.operands.size() != 2) {
            add_error(instruction.mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Port operand (immediate value)
        bool is_symbol;
        std::string symbol_name;
        int64_t port_value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            add_error("Port number must be an immediate value", instruction.line, instruction.column);
            return;
        } else {
            emit_byte(static_cast<uint8_t>(port_value));
        }
    } else if (instruction.mnemonic == "LOAD" || instruction.mnemonic == "STORE" ||
               instruction.mnemonic == "LEA" || instruction.mnemonic == "SWAP") {
        // Format: LOAD reg, addr  or  STORE reg, addr
        if (instruction.operands.size() != 2) {
            add_error(instruction.mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Address operand
        bool is_symbol;
        std::string symbol_name;
        int64_t addr_value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 1); // VirtComp uses 8-bit addresses
        } else {
            emit_byte(static_cast<uint8_t>(addr_value));
        }
    } else if (instruction.mnemonic == "SHL" || instruction.mnemonic == "SHR") {
        // Format: SHL reg, immediate
        if (instruction.operands.size() != 2) {
            add_error(instruction.mnemonic + " requires 2 operands", instruction.line, instruction.column);
            return;
        }

        // Register operand
        if (auto reg_expr = dynamic_cast<const RegisterExpression*>(instruction.operands[0].get())) {
            emit_byte(get_register_number(reg_expr->name));
        } else {
            add_error("First operand must be a register", instruction.line, instruction.column);
            return;
        }

        // Immediate operand
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*instruction.operands[1], is_symbol, symbol_name);

        if (is_symbol) {
            add_error("Shift amount must be an immediate value", instruction.line, instruction.column);
            return;
        } else {
            emit_byte(static_cast<uint8_t>(value));
        }
    } else {
        add_error("Instruction encoding not implemented: " + instruction.mnemonic, instruction.line, instruction.column);
    }
}

int64_t AssemblerEngine::evaluate_expression(const Expression& expr, bool& is_symbol_ref, std::string& symbol_name) {
    is_symbol_ref = false;
    symbol_name.clear();

    switch (expr.type) {
        case ASTNodeType::IMMEDIATE: {
            const auto& imm = static_cast<const ImmediateExpression&>(expr);
            return imm.value;
        }

        case ASTNodeType::IDENTIFIER: {
            const auto& id = static_cast<const IdentifierExpression&>(expr);
            auto it = symbol_table.find(id.name);
            if (it != symbol_table.end() && it->second.defined) {
                return it->second.address;
            } else {
                is_symbol_ref = true;
                symbol_name = id.name;
                return 0; // Will be resolved later
            }
        }

        case ASTNodeType::REGISTER: {
            const auto& reg = static_cast<const RegisterExpression&>(expr);
            return get_register_number(reg.name);
        }

        default:
            add_error("Cannot evaluate expression");
            return 0;
    }
}

uint8_t AssemblerEngine::get_register_number(const std::string& reg_name) {
    auto it = register_to_number.find(reg_name);
    if (it != register_to_number.end()) {
        return it->second;
    }
    add_error("Unknown register: " + reg_name);
    return 0;
}

uint8_t AssemblerEngine::get_opcode(const std::string& mnemonic) {
    auto it = mnemonic_to_opcode.find(mnemonic);
    if (it != mnemonic_to_opcode.end()) {
        return it->second;
    }
    return 0xFF; // Invalid opcode
}

void AssemblerEngine::emit_byte(uint8_t byte) {
    bytecode.push_back(byte);
    current_address++;
}

void AssemblerEngine::emit_word(uint16_t word) {
    emit_byte(word & 0xFF);
    emit_byte((word >> 8) & 0xFF);
}

void AssemblerEngine::emit_dword(uint32_t dword) {
    emit_byte(dword & 0xFF);
    emit_byte((dword >> 8) & 0xFF);
    emit_byte((dword >> 16) & 0xFF);
    emit_byte((dword >> 24) & 0xFF);
}

void AssemblerEngine::emit_forward_ref(const std::string& symbol, size_t size, bool relative) {
    forward_refs.push_back({current_address, symbol, size, relative});

    // Emit placeholder bytes
    for (size_t i = 0; i < size; ++i) {
        emit_byte(0);
    }
}

size_t AssemblerEngine::get_instruction_size(const std::string& mnemonic, const std::vector<std::unique_ptr<Expression>>& /* operands */) {
    // Basic instruction size calculations for VirtComp
    if (mnemonic == "NOP" || mnemonic == "HALT" || mnemonic == "RET" ||
        mnemonic == "PUSH_FLAG" || mnemonic == "POP_FLAG") {
        return 1;
    } else if (mnemonic == "LOAD_IMM") {
        return 3; // opcode + register + 1-byte immediate
    } else if (mnemonic == "ADD" || mnemonic == "SUB" || mnemonic == "MOV" ||
               mnemonic == "CMP" || mnemonic == "MUL" || mnemonic == "DIV" ||
               mnemonic == "AND" || mnemonic == "OR" || mnemonic == "XOR") {
        return 3; // opcode + reg1 + reg2
    } else if (mnemonic == "JMP" || mnemonic == "JZ" || mnemonic == "JNZ" ||
               mnemonic == "JS" || mnemonic == "JNS" || mnemonic == "JC" ||
               mnemonic == "JNC" || mnemonic == "JO" || mnemonic == "JNO" ||
               mnemonic == "JG" || mnemonic == "JL" || mnemonic == "JGE" ||
               mnemonic == "JLE" || mnemonic == "CALL") {
        return 2; // opcode + address
    } else if (mnemonic == "PUSH" || mnemonic == "POP" || mnemonic == "INC" ||
               mnemonic == "DEC" || mnemonic == "NOT") {
        return 2; // opcode + register
    } else if (mnemonic == "OUT" || mnemonic == "IN" || mnemonic == "OUTB" ||
               mnemonic == "INB" || mnemonic == "OUTW" || mnemonic == "INW" ||
               mnemonic == "OUTL" || mnemonic == "INL" || mnemonic == "OUTSTR" ||
               mnemonic == "INSTR" || mnemonic == "LOAD" || mnemonic == "STORE" ||
               mnemonic == "LEA" || mnemonic == "SWAP" || mnemonic == "SHL" ||
               mnemonic == "SHR") {
        return 3; // opcode + register + address/port/immediate
    }

    return 1; // Default size
}

void AssemblerEngine::handle_db_directive(const std::vector<std::unique_ptr<Expression>>& args) {
    for (const auto& arg : args) {
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 1);
        } else {
            emit_byte(static_cast<uint8_t>(value));
        }
    }
}

void AssemblerEngine::handle_dw_directive(const std::vector<std::unique_ptr<Expression>>& args) {
    for (const auto& arg : args) {
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 2);
        } else {
            emit_word(static_cast<uint16_t>(value));
        }
    }
}

void AssemblerEngine::handle_dd_directive(const std::vector<std::unique_ptr<Expression>>& args) {
    for (const auto& arg : args) {
        bool is_symbol;
        std::string symbol_name;
        int64_t value = evaluate_expression(*arg, is_symbol, symbol_name);

        if (is_symbol) {
            emit_forward_ref(symbol_name, 4);
        } else {
            emit_dword(static_cast<uint32_t>(value));
        }
    }
}

void AssemblerEngine::handle_string_directive(const std::vector<std::unique_ptr<Expression>>& args) {
    for (const auto& arg : args) {
        if (auto str_lit = dynamic_cast<const StringLiteralExpression*>(arg.get())) {
            for (char c : str_lit->value) {
                emit_byte(static_cast<uint8_t>(c));
            }
            emit_byte(0); // Null terminator
        } else {
            add_error("String directive requires string literal");
        }
    }
}

void AssemblerEngine::handle_org_directive(const std::vector<std::unique_ptr<Expression>>& args) {
    if (args.size() != 1) {
        add_error(".org directive requires exactly one argument");
        return;
    }

    bool is_symbol;
    std::string symbol_name;
    int64_t value = evaluate_expression(*args[0], is_symbol, symbol_name);

    if (is_symbol) {
        add_error(".org directive cannot use forward references");
        return;
    }

    current_address = static_cast<uint32_t>(value);

    // Pad bytecode to the new address
    while (bytecode.size() < current_address) {
        bytecode.push_back(0);
    }
}

void AssemblerEngine::resolve_forward_references() {
    for (const auto& ref : forward_refs) {
        auto it = symbol_table.find(ref.symbol);
        if (it == symbol_table.end() || !it->second.defined) {
            add_error("Undefined symbol: " + ref.symbol);
            continue;
        }

        uint32_t address = it->second.address;
        if (ref.relative) {
            address = address - (ref.address + ref.size);
        }

        // Patch the bytecode
        if (ref.address + ref.size > bytecode.size()) {
            add_error("Forward reference out of bounds");
            continue;
        }

        for (size_t i = 0; i < ref.size; ++i) {
            bytecode[ref.address + i] = (address >> (8 * i)) & 0xFF;
        }
    }
}

void AssemblerEngine::add_error(const std::string& message) {
    errors.push_back("Assembly error: " + message);
}

void AssemblerEngine::add_error(const std::string& message, size_t line, size_t column) {
    errors.push_back("Line " + std::to_string(line) + ", Column " + std::to_string(column) +
                    ": " + message);
}

} // namespace Assembler
