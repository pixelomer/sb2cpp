#pragma once

#include "macros.hpp"
#include <map>

SB_CLASS(Array)

SB_PRIVATE
std::map<std::string, std::map<std::string, Obj>> arrays;

SB_METHOD_3(SetValue) (std::string const& arrayName, SB_ARG index, SB_ARG value) {
    arrays[arrayName][(std::string)index] = value;
    return SB_VOID;
}

SB_METHOD_2(GetValue) (std::string const& arrayName, SB_ARG index) {
    auto &array = arrays[arrayName];
    std::string key = index;
    if (array.count(key) == 0) {
        return Obj();
    }
    return array[key];
}

SB_METHOD_2(RemoveValue) (std::string const& arrayName, SB_ARG index) {
    arrays[arrayName].erase((std::string)index);
    return SB_VOID;
}

SB_METHOD_1(GetAllIndices) (SB_ARG obj) {
    Obj indices;
    long indicesIdx = 1;
    size_t idx = 0;
    Obj::Substr key, value;
    while (obj.array_iter(idx, key, value)) {
        indices[Obj(indicesIdx++)] = Obj(key.str);
    }
    return indices;
}

SB_METHOD_1(GetItemCount) (SB_ARG obj) {
    long count = 0;
    size_t idx = 0;
    Obj::Substr key, value;
    while (obj.array_iter(idx, key, value)) {
        count++;
    }
    return count;
}

SB_METHOD_1(IsArray) (SB_ARG obj) {
    return (GetItemCount(obj) > Obj(0)) ? SB_TRUE : SB_FALSE;
}

SB_METHOD_2(ContainsValue) (SB_ARG obj, SB_ARG val) {
    size_t idx = 0;
    Obj::Substr key, value;
    while (obj.array_iter(idx, key, value)) {
        if (Obj(value.str) == val) {
            return SB_TRUE;
        }
    }
    return SB_FALSE;
}

SB_METHOD_2(ContainsIndex) (SB_ARG obj, SB_ARG expectedKey) {
    size_t idx = 0;
    Obj::Substr key, value;
    while (obj.array_iter(idx, key, value)) {
        if (Obj(key.str) == expectedKey) {
            return SB_TRUE;
        }
    }
    return SB_FALSE;
}

SB_CLASS_END