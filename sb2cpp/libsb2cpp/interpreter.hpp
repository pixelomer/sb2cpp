#pragma once

#include "source.hpp"
#include "../stdlib/Obj.hpp"
#include <stack>

namespace sb2cpp {

class Interpreter : public AST::BaseVisitor {
private:
    Source source;
    std::stack<SmallBasic::Obj> stack;

    SmallBasic::Obj pop() {
        auto val = this->stack.top();
        this->stack.pop();
        return val;
    }
    void push_val(SmallBasic::Obj const& obj) {
        this->stack.push(obj);
    }
    void push_cond(bool cond) {
        this->push_val(cond ? SmallBasic::Obj(1) : SmallBasic::Obj(0));
    }
    SmallBasic::Obj eval(AST::Value *val) {
        val->accept(this);
        return this->pop();
    }
    bool eval(AST::Condition *cond) {
        cond->accept(this);
        return this->pop() == 1;
    }
public:
    std::map<std::string, SmallBasic::Obj> variables;
    Interpreter(std::string code): source(code) {}

    void run(bool use_run_loop = false);

    SmallBasic::Obj &var(std::string const& name) {
        return this->variables.at(name);
    }

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
};

}