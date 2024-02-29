#pragma once

#include "source.hpp"
#include "ast.hpp"
#include "code_writer.hpp"
#include <stack>
#include <exception>

namespace sb2cpp {

class Transpiler : public AST::BaseVisitor {
private:
    std::stack<AST::Node *> node_parents;
    void write_block(AST::Statement *);
    void write_condition(AST::Condition *);
    template<typename T>
    T *get_parent() {
        AST::Node *node = this->node_parents.top();
        T *cast_node = dynamic_cast<T *>(node);
        return cast_node;
    }
public:
    Source source;
    CodeWriter out;
    class TranspilerError : public std::runtime_error {
    public:
        TranspilerError(std::string const& msg): std::runtime_error(msg) {}
    };
    Transpiler(std::string const& code): source(code) {
        this->node_parents.push(nullptr);
    }
    Transpiler(Source const& source): source(source) {
        this->node_parents.push(nullptr);
    }

    std::string transpile();

    virtual void visit_string(AST::StringValue *);
    virtual void visit_number(AST::NumberValue *);
    virtual void visit_value_op(AST::BinaryValueOp *);
    virtual void visit_logic_op(AST::BinaryLogicOp *);
    virtual void visit_compare_op(AST::BinaryCompareOp *);
    virtual void visit_add_group(AST::AddGroup *);
    virtual void visit_array_assign(AST::ArrayAssign *);
    virtual void visit_array_value(AST::ArrayValue *);
    virtual void visit_variable_value(AST::VariableValue *);
    virtual void visit_statement_group(AST::StatementGroup *);
    virtual void visit_stdlib_call(AST::StdlibCall *);
    virtual void visit_stdlib_value(AST::StdlibValue *);
    virtual void visit_stdlib_assign(AST::StdlibAssign *);
    virtual void visit_assign(AST::Assign *);
    virtual void visit_subroutine_call(AST::SubroutineCall *);
    virtual void visit_while_loop(AST::WhileLoop *);
    virtual void visit_goto_label(AST::GotoLabel *);
    virtual void visit_goto_statement(AST::GotoStatement *);
    virtual void visit_if_statement(AST::IfStatement *);
    virtual void visit_subroutine(AST::Subroutine *);
    virtual void visit_for_loop(AST::ForLoop *);
    virtual void visit_truthy_op(AST::TruthyOp *);
};

}