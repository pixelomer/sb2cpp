#include <string>
#include <map>
#include <exception>
#include <tuple>
#include "ast.hpp"
#include "tokenize.hpp"

namespace sb2cpp {
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
    class Parser {
    private:
        static const std::map<std::string, AST::ComparisonOp> comparators;
        static const std::map<std::string, AST::LogicOp> logic_ops;
        int line = 1;
        int idx = 0;
        std::vector<std::string> tokens;
        
        std::string token_next();
        std::string try_token_next(std::string expected);
        std::string token_get(int idx);

        AST::Statement *parse_statement();
        AST::Assign *parse_assign();
        AST::StdlibAssign *parse_stdlib_assign();
        AST::IfStatement *parse_if_statement();
        AST::Subroutine *parse_subroutine();
        AST::SubroutineCall *parse_subroutine_call();
        AST::StdlibCall *parse_stdlib_call();
        AST::Value *parse_value(bool throw_on_comparator = true);
        AST::Condition *parse_condition();
        AST::WhileLoop *parse_while_loop();
        std::tuple<int, int> save_state();
        void restore_state(std::tuple<int, int> state);
        void parse_value_or_condition(AST::Value **value, AST::Condition **condition);
    public:
        AST::Node *parse_next();
        Parser(std::string const& source) {
            this->tokens = tokenize(source);
        }
    };
}