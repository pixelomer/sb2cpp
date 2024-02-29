#pragma once

#include <map>
#include <iostream>
#include "GraphicsWindow.hpp"
#include "Drawable.hpp"
#include "macros.hpp"

SB_CLASS(Shapes)

SB_PRIVATE
std::map<long, Drawable *> objects;

SB_PRIVATE
long counter;

SB_PRIVATE
Obj create_object(Drawable *drawable) {
    long id = counter++;
    objects[id] = drawable;
    redraw_objects();
    return id;
}

SB_PRIVATE
void redraw_objects() {
    //FIXME: Objects may change/disappear before this code completes
    std::vector<Drawable *> drawables;
    drawables.push_back(new DrawableClear());
    for (auto &pair : objects) {
        drawables.push_back(pair.second);
    }
    RunLoop::current()->queue_draw(ANIMATION_LAYER, drawables);
}

SB_METHOD_1(GetLeft) (long id) {
    if (objects.count(id) == 0) {
        std::cerr << "unknown object: " << id << std::endl;
        return 0;
    }
    return objects.at(id)->pos.x;
}

SB_METHOD_2(AddEllipse) (int width, int height) {
    return create_object(new DrawableOval({ 0, 0 }, width, height,
        (int)GraphicsWindow::pen_width, GraphicsWindow::pen_color.color,
        GraphicsWindow::brush_color.color));
}

SB_METHOD_2(AddRectangle) (int width, int height) {
    return create_object(new DrawablePath({ 0, 0 }, { { 0, 0 },
        { width, 0 }, { width, height }, { 0, height } },
        (int)GraphicsWindow::pen_width,
        GraphicsWindow::pen_color.color,
        GraphicsWindow::brush_color.color));
}

SB_METHOD_3(Move) (long id, double x, double y) {
    if (objects.count(id) == 0) {
        std::cerr << "unknown object: " << id << std::endl;
        return SB_VOID;
    }
    objects[id]->pos = { x, y };
    redraw_objects();
    return SB_VOID;
}

SB_CLASS_END