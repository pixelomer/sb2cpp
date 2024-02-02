#pragma once

#include "RuntimeTypes.hpp"

namespace SmallBasic {

class Runtime {
public:
    static std::map<std::string, Class> classes;
    static void register_class(Class const& cls) {
        if (classes.count(cls.name) != 0) {
            throw std::runtime_error("Class '" + cls.cname + "' registered "
                "multiple times");
        }
        classes[cls.name] = cls;
    }
    static Class &get_class(std::string name) {
        name = strtolower(name);
        if (classes.count(name) == 0) {
            throw std::runtime_error("Unrecognized class: '" + name + "'");
        }
        return classes.at(name);
    }
};

}