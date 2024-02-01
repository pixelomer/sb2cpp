#pragma once

#include "Obj.hpp"
#include <vector>

#define SB_VOID Obj()
#define SB_ARG Obj const&

#define SB_CLASS(name) \
    namespace SmallBasic { \
    class name { \
    public:
#define SB_CLASS_END };}

#define SB_METHOD(name, arglist...) \
    static Obj dispatch_##name(std::vector<Obj> const& args) { \
        return name(arglist); \
    } \
    static Obj name

#define SB_METHOD_0(name) \
    SB_METHOD(name)
#define SB_METHOD_1(name) \
    SB_METHOD(name, args[0])
#define SB_METHOD_2(name) \
    SB_METHOD(name, args[0], args[1])
#define SB_METHOD_3(name) \
    SB_METHOD(name, args[0], args[1], args[2])
#define SB_METHOD_4(name) \
    SB_METHOD(name, args[0], args[1], args[2], args[3])
#define SB_METHOD_5(name) \
    SB_METHOD(name, args[0], args[1], args[2], args[3], args[4])