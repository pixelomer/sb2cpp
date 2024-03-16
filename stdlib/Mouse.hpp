#pragma once

#include "macros.hpp"
#include "RunLoop.hpp"

SB_CLASS(Mouse)

SB_VALUE_GETTER(MouseX) () {
    return RunLoop::current()->mouse_x.load();
}

SB_VALUE_GETTER(MouseY) () {
    return RunLoop::current()->mouse_y.load();
}

SB_CLASS_END