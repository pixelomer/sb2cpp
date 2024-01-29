#pragma once

#include <string>
#include "ast.hpp"
#include "parser.hpp"
#include <exception>

/*

A function, a variable and a goto label may all share the same identifier.
The correct one will be used as needed.
The following is perfectly valid code:

 1  fun = 10
 2  Sub fun
 3  fun = fun + 1                 'Refers to variable on line 1
 4  EndSub
 5  GraphicsWindow.KeyDown = fun  'Refers to sub on line 2
 6  fun:
 7  fun()                         'Refers to sub on line 2
 8  TextWindow.WriteLine(fun)     'Refers to variable on line 1
 9  Program.Delay(1000)
10  Goto fun                      'Refers to label on line 6

*/

namespace sb2cpp {


class Source : virtual public AST::Visitor {
public:
    class SourceError : public std::runtime_error {
    public:
        SourceError(std::string const& msg): std::runtime_error(msg) {}
    };
    struct Symbol {
        bool defined = false;
        bool used = false;
        std::string cname;
    };
private:
    // Class.Property = <identifier>
    // <identifier> could be a variable or a subroutine
    std::vector<std::string> unknown_identifiers;

    enum RegisterType {
        Use, Define
    };
    void register_variable(std::string &name, RegisterType type);
    void register_subroutine(std::string &name, AST::Subroutine *subroutine);
    void register_goto_label(AST::GotoLabel *label);

    template<typename T>
    void verify_defined(std::map<std::string, T> &symbols) {
        auto &unknown = this->unknown_identifiers;
        std::map<std::string, T> new_symbols;
        for (auto &pair : symbols) {
            auto &name = pair.first;
            auto &symbol = pair.second;
            if (!symbol.defined) {
                throw SourceError("Undefined symbol: '" + name + "'");
            }
            auto unknown_pt = std::find(unknown.begin(), unknown.end(),
                name);
            if (unknown_pt != unknown.end()) {
                unknown.erase(unknown_pt, unknown_pt + 1);
            }
            new_symbols[symbol.cname] = symbol;
        }
        symbols = new_symbols;
    }
public:
    struct Variable : public Symbol {
    };
    struct Subroutine : public Symbol {
        AST::Subroutine *subroutine = nullptr;
    };

    Source(std::string const& code) {
        this->entry_point = new AST::StatementGroup({ });
        Parser parser(code);
        AST::Node *node;
        while ((node = parser.parse_next()) != nullptr) {
            AST::Statement *statement = dynamic_cast<AST::Statement *>(node);
            if (statement != nullptr) {
                this->entry_point->statements.push_back(statement);
            }
            node->accept(this);
        }
        verify_defined(this->variables);
        verify_defined(this->subroutines);
        if (this->unknown_identifiers.size() != 0) {
            throw SourceError("Undefined symbol: '" +
                this->unknown_identifiers[0] + "'");
        }
    }

    std::map<std::string, Variable> variables;
    std::map<std::string, Subroutine> subroutines;

    // NOT IMPLEMENTED
    std::map<std::string, AST::GotoLabel *> goto_labels;

    AST::StatementGroup *entry_point;

    virtual void visit_array_assign(AST::ArrayAssign *) override;
    virtual void visit_array_value(AST::ArrayValue *) override;
    virtual void visit_variable_value(AST::VariableValue *) override;
    virtual void visit_assign(AST::Assign *) override;
    virtual void visit_subroutine_call(AST::SubroutineCall *) override;
    virtual void visit_subroutine(AST::Subroutine *) override;
    virtual void visit_goto_label(AST::GotoLabel *) override;
    virtual void visit_stdlib_assign(AST::StdlibAssign *) override;
};

}