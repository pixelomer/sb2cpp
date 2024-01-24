#include "parse.hpp"
#include "util.hpp"
#include <string>

#define EOF_TOKEN ""

namespace sb2cpp {

std::string Parser::token_get(int idx) {
    if (idx < this->tokens.size()) {
        std::string token = this->tokens[idx];
        if (token[0] == '\n') {
            this->line += token.length();
            token = "\n";
        }
        return token;
    }
    return EOF_TOKEN;
}

std::string Parser::token_next() {
    if (this->idx >= this->tokens.size()) return EOF_TOKEN;
    return token_get(this->idx++);
}

std::string Parser::try_token_next(std::string expected) {
    auto token = this->token_next();
    if (token == EOF_TOKEN) {
        throw SyntaxError(this->line, expected, EOF_TOKEN);
    }
    else if (expected.size() > 0 && expected[0] != '<' && expected != token) {
        throw SyntaxError(this->line, expected, token);
    }
    return token;
}

AST::Assign *Parser::parse_assign() {
    auto variable = token_next();
    auto assignment_op = token_next();
    if (assignment_op != "=") {
        throw SyntaxError(this->line, "=", assignment_op);
    }
    auto value = parse_value();
    return new AST::Assign(variable, value);
}

AST::IfStatement *Parser::parse_if_statement() {
    return nullptr;
}

AST::Subroutine *Parser::parse_subroutine() {
    return nullptr;
}

AST::SubroutineCall *Parser::parse_subroutine_call() {
    return nullptr;
}

AST::StdlibCall *Parser::parse_stdlib_call() {
    auto class_name = this->try_token_next("<class>");
    this->try_token_next(".");
    auto method_name = this->try_token_next("<method>");
    this->try_token_next("(");
    std::vector<AST::Value *> arguments;
    while (this->token_get(this->idx) != ")") {
        arguments.push_back(this->parse_value());
        if (this->token_get(this->idx) == ",") {
            if (this->token_get(this->idx+1) == ")") {
                throw SyntaxError(this->line, ")", ",");
            }
            this->idx++;
        }
    }
    this->idx++;
    return new AST::StdlibCall(class_name, method_name, arguments);
}

AST::Value *Parser::parse_value() {
    std::vector<AST::ValueGroup::ValueGroupElement> elements;
    bool allow_add_sub = true;
    bool allow_mult_div = false;
    bool expect_value = true;
    while (true) {
        AST::ValueGroup::ValueGroupElement elem;
        auto token = this->token_get(this->idx);
        if (token == EOF_TOKEN || token == ")") {
            if (expect_value) {
                throw SyntaxError(this->line, "Expected value");
            }
            break;
        }
        else if (expect_value && token == "(") {
            this->idx++;
            auto subvalue = this->parse_value();
            token = this->token_get(this->idx);
            if (token != ")") {
                throw SyntaxError(this->line, ")", token);
            }
            elem = { AST::NoValueOp, subvalue };
        }
        else if (expect_value && std::isdigit(token[0])) {
            elem = { AST::NoValueOp, new AST::NumberValue(std::stod(token)) };
        }
        else if (expect_value && token[0] == '"') {
            if (token[token.length()-1] != '"' || token.length() == 1) {
                throw SyntaxError(this->line, "\"", EOF_TOKEN);
            }
            auto str = token.substr(1, token.length()-2);
            elem = { AST::NoValueOp, new AST::StringValue(str) };
        }
        else if (allow_add_sub && (token == "+" || token == "-")) {
            if (token == "+") elem = { AST::Add, nullptr };
            else elem = { AST::Subtract, nullptr };
            expect_value = true;
            allow_mult_div = false;
        }
        else if (allow_mult_div && (token == "*" || token == "/")) {
            if (token == "*") elem = { AST::Multiply, nullptr };
            else elem = { AST::Divide, nullptr };
            expect_value = true;
            allow_add_sub = true;
            allow_mult_div = false;
        }
        else if (expect_value) {
            if (this->token_get(this->idx + 1) == ".") {
                if (this->token_get(this->idx + 3) == "(") {
                    elem = { AST::NoValueOp, this->parse_stdlib_call() };
                    this->idx--;
                }
                else {
                    auto value = new AST::StdlibValue(this->token_get(this->idx),
                        this->token_get(this->idx+2));
                    elem = { AST::NoValueOp, value };
                    this->idx += 2;
                }
            }
            else {
                elem = { AST::NoValueOp, new AST::VariableValue(token) };
            }
        }
        else {
            break;
        }
        this->idx++;
        elements.push_back(elem);
        if (elem.op == AST::NoValueOp) {
            expect_value = false;
            allow_add_sub = true;
            allow_mult_div = true;
        }

    }
    AST::ValueGroup value_group(elements);
    AST::Value *value = value_group.simplify();
    return value;
}

AST::Condition *Parser::parse_condition() {
    return nullptr;
}

AST::ConditionGroup *Parser::parse_condition_group() {
    return nullptr;
}

AST::StdlibAssign *Parser::parse_stdlib_assign() {
    auto class_name = this->try_token_next("<class>");
    this->try_token_next(".");
    auto property = this->try_token_next("<property>");
    this->try_token_next("=");
    auto value = this->parse_value();
    return new AST::StdlibAssign(class_name, property, value);
}

AST::Statement *Parser::parse_statement() {
    AST::Statement *node = nullptr;
    
    auto first_token = this->token_get(this->idx);
    auto first_keyword = strtolower(first_token);

    if (first_keyword == "if") {
        node = parse_if_statement();
    }
    else {
        auto second_token = this->token_get(this->idx+1);
        if (second_token == "=") {
            node = parse_assign();
        }
        else if (second_token == "(") {
            node = parse_subroutine_call();
        }
        else if (second_token == ".") {
            if (this->token_get(this->idx+3) == "=") {
                node = parse_stdlib_assign();
            }
            else {
                node = parse_stdlib_call();
            }
        }
        else {
            throw SyntaxError(this->line, "Expected statement");
        }
    }

    auto token = token_next();
    if (token != "\n" && token != EOF_TOKEN) {
        throw SyntaxError(this->line, "\n", token);
    }

    return node;
}

AST::Node *Parser::parse_next() {
    AST::Node *node = nullptr;
    
    auto first_token = this->token_get(this->idx);
    auto first_keyword = strtolower(first_token);

    if (first_token == EOF_TOKEN) return nullptr;

    if (first_keyword == "sub") {
        node = parse_subroutine();
    }
    else {
        node = parse_statement();
    }

    return node;
}

}