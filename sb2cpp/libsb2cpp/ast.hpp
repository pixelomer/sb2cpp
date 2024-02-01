#pragma once

#include <string>
#include "../stdlib/RuntimeTypes.hpp"

#define ACCEPT(visit_func) virtual void accept(BaseVisitor *visitor) { \
    visitor->visit_func(this); \
}

namespace sb2cpp {
namespace AST {
    class StringValue;
    class NumberValue;
    class BinaryValueOp;
    class BinaryLogicOp;
    class BinaryCompareOp;
    class AddGroup;
    class ArrayAssign;
    class ArrayValue;
    class VariableValue;
    class StatementGroup;
    class StdlibCall;
    class StdlibValue;
    class StdlibAssign;
    class Assign;
    class SubroutineCall;
    class WhileLoop;
    class GotoLabel;
    class GotoStatement;
    class IfStatement;
    class Subroutine;
    class ForLoop;
    class BaseVisitor {
    public:
        virtual void visit_string(StringValue *) = 0;
        virtual void visit_number(NumberValue *) = 0;
        virtual void visit_value_op(BinaryValueOp *) = 0;
        virtual void visit_logic_op(BinaryLogicOp *) = 0;
        virtual void visit_compare_op(BinaryCompareOp *) = 0;
        virtual void visit_add_group(AddGroup *) = 0;
        virtual void visit_array_assign(ArrayAssign *) = 0;
        virtual void visit_array_value(ArrayValue *) = 0;
        virtual void visit_variable_value(VariableValue *) = 0;
        virtual void visit_statement_group(StatementGroup *) = 0;
        virtual void visit_stdlib_call(StdlibCall *) = 0;
        virtual void visit_stdlib_value(StdlibValue *) = 0;
        virtual void visit_stdlib_assign(StdlibAssign *) = 0;
        virtual void visit_assign(Assign *) = 0;
        virtual void visit_subroutine_call(SubroutineCall *) = 0;
        virtual void visit_while_loop(WhileLoop *) = 0;
        virtual void visit_goto_label(GotoLabel *) = 0;
        virtual void visit_goto_statement(GotoStatement *) = 0;
        virtual void visit_if_statement(IfStatement *) = 0;
        virtual void visit_subroutine(Subroutine *) = 0;
        virtual void visit_for_loop(ForLoop *) = 0;
    };
    class Visitor;
    enum ComparisonOp {
        NoComparisonOp = 0,
        Equal,
        NotEqual,
        GreaterThan,
        GreaterThanOrEqual,
        LessThan,
        LessThanOrEqual
    };
    enum LogicOp {
        NoLogicOp = 0,
        And,
        Or
    };
    enum ValueOp {
        NoValueOp = 0,
        Add = '+',
        Subtract = '-',
        Multiply = '*',
        Divide = '/'
    };
    enum SignOp {
        NoSignOp = 0,
        Positive = 1,
        Negative = -1
    };
    class Node {
    public:
        virtual ~Node() {}
        virtual void accept(BaseVisitor *visitor) {
            throw std::runtime_error("Node::accept() is not implemented");
        }
    };
    class Condition : virtual public Node {};
    class Value : virtual public Node {};
    class Statement : virtual public Node {};
    class Declaration : virtual public Node {};

    template <typename T>
    class BinaryOperation : virtual public Node {
    public:
        T *lvalue;
        T *rvalue;
        BinaryOperation(T *lvalue, T *rvalue): lvalue(lvalue),
            rvalue(rvalue) {}
        ~BinaryOperation() {
            delete lvalue;
            delete rvalue;
        }
    };
    class StringValue : virtual public Value {
    public:
        std::string value;
        StringValue(std::string value): value(value) {}
        ~StringValue() {}
        ACCEPT(visit_string)
    };
    class NumberValue : virtual public Value {
    public:
        double number;
        NumberValue(double number): number(number) {}
        ACCEPT(visit_number)
    };
    class BinaryValueOp : virtual public BinaryOperation<Value>, virtual public Value {
    public:
        AST::ValueOp op; // May only be '*' or '/'
        BinaryValueOp(Value *lvalue, Value *rvalue, AST::ValueOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
        ACCEPT(visit_value_op)
    };
    class BinaryLogicOp : virtual public BinaryOperation<Condition>, virtual public Condition {
    public:
        AST::LogicOp op;
        BinaryLogicOp(Condition *lvalue, Condition *rvalue, AST::LogicOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
        ACCEPT(visit_logic_op)
    };
    class BinaryCompareOp : virtual public BinaryOperation<Value>, virtual public Condition {
    public:
        AST::ComparisonOp op;
        BinaryCompareOp(Value *lvalue, Value *rvalue, AST::ComparisonOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
        ACCEPT(visit_compare_op)
    };
    class AddGroup : virtual public Value {
    public:
        struct AddGroupElement {
            SignOp sign;
            Value *value;
            AddGroupElement(SignOp sign, Value *value): sign(sign),
                value(value) {}
        };
        std::vector<AddGroupElement> elements;
        AddGroup(std::vector<AddGroupElement> elements): elements(elements) {}
        ~AddGroup() {
            for (auto &elem : this->elements) {
                delete elem.value;
            }
        }
        ACCEPT(visit_add_group)
    };
    class ArrayAssign : virtual public Statement {
    public:
        std::string variable;
        Value *key;
        Value *value;
        ArrayAssign(std::string variable, Value *key, Value *value):
            variable(variable), key(key), value(value) {}
        ~ArrayAssign() {
            delete key;
            delete value;
        }
        ACCEPT(visit_array_assign)
    };
    class ArrayValue : virtual public Value {
    public:
        std::string variable;
        Value *key;
        ArrayValue(std::string variable, Value *key): variable(variable),
            key(key) {}
        ~ArrayValue() {
            delete key;
        }
        ACCEPT(visit_array_value)
    };
    class VariableValue : virtual public Value {
    public:
        std::string variable;
        VariableValue(std::string variable): variable(variable) {}
        ~VariableValue() {}
        ACCEPT(visit_variable_value)
    };
    class StatementGroup : virtual public Statement {
    public:
        std::vector<Statement *> statements;
        StatementGroup(std::vector<Statement *> statements):
            statements(statements) {}
        ~StatementGroup() {
            for (auto statement : this->statements) {
                delete statement;
            }
        }
        ACCEPT(visit_statement_group)
    };
    class StdlibCall : virtual public Value, virtual public Statement {
    public:
        std::string class_name;
        std::string method_name;
        SmallBasic::Method method;
        std::vector<Value *> arguments;
        bool returns_value;
        StdlibCall(std::string class_name, std::string method_name,
            std::vector<Value *> arguments, bool returns_value):
            class_name(class_name), method_name(method_name),
            arguments(arguments), returns_value(returns_value) {}
        ~StdlibCall() {
            for (auto argument : this->arguments) {
                delete argument;
            }
        }
        ACCEPT(visit_stdlib_call)
    };
    class StdlibValue : virtual public Value {
    public:
        std::string class_name;
        std::string property;
        StdlibValue(std::string class_name, std::string property):
            class_name(class_name), property(property) {}
        ACCEPT(visit_stdlib_value)
    };
    class StdlibAssign : virtual public Statement {
    public:
        std::string class_name;
        std::string property;
        Value *value;
        StdlibAssign(std::string class_name, std::string property, Value *value):
            class_name(class_name), property(property), value(value) {}
        ~StdlibAssign() {
            if (this->value != nullptr) {
                delete this->value;
            }
        }
        ACCEPT(visit_stdlib_assign)
    };
    class ValueGroup : virtual public Value {
    public:
        // Either op or value will be valid, not both
        // Example:
        //   { '+', nullptr }
        //   { '*', nullptr }
        //   { 0, new NumberValue(5) }
        struct ValueGroupElement {
            ValueOp op;
            Value *value;
            ValueGroupElement(): op(ValueOp::NoValueOp), value(nullptr) {}
            ValueGroupElement(ValueOp op, Value *value): op(op), value(value) {}
        };
        std::vector<ValueGroupElement> values;
        Value *simplify();
        ValueGroup(std::vector<ValueGroupElement> values): values(values) {}
        ~ValueGroup() {
            for (auto const& value : this->values) {
                if (value.value != nullptr) {
                    delete value.value;
                }
            }
        }
    };
    class ConditionGroup : virtual public Condition {
    private:
        void group_by(AST::LogicOp op);
    public:
        struct ConditionGroupElement {
            LogicOp op; // None for first element
            Condition *condition;
        };
        std::vector<ConditionGroupElement> conditions;
        Condition *simplify();
        ConditionGroup(std::vector<ConditionGroupElement> const& conditions):
            conditions(conditions) {}
        ~ConditionGroup() {
            for (auto const& condition : this->conditions) {
                delete condition.condition;
            }
        }
    };
    class Assign : virtual public Statement {
    public:
        std::string variable;
        Value *value;
        Assign(std::string const& variable, Value *value):
            variable(variable), value(value) {}
        ~Assign() {
            delete this->value;
        }
        ACCEPT(visit_assign)
    };
    class SubroutineCall : virtual public Statement {
    public:
        std::string subroutine_name;
        SubroutineCall(std::string const& subroutine_name):
            subroutine_name(subroutine_name) {}
        ACCEPT(visit_subroutine_call)
    };
    class WhileLoop : virtual public Statement {
    public:
        Condition *condition;
        Statement *statement;
        WhileLoop(Condition *condition, Statement *statement):
            condition(condition), statement(statement) {}
        ~WhileLoop() {
            delete condition;
            delete statement;
        }
        ACCEPT(visit_while_loop)
    };
    class GotoLabel : virtual public Statement {
    public:
        std::string name;
        GotoLabel(std::string name): name(name) {}
        ACCEPT(visit_goto_label)
    };
    class GotoStatement : virtual public Statement {
    public:
        std::string label;
        GotoStatement(std::string label): label(label) {}
        ACCEPT(visit_goto_statement)
    };
    class IfStatement : virtual public Statement {
    public:
        struct IfStatementPart {
            Condition *condition;
            Statement *statement;
        };
        std::vector<IfStatementPart> parts;
        IfStatement(std::vector<IfStatementPart> const& parts):
            parts(parts) {}
        ~IfStatement() {
            for (auto const& part : this->parts) {
                if (part.condition) {
                    delete part.condition;
                }
                delete part.statement;
            }
        }
        ACCEPT(visit_if_statement)
    };
    class Subroutine : virtual public Declaration {
    public:
        std::string subroutine_name;
        Statement *contents;
        Subroutine(std::string subroutine_name, Statement *contents):
            subroutine_name(subroutine_name), contents(contents) {}
        ~Subroutine() {
            delete this->contents;
        }
        ACCEPT(visit_subroutine)
    };

    // For <init> To <last> (Step <step>) ... EndFor
    // <last> is internally converted into a number. <step> is not.
    // By default, <step> is 1. Both <last> and <step> are evaluated
    // again every loop.
    //
    // Examples:
    //   For i = 1 To 10                'Runs 10 times
    //   For i = 5 To 2                 'Does not run
    //   For i = 5 To 2 Step -1         'Runs 4 times
    //   For i = 5 To 2 Step -2         'Runs 2 times
    //   For i = "a" To "aaa" Step "a"  'Runs forever ("aaa" becomes 0)
    class ForLoop : virtual public Statement {
    public:
        Assign *initializer;
        Value *last_value; // inclusive
        Value *step_value; // optional
        Statement *statement;
        ForLoop(Assign *initializer, Value *last_value, Value *step_value,
            Statement *statement): initializer(initializer), last_value(last_value),
            step_value(step_value), statement(statement) {}
        ~ForLoop() {
            delete initializer;
            delete last_value;
            if (step_value != nullptr) {
                delete step_value;
            }
        }
        ACCEPT(visit_for_loop)
    };

    class Visitor : public BaseVisitor {
    public:
        virtual void visit_value_op(BinaryValueOp *op) override {
            op->lvalue->accept(this);
            op->rvalue->accept(this);
        }
        virtual void visit_logic_op(BinaryLogicOp *op) override {
            op->lvalue->accept(this);
            op->rvalue->accept(this);
        }
        virtual void visit_compare_op(BinaryCompareOp *op) override {
            op->lvalue->accept(this);
            op->rvalue->accept(this);
        }
        virtual void visit_add_group(AddGroup *group) override {
            for (auto &elem : group->elements) {
                elem.value->accept(this);
            }
        }
        virtual void visit_array_assign(ArrayAssign *assign) override {
            assign->key->accept(this);
            assign->value->accept(this);
        }
        virtual void visit_array_value(ArrayValue *value) override {
            value->key->accept(this);
        }
        virtual void visit_statement_group(StatementGroup *group) override {
            for (auto &stmt : group->statements) {
                stmt->accept(this);
            }
        }
        virtual void visit_stdlib_call(StdlibCall *call) override {
            for (auto &arg : call->arguments) {
                arg->accept(this);
            }
        }
        virtual void visit_stdlib_assign(StdlibAssign *assign) override {
            assign->value->accept(this);
        }
        virtual void visit_assign(Assign *assign) override {
            assign->value->accept(this);
        }
        virtual void visit_while_loop(WhileLoop *loop) override {
            loop->condition->accept(this);
            loop->statement->accept(this);
        }
        virtual void visit_if_statement(IfStatement *if_stmt) override {
            for (auto &part : if_stmt->parts) {
                if (part.condition != nullptr) {
                    part.condition->accept(this);
                }
                part.statement->accept(this);
            }
        }
        virtual void visit_subroutine(Subroutine *sub) override {
            sub->contents->accept(this);
        }
        virtual void visit_for_loop(ForLoop *for_loop) override {
            for_loop->initializer->accept(this);
            for_loop->last_value->accept(this);
            if (for_loop->step_value != nullptr) {
                for_loop->step_value->accept(this);
            }
            for_loop->statement->accept(this);
        }
        virtual void visit_string(StringValue *) override { }
        virtual void visit_number(NumberValue *) override { }
        virtual void visit_stdlib_value(StdlibValue *) override { }
        virtual void visit_goto_label(GotoLabel *) override { }
        virtual void visit_goto_statement(GotoStatement *) override { }
        virtual void visit_variable_value(VariableValue *) override { }
        virtual void visit_subroutine_call(SubroutineCall *) override { }
    };

}
}

#undef ACCEPT