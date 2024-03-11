#pragma once

#include "macros.hpp"
#include "RunLoop.hpp"
#include "Drawable.hpp"
#include "SBColor.hpp"
#include "Graphics.hpp"
#include "Shapes.hpp"
#include "Math.hpp"
#include <iostream>

SB_CLASS(GraphicsWindow)

SB_VALUE_SETTER(FontName) (SB_ARG name) {
    Graphics::font_name = name;
}
SB_VALUE_GETTER(FontName) () {
    return Graphics::font_name;
}
SB_VALUE_SETTER(FontItalic) (SB_ARG italic) {
    Graphics::font_italic = italic;
}
SB_VALUE_GETTER(FontItalic) () {
    return Graphics::font_italic;
}
SB_VALUE_SETTER(FontSize) (SB_ARG size) {
    Graphics::font_size = size;
}
SB_VALUE_GETTER(FontSize) () {
    return Graphics::font_size;
}
SB_VALUE_SETTER(CanResize) (SB_ARG val) {
    RunLoop::current()->set_can_resize(val.truthy());
}
SB_VALUE_GETTER(CanResize) () {
    return RunLoop::current()->get_can_resize();
}
SB_VALUE_GETTER(MouseX) () {
    return RunLoop::current()->mouse_x.load();
}
SB_VALUE_GETTER(MouseY) () {
    return RunLoop::current()->mouse_y.load();
}
SB_VALUE_GETTER(Left) () {
    int x;
    RunLoop::current()->get_window_position(&x, NULL);
    return x;
}
SB_VALUE_GETTER(Top) () {
    int y;
    RunLoop::current()->get_window_position(NULL, &y);
    return y;
}
SB_VALUE_SETTER(Left) (int x) {
    int y;
    RunLoop::current()->get_window_position(NULL, &y);
    RunLoop::current()->set_window_position(x, y);
}
SB_VALUE_SETTER(Top) (int y) {
    int x;
    RunLoop::current()->get_window_position(&x, NULL);
    RunLoop::current()->set_window_position(x, y);
}
SB_VALUE_GETTER(BackgroundColor) () {
    RunLoop::current()->initialize_window();
    return Graphics::background_color.name;
}
SB_VALUE_SETTER(BackgroundColor) (std::string const& name) {
    Graphics::background_color = name;
    RunLoop::current()->background_color =
        Graphics::background_color.color;
    RunLoop::current()->request_redraw();
}
SB_VALUE_GETTER(PenWidth) () {
    RunLoop::current()->initialize_window();
    return Graphics::pen_width;
}
SB_VALUE_SETTER(PenWidth) (SB_ARG width) {
    RunLoop::current()->initialize_window();
    Graphics::pen_width = width;
}
SB_VALUE_GETTER(BrushColor) () {
    RunLoop::current()->initialize_window();
    return Graphics::brush_color.name;
}
SB_VALUE_SETTER(BrushColor) (std::string const& name) {
    RunLoop::current()->initialize_window();
    Graphics::brush_color = name;
}
SB_VALUE_GETTER(PenColor) () {
    RunLoop::current()->initialize_window();
    return Graphics::pen_color.name;
}
SB_VALUE_SETTER(PenColor) (std::string const& name) {
    RunLoop::current()->initialize_window();
    Graphics::pen_color = name;
}
SB_VALUE_GETTER(Title) () {
    return RunLoop::current()->get_window_title();
}
SB_VALUE_SETTER(Title) (std::string const& title) {
    RunLoop::current()->set_window_title(title);
}
SB_VALUE_GETTER(Height) () {
    int height;
    RunLoop::current()->get_window_size(NULL, &height);
    return height;
}
SB_VALUE_SETTER(Height) (int height) {
    RunLoop::current()->dispatch_main_sync([height]() {
        int width;
        RunLoop::current()->get_window_size(&width, NULL);
        RunLoop::current()->set_window_size(width, height);
    });
}
SB_VALUE_GETTER(Width) () {
    int width;
    RunLoop::current()->get_window_size(&width, NULL);
    return width;
}
SB_VALUE_SETTER(Width) (int width) {
    RunLoop::current()->dispatch_main_sync([width]() {
        int height;
        RunLoop::current()->get_window_size(NULL, &height);
        RunLoop::current()->set_window_size(width, height);
    });
}
SB_VALUE_GETTER(LastKey) () {
    RunLoop::current()->initialize_window();
    return RunLoop::current()->last_key.get();
}

SB_METHOD_0(Show) () {
    RunLoop::current()->show_window();
    return SB_VOID;
}
SB_METHOD_0(Hide) () {
    RunLoop::current()->hide_window();
    return SB_VOID;
}
SB_METHOD_4(DrawRectangle) (int x, int y, int w, int h) {
    int line_width = Graphics::pen_width;
    Color line_color = Graphics::pen_color.color;
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawablePath({x,y}, { {0,0}, {w,0}, {w,h}, {0,h} },
            line_width, line_color, {}));
    return SB_VOID;
}
SB_METHOD_4(FillRectangle) (int x, int y, int w, int h) {
    Color fill_color = Graphics::brush_color.color;
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawablePath({x,y}, { {0,0}, {w,0}, {w,h}, {0,h} },
            0, {}, fill_color));
    return SB_VOID;
}
SB_METHOD_4(DrawEllipse) (int x, int y, int w, int h) {
    return SB_VOID;
}
SB_METHOD_4(FillEllipse) (int x, int y, int width, int height) {
    return SB_VOID;
}
SB_METHOD_6(DrawTriangle) (int x1, int y1, int x2, int y2, int x3, int y3) {
    int line_width = Graphics::pen_width;
    Color line_color = Graphics::pen_color.color;
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawablePath({x1,y1}, { {0,0}, {x2-x1,y2-y1}, {x3-x1,y3-y1} },
            line_width, line_color, {}));
    return SB_VOID;
}
SB_METHOD_6(FillTriangle) (int x1, int y1, int x2, int y2, int x3, int y3) {
    Color fill_color = Graphics::brush_color.color;
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawablePath({x1,y1}, { {0,0}, {x2-x1,y2-y1}, {x3-x1,y3-y1} },
            0, {}, fill_color));
    return SB_VOID;
}
SB_METHOD_4(DrawLine) (int x1, int y1, int x2, int y2) {
    int line_width = Graphics::pen_width;
    Color line_color = Graphics::pen_color.color;
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawablePath({x1,y1}, { {0,0}, {x2-x1,y2-y1} },
            line_width, line_color, {}));
    return SB_VOID;
}
SB_METHOD_3(DrawText) (int x, int y, std::string const& text) {
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawableText({x,y}, text, Graphics::brush_color.color));
    return SB_VOID;
}
SB_METHOD_4(DrawBoundText) (int x, int y, int width, std::string const& text) {
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawableText({x,y}, text, Graphics::brush_color.color, width));
    return SB_VOID;
}
SB_METHOD_5(DrawResizedImage) (std::string const& imageName, int x, int y,
    int width, int height)
{
    return SB_VOID;
}
SB_METHOD_3(DrawImage) (std::string const& imageName, int x, int y) {
    return SB_VOID;
}
SB_METHOD_3(SetPixel) (int x, int y, std::string const& color) {
    SBColor old_color = Graphics::brush_color;
    Graphics::brush_color = color;
    FillRectangle(x, y, 1, 1);
    Graphics::brush_color = old_color;
    return SB_VOID;
}
SB_METHOD_2(GetPixel) (int x, int y) {
    return SB_VOID;
}
SB_METHOD_0(GetRandomColor) () {
    RunLoop::current()->initialize_window();
    return SBColor((int)Math::GetRandomNumber(255),
        (int)Math::GetRandomNumber(255),
        (int)Math::GetRandomNumber(255)).name;
}
SB_METHOD_3(GetColorFromRGB) (int red, int green, int blue) {
    RunLoop::current()->initialize_window();
    SBColor color(red, green, blue);
    return color.name;
}
SB_METHOD_0(Clear) () {
    RunLoop::current()->queue_draw(BACKGROUND_LAYER,
        new DrawableClear());
    Shapes::Clear();
    return SB_VOID;
}
SB_METHOD_2(ShowMessage) (std::string const& text, std::string const& title) {
    RunLoop::current()->show_message(title, text);
    return SB_VOID;
}
SB_CALLBACK_SETTER(MouseMove) (Callback cb) {
    RunLoop::current()->initialize_window();
    RunLoop::current()->on_mouse_move = cb;
}
SB_CALLBACK_SETTER(KeyDown) (Callback cb) {
    RunLoop::current()->initialize_window();
    RunLoop::current()->on_key_down = cb;
}
SB_CALLBACK_SETTER(KeyUp) (Callback cb) {
    RunLoop::current()->initialize_window();
    RunLoop::current()->on_key_up = cb;
}
SB_CALLBACK_SETTER(MouseDown) (Callback cb) {
    RunLoop::current()->initialize_window();
    RunLoop::current()->on_mouse_down = cb;
}
SB_CALLBACK_SETTER(MouseUp) (Callback cb) {
    RunLoop::current()->initialize_window();
    RunLoop::current()->on_mouse_up = cb;
}

SB_CLASS_END