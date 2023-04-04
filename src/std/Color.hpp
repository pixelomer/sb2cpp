#ifndef _SMALLBASIC_COLOR_H
#define _SMALLBASIC_COLOR_H

#include "Mixed.hpp"
#include <stdint.h>

namespace SmallBasic {
	class Color {
	private:
		static std::map<String, std::vector<uint8_t>> _colors;
		Color(uint8_t r, uint8_t g, uint8_t b, String const& name): r(r), g(g), b(b),
			name(name) {}
		Color(std::vector<uint8_t> &comps, String const& name): r(comps[0]), g(comps[1]),
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
			String lowercase;
			std::transform(name.begin(), name.end(), lowercase.begin(), std::towlower);
			if (_colors.count(lowercase) != 0) {
				return Color(_colors[lowercase], name);
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
	std::map<String, std::vector<uint8_t>> Color::_colors = {
		{ L"black", { 0, 0, 0 } },
		{ L"darkblue", { 0, 0, 128 } },
		{ L"darkgreen", { 64, 128, 64 } },
		{ L"darkcyan", { 64, 128, 128 } },
		{ L"darkred", { 128, 0, 0 } },
		{ L"darkmagenta", { 128, 0, 128 } },
		{ L"darkyellow", { 128, 128, 64 } },
		{ L"gray", { 192, 192, 192 } },
		{ L"darkgray", { 128, 128, 128 } },
		{ L"blue", { 0, 0, 255 } },
		{ L"green", { 128, 255, 128 } },
		{ L"cyan", { 128, 255, 255 } },
		{ L"red", { 255, 0, 0 } },
		{ L"magenta", { 255, 0, 255 } },
		{ L"yellow", { 255, 255, 128 } },
		{ L"white", { 255, 255, 255 } }
	};
}

#endif