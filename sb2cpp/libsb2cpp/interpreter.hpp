#pragma once

#include "source.hpp"
#include "../stdlib/Obj.hpp"
#include <stack>
#include <mutex>
#include <thread>

namespace sb2cpp {

class Interpreter : public AST::BaseVisitor {
private:
    std::shared_ptr<Source> source;
    std::mutex mutex;
    std::map<std::thread::id, std::stack<SmallBasic::Obj>> stacks;

    std::stack<SmallBasic::Obj> &get_stack() {
        std::lock_guard<std::mutex> guard(mutex);
        return stacks[std::this_thread::get_id()];
    }
    SmallBasic::Obj pop() {
        auto &stack = this->get_stack();
        auto val = stack.top();
        stack.pop();
        return val;
    }
    void push_val(SmallBasic::Obj const& obj) {
        auto &stack = this->get_stack();
        stack.push(obj);
    }
    void push_cond(bool cond) {
        this->push_val(cond ? SmallBasic::Obj(1) : SmallBasic::Obj(0));
    }
    SmallBasic::Obj eval(AST::Value *val) {
        val->accept(this);
        return this->pop();
    }
    SmallBasic::Obj eval(std::unique_ptr<AST::Value> const& val_pt) {
        return this->eval(val_pt.get());
    }
    bool eval(AST::Condition *cond) {
        cond->accept(this);
        return this->pop() == 1;
    }
    bool eval(std::unique_ptr<AST::Condition> const& val_pt) {
        return this->eval(val_pt.get());
    }
public:
    std::map<std::string, SmallBasic::Obj> variables;
    void run(bool use_run_loop = false);

    SmallBasic::Obj &var(std::string const& name) {
        return this->variables.at(name);
    }

    Interpreter(std::shared_ptr<Source> source): source(source) {}
    Interpreter(std::string const& code):
        source(std::make_shared<Source>(code)) {}

    virtual void visit_string(AST::StringValue *) override;
    virtual void visit_number(AST::NumberValue *) override;
    virtual void visit_value_op(AST::BinaryValueOp *) override;
    virtual void visit_logic_op(AST::BinaryLogicOp *) override;
    virtual void visit_compare_op(AST::BinaryCompareOp *) override;
    virtual void visit_add_group(AST::AddGroup *) override;
    virtual void visit_array_assign(AST::ArrayAssign *) override;
    virtual void visit_array_value(AST::ArrayValue *) override;
    virtual void visit_variable_value(AST::VariableValue *) override;
    virtual void visit_statement_group(AST::StatementGroup *) override;
    virtual void visit_stdlib_call(AST::StdlibCall *) override;
    virtual void visit_stdlib_value(AST::StdlibValue *) override;
    virtual void visit_stdlib_assign(AST::StdlibAssign *) override;
    virtual void visit_assign(AST::Assign *) override;
    virtual void visit_subroutine_call(AST::SubroutineCall *) override;
    virtual void visit_while_loop(AST::WhileLoop *) override;
    virtual void visit_goto_label(AST::GotoLabel *) override;
    virtual void visit_goto_statement(AST::GotoStatement *) override;
    virtual void visit_if_statement(AST::IfStatement *) override;
    virtual void visit_subroutine(AST::Subroutine *) override;
    virtual void visit_for_loop(AST::ForLoop *) override;
    virtual void visit_truthy_op(AST::TruthyOp *) override;
};

}