#ifndef _SMALLBASIC_COLOR_H
#define _SMALLBASIC_COLOR_H

#include "Types.hpp"
#include <stdint.h>

namespace SmallBasic {
	class Color {
	private:
		static std::map<String, std::vector<int>> _colors;
		Color(uint8_t r, uint8_t g, uint8_t b, String const& name): r(r), g(g), b(b),
			name(name) {}
		Color(std::vector<int> &comps, String const& name): r(comps[0]), g(comps[1]),
			b(comps[2]) {}
	public:
		uint8_t r, g, b;
		String name;
		Color &operator=(Color const& color) {
			r = color.r;
			g = color.g;
			b = color.b;
			name = color.name;
			return *this;
		}
		static Color _FromName(String const& name) {
			if (_colors.count(name) != 0) {
				return Color(_colors[name], name);
			}
			else if (name.length() > 0 && name[0] == L'#') {
				if (name.length() == 4) {
					return Color(std::stoi(name.substr(1, 1), nullptr, 16) << 4,
						std::stoi(name.substr(2, 1), nullptr, 16) << 4,
						std::stoi(name.substr(3, 1), nullptr, 16) << 4, name);
				}
				else if (name.length() == 7) {
					return Color(std::stoi(name.substr(1, 2), nullptr, 16),
						std::stoi(name.substr(3, 2), nullptr, 16),
						std::stoi(name.substr(5, 2), nullptr, 16), name);
				}
			}
			return Color(0, 0, 0, name);
		}
	};
	std::map<String, std::vector<int>> Color::_colors = {
		{ L"Black", { 0, 0, 0 } },
		{ L"DarkBlue", { 0, 0, 128 } },
		{ L"DarkGreen", { 64, 128, 64 } },
		{ L"DarkCyan", { 64, 128, 128 } },
		{ L"DarkRed", { 128, 0, 0 } },
		{ L"DarkMagenta", { 128, 0, 128 } },
		{ L"DarkYellow", { 128, 128, 64 } },
		{ L"Gray", { 192, 192, 192 } },
		{ L"DarkGray", { 128, 128, 128 } },
		{ L"Blue", { 0, 0, 255 } },
		{ L"Green", { 128, 255, 128 } },
		{ L"Cyan", { 128, 255, 255 } },
		{ L"Red", { 255, 0, 0 } },
		{ L"Magenta", { 255, 0, 255 } },
		{ L"Yellow", { 255, 255, 128 } },
		{ L"White", { 255, 255, 255 } }
	};
}

#endif