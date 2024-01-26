#include <string>

namespace sb2cpp::AST {
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
    };
    class NumberValue : virtual public Value {
    public:
        double number;
        NumberValue(double number): number(number) {}
    };
    class BinaryValueOp : virtual public BinaryOperation<Value>, virtual public Value {
    public:
        AST::ValueOp op;
        BinaryValueOp(Value *lvalue, Value *rvalue, AST::ValueOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
    };
    class BinaryLogicOp : virtual public BinaryOperation<Condition>, virtual public Condition {
    public:
        AST::LogicOp op;
        BinaryLogicOp(Condition *lvalue, Condition *rvalue, AST::LogicOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
    };
    class BinaryCompareOp : virtual public BinaryOperation<Value>, virtual public Condition {
    public:
        AST::ComparisonOp op;
        BinaryCompareOp(Value *lvalue, Value *rvalue, AST::ComparisonOp op):
            BinaryOperation(lvalue, rvalue), op(op) {}
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
        ComparisonOp comparison;
        SingleCondition(Value *lvalue, Value *rvalue, ComparisonOp comparison):
            lvalue(lvalue), rvalue(rvalue), comparison(comparison) {}
        ~SingleCondition() {
            if (lvalue != nullptr) delete lvalue;
            if (rvalue != nullptr) delete rvalue;
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
    };
    class SubroutineCall : virtual public Statement {
    public:
        std::string subroutine_name;
        SubroutineCall(std::string const& subroutine_name):
            subroutine_name(subroutine_name) {}
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