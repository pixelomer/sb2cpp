#include "ast.hpp"

namespace sb2cpp {

AST::Value *AST::ValueGroup::simplify() {
    auto &values = this->values;
    // Simplify addition and subtraction
    for (int start=0; start < values.size(); start++) {
        auto first = values[start];
        int sign;
        if (first.op == AST::Add) {
            sign = 1;
        }
        else if (first.op == AST::Subtract) {
            sign = -1;
        }
        else {
            continue;
        }
        int end;
        for (end=start+1; end < values.size(); end++) {
            auto next = values[end];
            if (next.op == AST::Add) {}
            else if (next.op == AST::Subtract) {
                sign *= -1;
            }
            else {
                break;
            }
        }
        values.erase(values.begin() + start + 1, values.begin() + end);
        if (sign == -1) {
            values[start].op = AST::Subtract;
        }
        else {
            values[start].op = AST::Add;
        }
    }

    // Merge multiplication and division
    for (int opidx=0; opidx < values.size(); opidx++) {
        Value *new_value;
        auto opelem = values[opidx];
        if (opelem.op == AST::Multiply) {
            new_value = new MultiplyValue(values[opidx-1].value,
                values[opidx+1].value);
        }
        else if (opelem.op == AST::Divide) {
            new_value = new DivideValue(values[opidx-1].value,
                values[opidx+1].value);
        }
        else {
            continue;
        }
        values.erase(values.begin() + opidx - 1, values.begin() + opidx + 1);
        values[opidx-1] = { AST::NoValueOp, new_value };
        opidx--;
    }

    // Merge values and signs
    for (int validx=0; validx < values.size(); validx++) {
        auto &value = values[validx];
        if (value.op != NoValueOp) {
            continue;
        }
        if (validx == 0) {
            continue;
        }
        auto sign = values[validx-1];
        value.op = sign.op;
        values.erase(values.begin() + validx - 1, values.begin() + validx);
        validx--;
    }

    // If left with only one entry with no sign, return its value
    if (values.size() == 1 && values[0].op == AST::NoValueOp) {
        auto value = values[0].value;
        values.clear();
        return value;
    }

    // Convert the value group into an add group
    std::vector<AST::AddGroup::AddGroupElement> elems;
    for (auto &value : values) {
        AST::SignOp sign = AST::SignOp::NoSignOp;
        if (value.op == AST::ValueOp::Add) {
            sign = AST::SignOp::Positive;
        }
        else if (value.op == AST::ValueOp::Subtract) {
            sign = AST::SignOp::Negative;
        }
        elems.push_back({ sign, value.value });
    }
    values.clear();

    return new AST::AddGroup(elems);
}

}