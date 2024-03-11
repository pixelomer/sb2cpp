#include "Graphics.hpp"
#include "macros.hpp"

namespace SmallBasic {

SBColor Graphics::background_color = "white";
SBColor Graphics::brush_color = "#6A5ACD";
Obj Graphics::pen_width = Obj(10);
SBColor Graphics::pen_color = "#000000";
Obj Graphics::font_name = Obj("Arial");
Obj Graphics::font_size = Obj(10);
Obj Graphics::font_italic = SB_FALSE;

}