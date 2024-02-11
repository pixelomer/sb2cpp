#pragma once

#include <cstdint>
#include <vector>
#include "SDL2_gfx/SDL2_gfx.h"
#include <iostream>

namespace SmallBasic {

struct Color {
    Uint8 r=0, g=0, b=0, a=0;
    Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a): r(r), g(g),
        b(b), a(a) {}
    Color(Uint8 r, Uint8 g, Uint8 b): r(r), g(g),
        b(b), a(255) {}
    Color() {}
};

struct Vec2 {
    int x=0;
    int y=0;
    Vec2(int x, int y): x(x), y(y) {}
    Vec2() {}
};

class Drawable {
public:
    Vec2 pos;
    virtual void render(SDL_Renderer *renderer) = 0;
    Drawable(Vec2 pos): pos(pos) {}
};

class DrawableClear : public Drawable {
public:
    virtual void render(SDL_Renderer *renderer) final {
        SDL_RenderClear(renderer);
    }
    DrawableClear(): Drawable({0, 0}) {}
};

class DrawablePath : public Drawable {
public:
    std::vector<Vec2> points;
    int line_width;
    Color line_color;
    Color fill_color;

    virtual void render(SDL_Renderer *renderer) final {
        int n = points.size();
        std::vector<Vec2> const& points = this->points;
        Sint16 abs_x[n];
        Sint16 abs_y[n];
        for (int i=0; i<n; i++) {
            Vec2 const& vec = points[i];
            abs_x[i] = this->pos.x + vec.x;
            abs_y[i] = this->pos.y + vec.y;
        }
        if (n > 2 && fill_color.a > 0) {
            filledPolygonRGBA(renderer, abs_x, abs_y, n, fill_color.r,
                fill_color.g, fill_color.b, fill_color.a);
        }
        if (line_color.a > 0) {
            for (int i=0; i<n; i++) {
                thickLineRGBA(renderer, abs_x[i], abs_y[i], abs_x[(i+1)%n], abs_y[(i+1)%n],
                    (Uint8)line_width, line_color.r, line_color.r, line_color.g,
                    line_color.a);
            }
        }
    }

    DrawablePath(Vec2 pos, std::vector<Vec2> points, int line_width,
        Color line_color, Color fill_color):
        Drawable(pos), points(points), line_width(line_width),
        line_color(line_color), fill_color(fill_color) {}
};

class DrawableOval : public Drawable {
public:
    int width;
    int height;
    int line_width;
    Color line_color;
    Color fill_color;

    virtual void render(SDL_Renderer *renderer) final {
        int rx = width / 2;
        int ry = height / 2;
        int cx = pos.x + rx;
        int cy = pos.y + ry;
        if (fill_color.a > 0) {
            filledEllipseRGBA(renderer, cx, cy, rx, ry, fill_color.r,
                fill_color.g, fill_color.b, fill_color.a);
        }
        if (line_color.a > 0) {
            aaellipseRGBA(renderer, cx, cy, rx, ry, line_color.r,
                line_color.g, line_color.b, line_color.a); 
        }
    }

    DrawableOval(Vec2 pos, int width, int height, int line_width,
        Color line_color, Color fill_color): Drawable(pos), width(width),
        height(height), line_width(line_width), line_color(line_color),
        fill_color(fill_color) {}
};

}