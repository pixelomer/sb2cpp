#pragma once

#include "macros.hpp"
#include "RunLoop.hpp"
#include "Drawable.hpp"
#include "SBColor.hpp"
#include "Math.hpp"
#include <iostream>

SB_CLASS(GraphicsWindow)

SB_PRIVATE SBColor background_color;
SB_PRIVATE SBColor brush_color;
SB_PRIVATE Obj pen_width;
SB_PRIVATE SBColor pen_color;

SB_VALUE_GETTER(BackgroundColor) () {
    RunLoop::current()->initialize_window();
    return background_color.name;
}
SB_VALUE_SETTER(BackgroundColor) (std::string const& name) {
    background_color = name;
    RunLoop::current()->background_color = background_color.color;
    RunLoop::current()->request_redraw();
}
SB_VALUE_GETTER(PenWidth) () {
    RunLoop::current()->initialize_window();
    return pen_width;
}
SB_VALUE_SETTER(PenWidth) (SB_ARG width) {
    RunLoop::current()->initialize_window();
    pen_width = width;
}
SB_VALUE_GETTER(BrushColor) () {
    RunLoop::current()->initialize_window();
    return brush_color.name;
}
SB_VALUE_SETTER(BrushColor) (std::string const& name) {
    RunLoop::current()->initialize_window();
    brush_color = name;
}
SB_VALUE_GETTER(PenColor) () {
    RunLoop::current()->initialize_window();
    return pen_color.name;
}
SB_VALUE_SETTER(PenColor) (std::string const& name) {
    RunLoop::current()->initialize_window();
    pen_color = name;
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
    int line_width = pen_width;
    Color line_color = pen_color.color;
    RunLoop::current()->request_renderer(BACKGROUND_LAYER,
        [x, y, w, h, line_width, line_color](SDL_Renderer *renderer)
    {
        DrawablePath({x,y}, { {0,0}, {w,0}, {w,h}, {0,h} },
            line_width, line_color, {}).render(renderer);
    });
    return SB_VOID;
}
SB_METHOD_4(FillRectangle) (int x, int y, int w, int h) {
    Color fill_color = brush_color.color;
    RunLoop::current()->request_renderer(BACKGROUND_LAYER,
        [x, y, w, h, fill_color](SDL_Renderer *renderer)
    {
        DrawablePath({x,y}, { {0,0}, {w,0}, {w,h}, {0,h} },
            0, {}, fill_color).render(renderer);
    });
    return SB_VOID;
}
SB_METHOD_4(DrawEllipse) (int x, int y, int w, int h) {
    
    return SB_VOID;
}
SB_METHOD_4(FillEllipse) (int x, int y, int width, int height) {
    return SB_VOID;
}
SB_METHOD_6(DrawTriangle) (int x1, int y1, int x2, int y2, int x3, int y3) {
    return SB_VOID;
}
SB_METHOD_6(FillTriangle) (int x1, int y1, int x2, int y2, int x3, int y3) {
    return SB_VOID;
}
SB_METHOD_4(DrawLine) (int x1, int y1, int x2, int y2) {
    return SB_VOID;
}
SB_METHOD_3(DrawText) (int x, int y, std::string const& text) {
    return SB_VOID;
}
SB_METHOD_4(DrawBoundText) (int x, int y, int width, std::string const& text) {
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
    SBColor old_color = brush_color;
    brush_color = color;
    FillRectangle(x, y, 1, 1);
    brush_color = old_color;
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
    return SB_VOID;
}
SB_METHOD_2(ShowMessage) (std::string const& text, std::string const& title) {
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