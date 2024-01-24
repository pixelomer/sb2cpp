#include <string>

namespace sb2cpp::AST {
    enum Comparison {
        NoComparison = 0, // Used for else statement
        Equal,
        GreaterThan,
        GreaterThanOrEqual,
        LessThan,
        LessThanOrEqual
    };
    enum ConditionOp {
        NoConditionOp = 0,
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
    };
    class Condition : virtual public Node {};
    class Value : virtual public Node {};
    class Statement : virtual public Node {};
    class Declaration : virtual public Node {};

    class StringValue : virtual public Value {
    public:
        std::string value;
        StringValue(std::string value): value(value) {}
        ~StringValue() {}
    };
    class NumberValue : virtual public Value {
    public:
        double number;
        NumberValue(double number): number(number) {}
    };
    class MultiplyValue : virtual public Value {
    public:
        Value *lvalue;
        Value *rvalue;
        MultiplyValue(Value *lvalue, Value *rvalue): lvalue(lvalue),
            rvalue(rvalue) {}
    };
    class DivideValue : virtual public Value {
    public:
        Value *lvalue;
        Value *rvalue;
        DivideValue(Value *lvalue, Value *rvalue): lvalue(lvalue),
            rvalue(rvalue) {}
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
    };
    class VariableValue : virtual public Value {
    public:
        std::string variable;
        VariableValue(std::string variable): variable(variable) {}
        ~VariableValue() {}
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
    };
    class StdlibCall : virtual public Value, virtual public Statement {
    public:
        std::string class_name;
        std::string method_name;
        std::vector<Value *> arguments;
        StdlibCall(std::string class_name, std::string method_name,
            std::vector<Value *> arguments): class_name(class_name),
            method_name(method_name), arguments(arguments) {}
        ~StdlibCall() {
            for (auto argument : this->arguments) {
                delete argument;
            }
        }
    };
    class StdlibValue : virtual public Value {
    public:
        std::string class_name;
        std::string property;
        StdlibValue(std::string class_name, std::string property):
            class_name(class_name), property(property) {}
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
    class SingleCondition : virtual public Condition {
    public:
        Value *lvalue;
        Value *rvalue;
        Comparison comparison;
        SingleCondition(Value *lvalue, Value *rvalue, Comparison comparison):
            lvalue(lvalue), rvalue(rvalue), comparison(comparison) {}
        ~SingleCondition() {
            if (lvalue != nullptr) delete lvalue;
            if (rvalue != nullptr) delete rvalue;
        }
    };
    class ConditionGroup : virtual public Condition {
    public:
        struct ConditionGroupElement {
            ConditionOp op; // None for first element
            SingleCondition *condition;
        };
        std::vector<ConditionGroupElement> conditions;
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
    };
    class SubroutineCall : virtual public Statement {
    public:
        std::string subroutine_name;
        SubroutineCall(std::string const& subroutine_name):
            subroutine_name(subroutine_name) {}
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
                delete part.condition;
                delete part.statement;
            }
        }
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
    };
}