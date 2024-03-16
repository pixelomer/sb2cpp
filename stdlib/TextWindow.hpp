#pragma once

#include "macros.hpp"
#include <iostream>

SB_CLASS(TextWindow)

SB_METHOD_1(WriteLine) (std::string const& text) {
    std::cout << text << std::endl;
    return SB_VOID;
}

SB_METHOD_1(Write) (std::string const& text) {
    std::cout << text;
    return SB_VOID;
}

SB_METHOD_0(Show) () {
    std::cerr << "warning: TextWindow.Show() not available" << std::endl;
    return SB_VOID;
}

SB_CLASS_END