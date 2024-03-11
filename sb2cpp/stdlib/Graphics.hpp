#pragma once

#include "Obj.hpp"
#include "SBColor.hpp"

namespace SmallBasic {

class Graphics {
public:
    static SBColor background_color;
    static SBColor brush_color;
    static Obj pen_width;
    static SBColor pen_color;
    static Obj font_name;
    static Obj font_size;
    static Obj font_italic;
};

}