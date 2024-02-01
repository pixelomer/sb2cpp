#pragma once

#include "Obj.hpp"
#include "util.hpp"
#include <vector>
#include <functional>
#include <map>

namespace SmallBasic {

typedef std::function<Obj(std::vector<Obj> const&)> MethodHandler;

class Method {
public:
    std::string cname;
    std::string name;
    MethodHandler handler;
    int argc;
    bool returns_value;
    Method(std::string const& cname, MethodHandler handler, int argc,
        bool returns_value):
        cname(cname), name(strtolower(cname)), handler(handler),
        argc(argc), returns_value(returns_value) {}
    Method() {}
};

class Class {
public:
    std::map<std::string, Method> methods;
    std::string cname;
    std::string name;
    Class(std::string const& cname): cname(cname),
        name(strtolower(cname)) {}
    Class() {}
    void register_method(Method const& method) {
        if (methods.count(method.name) != 0) {
            throw std::runtime_error("Registered method named '" + 
                method.name + "' multiple times");
        }
        this->methods[method.name] = method;
    }
};

}