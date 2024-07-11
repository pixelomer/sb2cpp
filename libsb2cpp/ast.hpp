#pragma once

#include <string>
#include <memory>
#include <stdexcept>
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
    class TruthyOp;
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
        virtual void visit_truthy_op(TruthyOp *) = 0;
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
        std::unique_ptr<T> lvalue;
        std::unique_ptr<T> rvalue;
        BinaryOperation(std::unique_ptr<T> lvalue, std::unique_ptr<T> rvalue):
            lvalue(std::move(lvalue)), rvalue(std::move(rvalue)) {}
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
        ValueOp op; // May only be '*' or '/'
        BinaryValueOp(std::unique_ptr<Value> lvalue, std::unique_ptr<Value> rvalue,
            ValueOp op): BinaryOperation(std::move(lvalue), std::move(rvalue)), op(op) {}
        ACCEPT(visit_value_op)
    };
    class BinaryLogicOp : virtual public BinaryOperation<Condition>, virtual public Condition {
    public:
        LogicOp op;
        BinaryLogicOp(std::unique_ptr<Condition> lvalue, std::unique_ptr<Condition> rvalue,
            LogicOp op): BinaryOperation(std::move(lvalue), std::move(rvalue)), op(op) {}
        ACCEPT(visit_logic_op)
    };
    class BinaryCompareOp : virtual public BinaryOperation<Value>, virtual public Condition {
    public:
        ComparisonOp op;
        BinaryCompareOp(std::unique_ptr<Value> lvalue, std::unique_ptr<Value> rvalue,
            ComparisonOp op): BinaryOperation(std::move(lvalue),
            std::move(rvalue)), op(op) {}
        ACCEPT(visit_compare_op)
    };
    class TruthyOp : virtual public Condition {
    public:
        std::unique_ptr<Value> value;
        TruthyOp(std::unique_ptr<Value> value): value(std::move(value)) {}
        ACCEPT(visit_truthy_op)
    };
    class AddGroup : virtual public Value {
    public:
        struct AddGroupElement {
            SignOp sign;
            std::unique_ptr<Value> value;
            AddGroupElement(SignOp sign, std::unique_ptr<Value> value):
                sign(sign), value(std::move(value)) {}
        };
        std::vector<std::unique_ptr<AddGroupElement>> elements;
        AddGroup(std::vector<std::unique_ptr<AddGroupElement>> elements):
            elements(std::move(elements)) {}
        ACCEPT(visit_add_group)
    };
    class ArrayAssign : virtual public Statement {
    public:
        std::string variable;
        std::vector<std::unique_ptr<Value>> keys;
        std::unique_ptr<Value> value;
        ArrayAssign(std::string variable, std::vector<std::unique_ptr<Value>> keys,
            std::unique_ptr<Value> value): variable(variable),
            keys(std::move(keys)), value(std::move(value)) {}
        ACCEPT(visit_array_assign)
    };
    class ArrayValue : virtual public Value {
    public:
        std::string variable;
        std::vector<std::unique_ptr<Value>> keys;
        ArrayValue(std::string variable, std::vector<std::unique_ptr<Value>> keys):
            variable(variable), keys(std::move(keys)) {}
        ACCEPT(visit_array_value)
    };
    class VariableValue : virtual public Value {
    public:
        std::string variable;
        bool is_subroutine = false;
        VariableValue(std::string variable): variable(variable) {}
        ACCEPT(visit_variable_value)
    };
    class StatementGroup : virtual public Statement {
    public:
        std::vector<std::shared_ptr<Statement>> statements;
        StatementGroup(std::vector<std::shared_ptr<Statement>> statements):
            statements(statements) {}
        StatementGroup() {}
        ACCEPT(visit_statement_group)
    };
    class StdlibCall : virtual public Value, virtual public Statement {
    public:
        std::string class_name;
        std::string method_name;
        std::shared_ptr<SmallBasic::Class> cls;
        std::shared_ptr<SmallBasic::Method> method;
        std::vector<std::unique_ptr<Value>> arguments;
        bool returns_value;
        StdlibCall(std::string class_name, std::string method_name,
            std::vector<std::unique_ptr<Value>> arguments, bool returns_value):
            class_name(class_name), method_name(method_name),
            arguments(std::move(arguments)),
            returns_value(returns_value) {}
        ACCEPT(visit_stdlib_call)
    };
    class StdlibValue : virtual public Value {
    public:
        std::string class_name;
        std::string property_name;
        std::shared_ptr<SmallBasic::Class> cls;
        std::shared_ptr<SmallBasic::Property> property;
        StdlibValue(std::string class_name, std::string property_name):
            class_name(class_name), property_name(property_name) {}
        ACCEPT(visit_stdlib_value)
    };
    class StdlibAssign : virtual public Statement {
    public:
        std::string class_name;
        std::string property_name;
        std::shared_ptr<SmallBasic::Class> cls;
        std::shared_ptr<SmallBasic::Property> property;
        std::unique_ptr<Value> value;
        StdlibAssign(std::string class_name, std::string property_name,
            std::unique_ptr<Value> value): class_name(class_name),
            property_name(property_name), value(std::move(value)) {}
        ACCEPT(visit_stdlib_assign)
    };
    class ValueGroup : virtual public Value {
    public:
        // Either op or value will be valid, not both
        // Example:
        //   { '+', nullptr }
        //   { '*', nullptr }
        //   { 0, NumberValue(5) }
        struct ValueGroupElement {
            ValueOp op;
            std::unique_ptr<Value> value;
            ValueGroupElement(): op(ValueOp::NoValueOp), value(nullptr) {}
            ValueGroupElement(ValueOp op, Value *value): op(op), value(value) {}
        };
        std::vector<std::unique_ptr<ValueGroupElement>> values;
        // Side effect: Destroys object
        std::unique_ptr<Value> simplify();
        ValueGroup(std::vector<std::unique_ptr<ValueGroupElement>> values):
            values(std::move(values)) {}
    };
    class ConditionGroup : virtual public Condition {
    private:
        void group_by(LogicOp op);
    public:
        struct ConditionGroupElement {
            LogicOp op; // None for first element
            std::unique_ptr<Condition> condition;
            ConditionGroupElement(LogicOp op, std::unique_ptr<Condition> condition):
                op(op), condition(std::move(condition)) {}
        };
        std::vector<std::unique_ptr<ConditionGroupElement>> conditions;
        // Side effect: Destroys object
        std::unique_ptr<Condition> simplify();
        ConditionGroup(std::vector<std::unique_ptr<ConditionGroupElement>> conditions):
            conditions(std::move(conditions)) {}
    };
    class Assign : virtual public Statement {
    public:
        std::string variable;
        std::unique_ptr<Value> value;
        Assign(std::string variable, std::unique_ptr<Value> value):
            variable(variable), value(std::move(value)) {}
        ACCEPT(visit_assign)
    };
    class SubroutineCall : virtual public Statement {
    public:
        std::string subroutine_name;
        SubroutineCall(std::string subroutine_name):
            subroutine_name(subroutine_name) {}
        ACCEPT(visit_subroutine_call)
    };
    class WhileLoop : virtual public Statement {
    public:
        std::unique_ptr<Condition> condition;
        std::shared_ptr<StatementGroup> statement;
        WhileLoop(std::unique_ptr<Condition> condition,
            std::shared_ptr<StatementGroup> statement):
            condition(std::move(condition)), statement(statement) {}
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
            std::unique_ptr<Condition> condition;
            std::shared_ptr<StatementGroup> statement;
            IfStatementPart(std::unique_ptr<Condition> condition,
                std::shared_ptr<StatementGroup> statement):
                condition(std::move(condition)),
                statement(statement) {}
        };
        std::vector<std::unique_ptr<IfStatementPart>> parts;
        IfStatement(std::vector<std::unique_ptr<IfStatementPart>> parts):
            parts(std::move(parts)) {}
        ACCEPT(visit_if_statement)
    };
    class Subroutine : virtual public Declaration {
    public:
        std::string subroutine_name;
        std::shared_ptr<StatementGroup> contents;
        Subroutine(std::string subroutine_name,
            std::shared_ptr<StatementGroup> contents):
            subroutine_name(subroutine_name),
            contents(contents) {}
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
        std::unique_ptr<Assign> initializer;
        std::unique_ptr<Value> last_value; // inclusive
        std::unique_ptr<Value> step_value; // optional
        std::shared_ptr<StatementGroup> statement;
        ForLoop(std::unique_ptr<Assign> initializer,
            std::unique_ptr<Value> last_value, std::unique_ptr<Value> step_value,
            std::shared_ptr<StatementGroup> statement): initializer(std::move(initializer)),
            last_value(std::move(last_value)), step_value(std::move(step_value)),
            statement(statement) {}
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
                elem->value->accept(this);
            }
        }
        virtual void visit_array_assign(ArrayAssign *assign) override {
            for (auto &key : assign->keys) {
                key->accept(this);
            }
            assign->value->accept(this);
        }
        virtual void visit_array_value(ArrayValue *value) override {
            for (auto &key : value->keys) {
                key->accept(this);
            }
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
                if (part->condition != nullptr) {
                    part->condition->accept(this);
                }
                part->statement->accept(this);
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
        virtual void visit_truthy_op(TruthyOp *truthy_op) override {
            truthy_op->value->accept(this);
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