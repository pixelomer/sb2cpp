#pragma once

#include "RuntimeTypes.hpp"

namespace SmallBasic {

class Runtime {
public:
    static std::map<std::string, Class> classes;
    static void register_class(Class const& cls) {
        if (classes.count(cls.name) != 0) {
            throw std::runtime_error("Class '" + cls.name + "' registered "
                "multiple times");
        }
        classes[cls.name] = cls;
    }
};

}