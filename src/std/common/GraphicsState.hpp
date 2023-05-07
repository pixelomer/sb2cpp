#ifndef SMALLBASIC_COMMON_GRAPHICSSTATE_H
#define SMALLBASIC_COMMON_GRAPHICSSTATE_H

#include "Mixed.hpp"
#include "Color.hpp"

namespace SmallBasic {
	class GraphicsState {
	public:
		String fontName;
		Number fontSize;
		bool fontBold, fontItalic;
		Color penColor, brushColor;
		Number width, height;
		Number penWidth;
	};
}

#endif