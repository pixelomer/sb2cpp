#include <string>
#include <exception>
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
        AST::Value *parse_value();
        AST::Condition *parse_condition();
        AST::ConditionGroup *parse_condition_group();
    public:
        AST::Node *parse_next();
        Parser(std::string const& source) {
            this->tokens = tokenize(source);
        }
    };
}