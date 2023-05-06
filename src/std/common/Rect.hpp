#ifndef SMALLBASIC_COMMON_RECT_H
#define SMALLBASIC_COMMON_RECT_H

#include "Mixed.hpp"
#include <stdint.h>

namespace SmallBasic {
	class Rect {
	public:
		Number x, y, width, height;
		Rect(Number x, Number y, Number width, Number height): x(x), y(y),
			width(width), height(height) {}
		Rect() = delete;
	};
}

#endif