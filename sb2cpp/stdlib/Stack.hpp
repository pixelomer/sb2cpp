#pragma once

#include "macros.hpp"
#include <stack>
#include <map>

SB_CLASS(Stack)

SB_PRIVATE
std::map<std::string, std::stack<Obj>> stacks;

SB_METHOD_2(PushValue) (std::string const& key, SB_ARG value) {
    auto &stack = stacks[key];
    stack.push(value);
    return SB_VOID;
}

SB_METHOD_1(PopValue) (std::string const& key) {
    auto val = stacks[key].top();
    stacks[key].pop();
    return val;
}

SB_METHOD_1(GetCount) (std::string const& key) {
    if (stacks.count(key) == 0) {
        return 0;
    }
    return (long)(stacks.at(key).size());
}

SB_CLASS_END