#pragma once

#include "Obj.hpp"
#include "util.hpp"
#include <vector>
#include <functional>
#include <map>
#include <exception>
#include <memory>

namespace SmallBasic {

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(std::string const& msg) : std::runtime_error(msg) {}
};

typedef std::function<Obj(std::vector<Obj> const&)> MethodHandler;
typedef std::function<void()> Callback;
typedef std::function<Obj()> ValueGetter;
typedef std::function<void(Obj const&)> ValueSetter;
typedef std::function<void(Callback)> CallbackSetter;

class RuntimeType {
public:
    std::string cname;
    std::string name;
    RuntimeType(std::string const& cname): cname(cname),
        name(strtolower(cname)) {}
    RuntimeType() {}
};

class Method : public RuntimeType {
public:
    MethodHandler handler;
    int argc;
    bool returns_value;
    Method(std::string const& cname, MethodHandler handler, int argc,
        bool returns_value):
        RuntimeType(cname), handler(handler),
        argc(argc), returns_value(returns_value) {}
    Method(): RuntimeType() {}
};

class Property : public RuntimeType {
public:
    ValueGetter value_getter;
    ValueSetter value_setter;
    CallbackSetter callback_setter;
    Property(std::string const& cname, ValueGetter value_getter,
        ValueSetter value_setter, CallbackSetter callback_setter):
        RuntimeType(cname), value_getter(value_getter),
        value_setter(value_setter), callback_setter(callback_setter)
    {
        if (callback_setter != nullptr && value_setter != nullptr) {
            throw RuntimeError("A property cannot have multiple setters.");
        }
    }
    Property(): RuntimeType() {}
};

class Class : public RuntimeType {
public:
    std::map<std::string, std::shared_ptr<Method>> methods;
    std::map<std::string, std::shared_ptr<Property>> properties;
    Class(std::string const& cname): RuntimeType(cname) {}
    Class(): RuntimeType() {}
    void register_method(Method const& method) {
        if (methods.count(method.name) != 0) {
            throw RuntimeError("Method '" + 
                method.name + "' registered multiple times");
        }
        this->methods[method.name] = std::make_shared<Method>(method);
    }
    void register_property(Property const& property) {
        if (properties.count(property.name) != 0) {
            throw RuntimeError("Property '" + 
                property.name + "' registered multiple times");
        }
        this->properties[property.name] = std::make_shared<Property>(property);
    }
    std::shared_ptr<Method> get_method(std::string name) {
        name = strtolower(name);
        if (this->methods.count(name) == 0) {
            throw RuntimeError("Unrecognized method: '" + this->cname +
                "." + name + "()" + "'");
        }
        return this->methods.at(name);
    }
    std::shared_ptr<Property> get_property(std::string name) {
        name = strtolower(name);
        if (this->properties.count(name) == 0) {
            throw RuntimeError("Unrecognized property: '" + this->cname +
                "." + name + "'");
        }
        return this->properties.at(name);
    }
};

}