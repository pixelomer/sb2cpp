#pragma once

#include "RunLoop.hpp"
#include "macros.hpp"

SB_CLASS(Desktop)

SB_VALUE_GETTER(Height) () {
    int height;
    RunLoop::current()->get_desktop_size(NULL, &height);
    return height;
}

SB_VALUE_GETTER(Width) () {
    int width;
    RunLoop::current()->get_desktop_size(&width, NULL);
    return width;
}

SB_CLASS_END