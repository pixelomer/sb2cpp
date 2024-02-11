#pragma once

#include "Drawable.hpp"
#include "util.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>

namespace SmallBasic {

class SBColor {
private:
    static std::map<std::string, Color> colors;
    void name_init(std::string name) {
        if (name.length() >= 1 && name[0] == '#') {
            if (name.length() == 4) {
                color.r = std::stoi(name.substr(1, 1), nullptr, 16) << 4;
                color.g = std::stoi(name.substr(2, 1), nullptr, 16) << 4;
                color.b = std::stoi(name.substr(3, 1), nullptr, 16) << 4;
                color.a = 255;
            }
            else if (name.length() == 7) {
                color.r = std::stoi(name.substr(1, 2), nullptr, 16);
                color.g = std::stoi(name.substr(3, 2), nullptr, 16);
                color.b = std::stoi(name.substr(5, 2), nullptr, 16);
                color.a = 255;
            }
            else {
                color = { 0, 0, 0 };
            }
        }
        else {
            std::string lower = strtolower(name);
            if (colors.count(lower) != 0) {
                color = colors.at(name);
            }
            else {
                color = { 0, 0, 0 };
            }
        }
        this->name = name;
    }
public:
    Color color;
    std::string name;
    SBColor(std::string name) {
        name_init(name);
    }
    SBColor(const char *name) {
        name_init(name);
    }
    SBColor(uint8_t r, uint8_t g, uint8_t b) {
        color = { r, g, b, 255 };
        char str[8];
        std::snprintf(str, 8, "#%02hhX%02hhX%02hhX", r, g, b);
        str[7] = '\0';
        this->name = str;
    }
};

}