#pragma once

#include <string>
#include <map>
#include <exception>
#include <tuple>
#include "ast.hpp"
#include "tokenize.hpp"

namespace sb2cpp {
    class Parser {
    public:
        class SyntaxError : public std::runtime_error {
        private:
            static std::string normalize(std::string token) {
                if (token == "") return "EOF";
                else if (token == "\n") return "newline";
                else return "'" + token + "'";
            }
        public:
            int line;
            SyntaxError(int line, std::string description):
                runtime_error(description)
            {
                this->line = line;
            }
            SyntaxError(int line, std::string expected, std::string got):
                runtime_error("Expected " + normalize(expected) +
                    ", got " + normalize(got))
            {
                this->line = line;
            }
        };
    private:
        static const std::map<std::string, AST::ComparisonOp> comparators;
        static const std::map<std::string, AST::LogicOp> logic_ops;
        long line = 1;
        long idx = 0;
        std::vector<std::string> tokens;
        
        std::string token_next();
        std::string try_token_next(std::string expected);
        std::string token_get(int idx, bool commit_nl = false);

        std::unique_ptr<AST::GotoLabel> parse_goto_label();
        std::unique_ptr<AST::GotoStatement> parse_goto_statement();
        std::unique_ptr<AST::StatementGroup> parse_statement_group(std::vector<std::string> end_tokens);
        std::unique_ptr<AST::StatementGroup> parse_statement_group(std::string end_token);
        std::unique_ptr<AST::Statement> parse_statement();
        std::unique_ptr<AST::Assign> parse_assign();
        std::unique_ptr<AST::ArrayAssign> parse_array_assign();
        std::unique_ptr<AST::ArrayValue> parse_array_value();
        std::unique_ptr<AST::StdlibAssign> parse_stdlib_assign();
        std::unique_ptr<AST::IfStatement> parse_if_statement();
        std::unique_ptr<AST::Subroutine> parse_subroutine();
        std::unique_ptr<AST::SubroutineCall> parse_subroutine_call();
        std::unique_ptr<AST::StdlibCall> parse_stdlib_call(bool returns_value);
        std::unique_ptr<AST::Value> parse_value(bool throw_on_comparator = true);
        std::unique_ptr<AST::Condition> parse_condition();
        std::unique_ptr<AST::WhileLoop> parse_while_loop();
        std::unique_ptr<AST::ForLoop> parse_for_loop();
        std::string parse_id(std::string token);
        std::tuple<long, long> save_state();
        void restore_state(std::tuple<long, long> state);
        void parse_value_or_condition(std::unique_ptr<AST::Value> *value,
            std::unique_ptr<AST::Condition> *condition);
    public:
        // Returns either AST::Subroutine or AST::Statement
        std::unique_ptr<AST::Node> parse_next();
        Parser(std::string const& source) {
            this->tokens = tokenize(source);
        }
    };
}