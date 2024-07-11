#include "interpreter.hpp"
#include "../stdlib/RunLoop.hpp"

using namespace SmallBasic;

namespace sb2cpp {

void Interpreter::run(bool use_run_loop) {
    // Initialize variables
    for (auto &sym : this->source->variables) {
        this->variables[sym.first] = "";
    }
    if (use_run_loop) {
        RunLoop::current()->run([this]() {
            this->source->entry_point->accept(this);
        });
    }
    else {
        this->source->entry_point->accept(this);
    }
}

void Interpreter::visit_string(AST::StringValue *str) {
    this->push_val(str->value);
}

void Interpreter::visit_number(AST::NumberValue *num) {
    this->push_val(num->number);
}

void Interpreter::visit_value_op(AST::BinaryValueOp *op) {
    Obj obj;
    Obj lhs = this->eval(op->lvalue);
    Obj rhs = this->eval(op->rvalue);
    switch (op->op) {
        case AST::Multiply: obj = lhs * rhs; break;
        case AST::Divide:   obj = lhs / rhs; break;
        default: break;
    }
    this->push_val(obj);
}

void Interpreter::visit_logic_op(AST::BinaryLogicOp *op) {
    bool result = false;
    bool lhs = this->eval(op->lvalue);
    bool rhs = this->eval(op->rvalue);
    switch (op->op) {
        case AST::And: result = lhs && rhs; break;
        case AST::Or:  result = lhs || rhs; break;
        default: break;
    }
    this->push_cond(result);
}

void Interpreter::visit_compare_op(AST::BinaryCompareOp *op) {
    bool result = false;
    Obj lhs = this->eval(op->lvalue);
    Obj rhs = this->eval(op->rvalue);
    switch (op->op) {
        case AST::Equal:              result = lhs == rhs; break;
        case AST::GreaterThan:        result = lhs >  rhs; break;
        case AST::GreaterThanOrEqual: result = lhs >= rhs; break;
        case AST::LessThan:           result = lhs <  rhs; break;
        case AST::LessThanOrEqual:    result = lhs <= rhs; break;
        case AST::NotEqual:           result = lhs != rhs; break;
        default: break;
    }
    this->push_cond(result);
}

void Interpreter::visit_add_group(AST::AddGroup *group) {
    Obj obj;
    for (auto &elem : group->elements) {
        Obj rhs = this->eval(elem->value);
        switch (elem->sign) {
            case AST::Positive: obj += rhs; break;
            case AST::Negative: obj -= rhs; break;
            case AST::NoSignOp: obj += rhs; break;
        }
    }
    this->push_val(obj);
}

void Interpreter::visit_array_assign(AST::ArrayAssign *assign) {
    Obj &var = this->var(assign->variable);
    std::vector<Obj> keys;
    for (auto &key : assign->keys) {
        keys.push_back(this->eval(key));
    }
    var[keys] = this->eval(assign->value);
}

void Interpreter::visit_array_value(AST::ArrayValue *value) {
    Obj &var = this->var(value->variable);
    std::vector<Obj> keys;
    for (auto &key : value->keys) {
        keys.push_back(this->eval(key));
    }
    this->push_val(var[keys]);
}

void Interpreter::visit_variable_value(AST::VariableValue *value) {
    if (value->is_subroutine) {
        throw std::runtime_error("Cannot evaluate subroutine as a variable");
    }
    Obj &var = this->var(value->variable);
    this->push_val(var);
}

void Interpreter::visit_statement_group(AST::StatementGroup *group) {
    for (auto &statement : group->statements) {
        statement->accept(this);
    }
}

void Interpreter::visit_stdlib_call(AST::StdlibCall *call) {
    auto method = call->method;

    std::vector<Obj> args;
    for (unsigned int i=0; i<method->argc; i++) {
        args.push_back(this->eval(call->arguments[i]));
    }
    
    Obj ret = method->handler(args);
    if (call->returns_value) {
        this->push_val(ret);
    }
}

void Interpreter::visit_stdlib_value(AST::StdlibValue *value) {
    this->push_val(value->property->value_getter());
}

void Interpreter::visit_stdlib_assign(AST::StdlibAssign *assign) {
    auto property = assign->property;

    auto rvalue = dynamic_cast<AST::VariableValue *>(assign->value.get());
    if (rvalue != nullptr && rvalue->is_subroutine) {
        auto sub_name = rvalue->variable;
        auto callback = [sub_name, this]() {
            AST::SubroutineCall call(sub_name);
            call.accept(this);
        };
        property->callback_setter(callback);
    }
    else {
        property->value_setter(this->eval(assign->value));
    }
}

void Interpreter::visit_assign(AST::Assign *assign) {
    Obj &var = this->var(assign->variable);
    var = this->eval(assign->value);
}

void Interpreter::visit_subroutine_call(AST::SubroutineCall *call) {
    auto &subroutine = this->source->subroutines.at(call->subroutine_name);
    subroutine.node->contents->accept(this);
}

void Interpreter::visit_while_loop(AST::WhileLoop *loop) {
    while (this->eval(loop->condition)) {
        loop->statement->accept(this);
    }
}

void Interpreter::visit_goto_label(AST::GotoLabel *) {
    //no-op
}

#warning visit_goto_statement() not implemented
void Interpreter::visit_goto_statement(AST::GotoStatement *) {
    throw std::runtime_error("goto not implemented");
}

void Interpreter::visit_if_statement(AST::IfStatement *stmt) {
    for (auto &part : stmt->parts) {
        if (part->condition == nullptr || this->eval(part->condition)) {
            part->statement->accept(this);
            break;
        }
    }
}

void Interpreter::visit_subroutine(AST::Subroutine *) {
    //no-op
}

void Interpreter::visit_for_loop(AST::ForLoop *loop) {
    auto &assign = loop->initializer;
    Obj &acc = this->var(assign->variable);
    assign->accept(this);

    while (true) {
        Obj end = this->eval(loop->last_value);
        Obj step;
        if (loop->step_value != nullptr) {
            step = this->eval(loop->step_value);
        }
        else {
            step = 1;
        }
        if ((step >= Obj(0) && acc <= end) || (step <= Obj(0) && acc >= end)) {
            loop->statement->accept(this);
        }
        else break;
        acc += step;
    }
}

void Interpreter::visit_truthy_op(AST::TruthyOp *truthy_op) {
    Obj obj;
    obj = this->eval(truthy_op->value);
    this->push_cond(obj.truthy());
}


}