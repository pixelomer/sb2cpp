#include "parser.hpp"
#include "util.hpp"
#include <string>
#include <map>
#include <algorithm>

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

std::string Parser::token_get(int idx, bool commit_nl) {
    if (idx < (long)this->tokens.size()) {
        std::string token = this->tokens[idx];
        if (token[0] == '\n') {
            if (commit_nl) {
                this->line += token.length();
            }
            token = "\n";
        }
        return token;
    }
    return EOF_TOKEN;
}

std::string Parser::token_next() {
    if (this->idx >= (long)this->tokens.size()) return EOF_TOKEN;
    return token_get(this->idx++, true);
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

std::unique_ptr<AST::Assign> Parser::parse_assign() {
    auto variable = this->parse_id(this->token_next());
    this->try_token_next("=");
    auto value = this->parse_value();
    return std::make_unique<AST::Assign>(variable,
        std::move(value));
}

std::unique_ptr<AST::ArrayAssign> Parser::parse_array_assign() {
    auto variable = this->parse_id(this->token_next());
    std::vector<std::unique_ptr<AST::Value>> keys;
    while (this->token_get(this->idx) == "[") {
        this->try_token_next("[");
        keys.push_back(this->parse_value());
        this->try_token_next("]");
    }
    this->try_token_next("=");
    auto value = this->parse_value();
    return std::make_unique<AST::ArrayAssign>(variable, std::move(keys),
        std::move(value));
}

std::unique_ptr<AST::ArrayValue> Parser::parse_array_value() {
    auto variable = this->parse_id(this->token_next());
    std::vector<std::unique_ptr<AST::Value>> keys;
    while (this->token_get(this->idx) == "[") {
        this->try_token_next("[");
        keys.push_back(this->parse_value());
        this->try_token_next("]");
    }
    return std::make_unique<AST::ArrayValue>(variable, std::move(keys));
}

std::unique_ptr<AST::IfStatement> Parser::parse_if_statement() {
    std::vector<std::unique_ptr<AST::IfStatement::IfStatementPart>> parts;
    
    while (true) {
        auto token = strtolower(this->token_next());
        if (parts.size() == 0 && token != "if") {
            throw SyntaxError(this->line, "If", token);
        }
        if (token == "elseif" || token == "if") {
            if (parts.size() > 0 && token != "elseif") {
                throw SyntaxError(this->line, "ElseIf", token);
            }
            auto condition = this->parse_condition();
            this->try_token_next("Then");
            this->try_token_next("\n");
            auto statement = this->parse_statement_group({ "else",
                "elseif", "endif" });
            this->idx--;
            parts.push_back(std::make_unique<AST::IfStatement::IfStatementPart>(
                std::move(condition), std::move(statement)));
        }
        else if (token == "else") {
            this->try_token_next("\n");
            auto statement = this->parse_statement_group("endif");
            parts.push_back(std::make_unique<AST::IfStatement::IfStatementPart>(
                nullptr, std::move(statement)));
            break;
        }
        else if (token == "endif") {
            break;
        }
        else {
            throw SyntaxError(this->line, "EndIf", token);
        }
    }

    return std::make_unique<AST::IfStatement>(std::move(parts));
}

std::unique_ptr<AST::StatementGroup> Parser::parse_statement_group(
    std::vector<std::string> end_tokens)
{
    std::vector<std::shared_ptr<AST::Statement>> statements;
    while (std::find(end_tokens.begin(), end_tokens.end(),
        strtolower(this->token_get(this->idx))) == end_tokens.end())
    {
        statements.push_back(this->parse_statement());
    }
    this->idx++;
    return std::make_unique<AST::StatementGroup>(statements);
}

std::unique_ptr<AST::StatementGroup> Parser::parse_statement_group(
    std::string end_token)
{
    std::vector<std::string> tokens = { end_token };
    return this->parse_statement_group(tokens);
}

std::unique_ptr<AST::Subroutine> Parser::parse_subroutine() {
    this->try_token_next("Sub");
    auto name = this->parse_id(this->token_next());
    this->try_token_next("\n");
    auto statement_group = this->parse_statement_group("endsub");

    auto token = this->token_next();
    if (token != "\n" && token != EOF_TOKEN) {
        throw SyntaxError(this->line, "\n", token);
    }

    return std::make_unique<AST::Subroutine>(name,
        std::move(statement_group));
}

std::unique_ptr<AST::SubroutineCall> Parser::parse_subroutine_call() {
    auto name = this->parse_id(this->token_next());
    this->try_token_next("(");
    this->try_token_next(")");
    return std::make_unique<AST::SubroutineCall>(name);
}

std::unique_ptr<AST::StdlibCall> Parser::parse_stdlib_call(bool returns_value) {
    auto class_name = this->parse_id(this->try_token_next("<class>"));
    this->try_token_next(".");
    auto method_name = this->parse_id(this->try_token_next("<method>"));
    this->try_token_next("(");
    std::vector<std::unique_ptr<AST::Value>> arguments;
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
    return std::make_unique<AST::StdlibCall>(class_name,
        method_name, std::move(arguments), returns_value);
}

std::unique_ptr<AST::Value> Parser::parse_value(bool throw_on_comparator) {
    std::vector<std::unique_ptr<AST::ValueGroup::ValueGroupElement>> elements;
    bool allow_add_sub = true;
    bool allow_mult_div = false;
    bool expect_value = true;
    while (true) {
        auto elem = std::make_unique<AST::ValueGroup::ValueGroupElement>();
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
            elem->op = AST::NoValueOp;
            elem->value = std::move(subvalue);
        }
        else if (expect_value && std::isdigit(token[0])) {
            elem->op = AST::NoValueOp;
            elem->value = std::make_unique<AST::NumberValue>(std::stod(token));
        }
        else if (expect_value && token[0] == '"') {
            int str_size = token.length()-2;
            if (token.length() > 1 && token[token.length()-1] != '"') {
                str_size += 1;
            }
            auto str = token.substr(1, str_size);
            elem->op = AST::NoValueOp;
            elem->value = std::make_unique<AST::StringValue>(str);
        }
        else if (allow_add_sub && (token == "+" || token == "-")) {
            if (token == "+") *elem = { AST::Add, nullptr };
            else *elem = { AST::Subtract, nullptr };
            expect_value = true;
            allow_mult_div = false;
        }
        else if (allow_mult_div && (token == "*" || token == "/")) {
            if (token == "*") *elem = { AST::Multiply, nullptr };
            else *elem = { AST::Divide, nullptr };
            expect_value = true;
            allow_add_sub = true;
            allow_mult_div = false;
        }
        else if (expect_value && comparators.count(token) == 0) {
            if (this->token_get(this->idx + 1) == ".") {
                if (this->token_get(this->idx + 3) == "(") {
                    elem->op = AST::NoValueOp; 
                    elem->value = this->parse_stdlib_call(true);
                    this->idx--;
                }
                else {
                    elem->op = AST::NoValueOp;
                    elem->value = std::make_unique<AST::StdlibValue>(
                        this->token_get(this->idx), this->token_get(this->idx+2));
                    this->idx += 2;
                }
            }
            else if (this->token_get(this->idx + 1) == "[") {
                elem->op = AST::NoValueOp;
                elem->value = this->parse_array_value();
                this->idx--;
            }
            else {
                elem->op = AST::NoValueOp;
                elem->value = std::make_unique<AST::VariableValue>(
                    this->parse_id(token));
            }
        }
        else if (throw_on_comparator && comparators.count(token) != 0) {
            throw SyntaxError(this->line, "Expected arithmetic operator");
        }
        else {
            break;
        }
        this->idx++;
        if (elem->op == AST::NoValueOp) {
            expect_value = false;
            allow_add_sub = true;
            allow_mult_div = true;
        }
        elements.push_back(std::move(elem));

    }
    AST::ValueGroup value_group(std::move(elements));
    return value_group.simplify();
}

std::tuple<long, long> Parser::save_state() {
    return { this->idx, this->line };
}

void Parser::restore_state(std::tuple<long, long> state) {
    std::tuple<long&, long&>(this->idx, this->line) = state;
}

void Parser::parse_value_or_condition(std::unique_ptr<AST::Value> *value,
    std::unique_ptr<AST::Condition> *condition)
{
    this->try_token_next("(");
    auto state = this->save_state();
    try {
        *value = this->parse_value();
        this->try_token_next(")");
    }
    catch (SyntaxError &err) {
        this->restore_state(state);
        *condition = this->parse_condition();
        this->try_token_next(")");
    }
}

std::unique_ptr<AST::Condition> Parser::parse_condition() {
    std::vector<std::unique_ptr<AST::ConditionGroup::ConditionGroupElement>> elems;
    AST::LogicOp next_logic = AST::NoLogicOp;
    AST::ComparisonOp next_comp = AST::NoComparisonOp;
    std::unique_ptr<AST::Value> lvalue = nullptr;
    while (true) {
        auto token = this->token_get(this->idx);
        std::unique_ptr<AST::Value> value = nullptr;
        std::unique_ptr<AST::Condition> condition = nullptr;
        if (token == "(") {
            this->parse_value_or_condition(&value, &condition);
        }
        else {
            value = this->parse_value(false);
        }
        if (lvalue != nullptr) {
            if (condition != nullptr) {
                throw SyntaxError(this->line, "Expected value");
            }
            elems.push_back(std::make_unique<AST::ConditionGroup::ConditionGroupElement>(
                next_logic, std::make_unique<AST::BinaryCompareOp>(std::move(lvalue),
                std::move(value), next_comp)));
            lvalue = nullptr;
        }
        else if (condition == nullptr) {
            token = this->token_get(this->idx);
            if (comparators.count(token) == 0) {
                condition = std::make_unique<AST::TruthyOp>(std::move(value));
            }
            else {
                this->idx++;
                lvalue = std::move(value);
                next_comp = comparators.at(token);
            }
        }
        if (condition != nullptr) {
            elems.push_back(std::make_unique<AST::ConditionGroup::ConditionGroupElement>(
                next_logic, std::move(condition)));
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
    AST::ConditionGroup condition_group(std::move(elems));
    return condition_group.simplify();
}

std::unique_ptr<AST::WhileLoop> Parser::parse_while_loop() {
    this->try_token_next("While");
    std::unique_ptr<AST::Condition> condition = this->parse_condition();
    this->try_token_next("\n");
    auto statement_group = this->parse_statement_group("endwhile");
    return std::make_unique<AST::WhileLoop>(std::move(condition),
        std::move(statement_group));
}

std::unique_ptr<AST::StdlibAssign> Parser::parse_stdlib_assign() {
    auto class_name = this->parse_id(this->try_token_next("<class>"));
    this->try_token_next(".");
    auto property = this->parse_id(this->try_token_next("<property>"));
    this->try_token_next("=");
    auto value = this->parse_value();
    return std::make_unique<AST::StdlibAssign>(class_name, property,
        std::move(value));
}

std::unique_ptr<AST::ForLoop> Parser::parse_for_loop() {
    this->try_token_next("For");
    auto init = this->parse_assign();
    this->try_token_next("To");
    auto end = this->parse_value();
    std::unique_ptr<AST::Value> step = nullptr;
    if (strtolower(this->token_get(this->idx)) == "step") {
        this->idx++;
        step = this->parse_value();
    }
    this->try_token_next("\n");
    auto statements = this->parse_statement_group("endfor");
    return std::make_unique<AST::ForLoop>(std::move(init), std::move(end),
        std::move(step), std::move(statements));
}

std::unique_ptr<AST::GotoLabel> Parser::parse_goto_label() {
    auto name = this->parse_id(this->token_next());
    this->try_token_next(":");
    return std::make_unique<AST::GotoLabel>(name);
}

std::unique_ptr<AST::GotoStatement> Parser::parse_goto_statement() {
    this->try_token_next("Goto");
    auto label = this->parse_id(this->token_next());
    return std::make_unique<AST::GotoStatement>(label);
}

std::unique_ptr<AST::Statement> Parser::parse_statement() {
    std::unique_ptr<AST::Statement> node = nullptr;
    
    auto first_token = this->token_get(this->idx);
    if (first_token == "\n") {
        this->token_next();
        first_token = this->token_get(this->idx);
    }
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
    else if (first_keyword == "goto") {
        node = parse_goto_statement();
    }
    else {
        auto second_token = this->token_get(this->idx+1);
        if (second_token == ":") {
            node = parse_goto_label();
        }
        else if (second_token == "=") {
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
                node = parse_stdlib_call(false);
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

std::unique_ptr<AST::Node> Parser::parse_next() {
    std::unique_ptr<AST::Node> node = nullptr;
    
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