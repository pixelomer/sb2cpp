#include "parse.hpp"
#include "util.hpp"
#include <string>
#include <map>

#define EOF_TOKEN ""

namespace sb2cpp {

const std::map<std::string, AST::ComparisonOp> Parser::comparators = {
    { "<>", AST::NotEqual },
    { "<=", AST::LessThanOrEqual },
    { ">=", AST::GreaterThanOrEqual },
    { "=", AST::Equal },
    { ">", AST::GreaterThan },
    { "<", AST::LessThan }
};

const std::map<std::string, AST::LogicOp> Parser::logic_ops = {
    { "and", AST::And },
    { "or", AST::Or }
};

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

std::string Parser::try_token_next(std::string expected_keyword) {
    auto token = this->token_next();
    if (token == EOF_TOKEN) {
        throw SyntaxError(this->line, expected_keyword, EOF_TOKEN);
    }
    if (expected_keyword.size() > 0 && expected_keyword[0] != '<') {
        auto token_lower = strtolower(token);
        auto expected_lower = strtolower(expected_keyword);
        if (expected_lower != token_lower) {
            throw SyntaxError(this->line, expected_keyword, token);
        }
    }
    return token;
}

std::string Parser::parse_id(std::string token) {
    const std::vector<std::string> wordlist = { "for", "endfor", "to",
        "step", "if", "then", "else", "elseif", "endif", "goto", "sub",
        "endsub", "while", "endwhile", "and", "or" };
    const std::vector<char> charlist = { '=', '>', '<', '-', '+',
        '/', '*', ',', '\'' };
    auto token_lower = strtolower(token);
    auto word_match = std::find(wordlist.begin(), wordlist.end(), token_lower)
        != wordlist.end();
    auto char_match = token == "" || std::find(charlist.begin(), charlist.end(),
        token_lower[0]) != charlist.end();
    if (word_match || char_match) {
        throw SyntaxError(this->line, "<identifier>", token);
    }
    return token;
}

AST::Assign *Parser::parse_assign() {
    auto variable = this->parse_id(this->token_next());
    this->try_token_next("=");
    auto value = this->parse_value();
    return new AST::Assign(variable, value);
}

AST::ArrayAssign *Parser::parse_array_assign() {
    auto variable = this->parse_id(this->token_next());
    this->try_token_next("[");
    auto key = this->parse_value();
    this->try_token_next("]");
    this->try_token_next("=");
    auto value = this->parse_value();
    return new AST::ArrayAssign(variable, key, value);
}

AST::ArrayValue *Parser::parse_array_value() {
    auto variable = this->parse_id(this->token_next());
    this->try_token_next("[");
    auto key = this->parse_value();
    this->try_token_next("]");
    return new AST::ArrayValue(variable, key);
}

AST::IfStatement *Parser::parse_if_statement() {
    return nullptr;
}

AST::Statement *Parser::parse_statement_group(std::string end_token) {
    auto end_lower = strtolower(end_token);
    std::vector<AST::Statement *> statements;
    while (strtolower(this->token_get(this->idx)) != end_lower) {
        statements.push_back(this->parse_statement());
    }
    this->idx++;
    if (statements.size() == 1) {
        return statements[0];
    }
    return new AST::StatementGroup(statements);
}

AST::Subroutine *Parser::parse_subroutine() {
    this->try_token_next("Sub");
    auto name = this->parse_id(this->token_next());
    this->try_token_next("\n");
    auto statement_group = this->parse_statement_group("EndSub");

    auto token = this->token_next();
    if (token != "\n" && token != EOF_TOKEN) {
        throw SyntaxError(this->line, "\n", token);
    }

    return new AST::Subroutine(name, statement_group);
}

AST::SubroutineCall *Parser::parse_subroutine_call() {
    auto name = this->parse_id(this->token_next());
    this->try_token_next("(");
    this->try_token_next(")");
    return new AST::SubroutineCall(name);
}

AST::StdlibCall *Parser::parse_stdlib_call() {
    auto class_name = this->parse_id(this->try_token_next("<class>"));
    this->try_token_next(".");
    auto method_name = this->parse_id(this->try_token_next("<method>"));
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

AST::Value *Parser::parse_value(bool throw_on_comparator) {
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
        else if (expect_value && comparators.count(token) == 0) {
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
            else if (this->token_get(this->idx + 1) == "[") {
                elem = { AST::NoValueOp, this->parse_array_value() };
                this->idx--;
            }
            else {
                elem = { AST::NoValueOp,
                    new AST::VariableValue(this->parse_id(token)) };
            }
        }
        else if (throw_on_comparator && comparators.count(token) != 0) {
            throw SyntaxError(this->line, "Expected arithmetic operator");
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

std::tuple<int, int> Parser::save_state() {
    return { this->idx, this->line };
}

void Parser::restore_state(std::tuple<int, int> state) {
    std::tuple<int&, int&>(this->idx, this->line) = state;
}

void Parser::parse_value_or_condition(AST::Value **value,
    AST::Condition **condition)
{
    auto state = this->save_state();
    try {
        *value = this->parse_value();
    }
    catch (SyntaxError err) {
        this->restore_state(state);
        *condition = this->parse_condition();
    }
}

AST::Condition *Parser::parse_condition() {
    std::vector<AST::ConditionGroup::ConditionGroupElement> elems;
    AST::LogicOp next_logic = AST::NoLogicOp;
    AST::ComparisonOp next_comp = AST::NoComparisonOp;
    AST::Value *lvalue = nullptr;
    while (true) {
        auto token = this->token_get(this->idx);
        AST::Value *value = nullptr;
        AST::Condition *condition = nullptr;
        if (token == "(") {
            this->idx++;
            this->parse_value_or_condition(&value, &condition);
            this->try_token_next(")");
        }
        else {
            value = this->parse_value(false);
        }
        if (lvalue != nullptr) {
            if (condition != nullptr) {
                throw SyntaxError(this->line, "Expected value");
            }
            elems.push_back({ next_logic, new AST::SingleCondition(lvalue,
                value, next_comp) });
            lvalue = nullptr;
        }
        else if (condition != nullptr) {
            elems.push_back({ next_logic, condition });
        }
        else {
            lvalue = value;
            token = this->token_next();
            if (comparators.count(token) == 0) {
                throw SyntaxError(this->line, "<comparator>", token);
            }
            next_comp = comparators.at(token);
        }
        if (lvalue == nullptr) {
            token = strtolower(this->token_get(this->idx));
            if (logic_ops.count(token) == 0) {
                break;
            }
            else {
                this->idx++;
                next_logic = logic_ops.at(token);
                lvalue = nullptr;
            }
        }
    }
    if (elems.size() == 0) {
        throw SyntaxError(this->line, "Expected condition");
    }
    AST::ConditionGroup condition_group(elems);
    return condition_group.simplify();
}

AST::WhileLoop *Parser::parse_while_loop() {
    this->try_token_next("While");
    AST::Condition *condition = this->parse_condition();
    this->try_token_next("\n");
    auto statement_group = this->parse_statement_group("EndWhile");
    return new AST::WhileLoop(condition, statement_group);
}

AST::StdlibAssign *Parser::parse_stdlib_assign() {
    auto class_name = this->parse_id(this->try_token_next("<class>"));
    this->try_token_next(".");
    auto property = this->parse_id(this->try_token_next("<property>"));
    this->try_token_next("=");
    auto value = this->parse_value();
    return new AST::StdlibAssign(class_name, property, value);
}

AST::ForLoop *Parser::parse_for_loop() {
    this->try_token_next("For");
    auto init = this->parse_assign();
    this->try_token_next("To");
    auto end = this->parse_value();
    AST::Value *step = nullptr;
    if (strtolower(this->token_get(this->idx)) == "step") {
        this->idx++;
        step = this->parse_value();
    }
    this->try_token_next("\n");
    auto statements = this->parse_statement_group("EndFor");
    return new AST::ForLoop(init, end, step, statements);
}

AST::Statement *Parser::parse_statement() {
    AST::Statement *node = nullptr;
    
    auto first_token = this->token_get(this->idx);
    auto first_keyword = strtolower(first_token);

    if (first_keyword == "if") {
        node = parse_if_statement();
    }
    else if (first_keyword == "while") {
        node = parse_while_loop();
    }
    else if (first_keyword == "for") {
        node = parse_for_loop();
    }
    else {
        auto second_token = this->token_get(this->idx+1);
        if (second_token == "=") {
            node = parse_assign();
        }
        else if (second_token == "(") {
            node = parse_subroutine_call();
        }
        else if (second_token == "[") {
            node = parse_array_assign();
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
            throw SyntaxError(this->line, "<statement>", first_token);
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