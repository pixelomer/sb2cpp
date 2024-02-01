#pragma once

#include "macros.hpp"
#include <iostream>

SB_CLASS(TextWindow)

SB_METHOD_1(WriteLine) (SB_ARG text) {
    std::cout << (std::string)text << std::endl;
    return SB_VOID;
}

SB_METHOD_1(Write) (SB_ARG text) {
    std::cout << (std::string)text;
    return SB_VOID;
}

SB_METHOD_0(Show) () {
    std::cerr << "warning: TextWindow.Show() not available" << std::endl;
    return SB_VOID;
}

SB_CLASS_END