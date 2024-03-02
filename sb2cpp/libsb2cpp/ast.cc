#include "ast.hpp"

namespace sb2cpp {

std::unique_ptr<AST::Value> AST::ValueGroup::simplify() {
    auto &values = this->values;
    // Simplify addition and subtraction
    for (int start=0; start < values.size(); start++) {
        auto &first = values[start];
        int sign;
        if (first->op == AST::Add) {
            sign = 1;
        }
        else if (first->op == AST::Subtract) {
            sign = -1;
        }
        else {
            continue;
        }
        int end;
        for (end=start+1; end < values.size(); end++) {
            auto &next = values[end];
            if (next->op == AST::Add) {}
            else if (next->op == AST::Subtract) {
                sign *= -1;
            }
            else {
                break;
            }
        }
        values.erase(values.begin() + start + 1, values.begin() + end);
        if (sign == -1) {
            values[start]->op = AST::Subtract;
        }
        else {
            values[start]->op = AST::Add;
        }
    }

    // Merge multiplication and division
    for (int opidx=0; opidx < values.size(); opidx++) {
        std::unique_ptr<Value> new_value;
        auto &opelem = values[opidx];
        if (opelem->op == AST::Multiply || opelem->op == AST::Divide) {
            new_value = std::make_unique<BinaryValueOp>(
                std::move(values[opidx-1]->value),
                std::move(values[opidx+1]->value), opelem->op);
        }
        else {
            continue;
        }
        values.erase(values.begin() + opidx - 1, values.begin() + opidx + 1);
        values[opidx-1]->op = AST::NoValueOp;
        values[opidx-1]->value = std::move(new_value);
        opidx--;
    }

    // Merge values and signs
    for (int validx=0; validx < values.size(); validx++) {
        auto &value = values[validx];
        if (value->op != NoValueOp) {
            continue;
        }
        if (validx == 0) {
            continue;
        }
        auto &sign = values[validx-1];
        value->op = sign->op;
        values.erase(values.begin() + validx - 1, values.begin() + validx);
        validx--;
    }

    // If left with only one entry with no sign, return its value
    if (values.size() == 1 && values[0]->op == AST::NoValueOp) {
        auto value = std::move(values[0]->value);
        values.clear();
        return value;
    }

    // Convert the value group into an add group
    std::vector<std::unique_ptr<AST::AddGroup::AddGroupElement>> elems;
    for (auto &value : values) {
        AST::SignOp sign = AST::SignOp::NoSignOp;
        if (value->op == AST::ValueOp::Add) {
            sign = AST::SignOp::Positive;
        }
        else if (value->op == AST::ValueOp::Subtract) {
            sign = AST::SignOp::Negative;
        }
        elems.push_back(std::make_unique<AST::AddGroup::AddGroupElement>(
            sign, std::move(value->value)));
    }
    values.clear();

    return std::make_unique<AST::AddGroup>(std::move(elems));
}

void AST::ConditionGroup::group_by(AST::LogicOp group_op) {
    auto &elems = this->conditions;

    for (int i=0; i < elems.size(); i++) {
        auto &elem = elems[i];
        if (elem->op != group_op) continue;
        elems[i-1]->condition = std::make_unique<AST::BinaryLogicOp>(
            std::move(elems[i-1]->condition), std::move(elems[i]->condition),
            group_op);
        elems.erase(elems.begin() + i, elems.begin() + i + 1);
        i--;
    }
}

std::unique_ptr<AST::Condition> AST::ConditionGroup::simplify() {
    auto &elems = this->conditions;
    this->group_by(AST::And);
    this->group_by(AST::Or);
    auto condition = std::move(elems[0]->condition);
    elems.clear();
    return condition;
}

}