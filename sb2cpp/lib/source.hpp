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

class SourceError : public std::runtime_error {
public:
    SourceError(std::string const& msg): std::runtime_error(msg) {}
};

class Source : virtual public AST::Visitor {
public:
    struct Symbol {
        bool defined = false;
        bool used = false;
    };
private:
    // Class.Property = <identifier>
    // <identifier> could be a variable or a subroutine
    std::vector<std::string> unknown_identifiers;

    enum RegisterType {
        Use, Define
    };
    void register_variable(std::string const& name, RegisterType type);
    void register_subroutine(AST::Subroutine *subroutine);
    void register_goto_label(AST::GotoLabel *label);
    void register_call(std::string const& name);

    template<typename T>
    void verify_defined(std::map<std::string, T> const& symbols) {
        auto &unknown = this->unknown_identifiers;
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

        }
    }
public:
    struct Variable : public Symbol {
        std::string name;
    };
    struct Subroutine : public Symbol {
        AST::Subroutine *subroutine = nullptr;
    };

    Source(std::string const& code) {
        Parser parser(code);
        AST::Node *node;
        while ((node = parser.parse_next()) != nullptr) {
            node->accept(this);
        }
        verify_defined(this->variables);
        verify_defined(this->subroutines);
        if (this->unknown_identifiers.size() != 0) {
            throw SourceError("Undefined symbol: '" +
                this->unknown_identifiers[0] + "'");
        }
    }

    // variables["var"] = "Var"
    std::map<std::string, Variable> variables;
    // variables["sub"] = { "Sub", ... }
    std::map<std::string, Subroutine> subroutines;
    std::map<std::string, AST::GotoLabel *> goto_labels;

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