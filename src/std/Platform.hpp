#ifndef _SMALLBASIC_PLATFORM_H
#define _SMALLBASIC_PLATFORM_H

#include "Color.hpp"
#include "Mixed.hpp"
#include <cmath>

#if defined(__APPLE__) && defined(__OBJC__)
#include <AppKit/AppKit.h>
@class SmallBasicWindow;
#endif

namespace SmallBasic {
	class Platform {
	private:
		static Platform *_default;
#if defined(__APPLE__) && defined(__OBJC__)
		SmallBasicWindow *__strong _window;
		SmallBasicWindow *_GetWindow();
		CGContextRef _context = NULL;
		CGFloat _strokeWidth;
		CGColorRef _fillColor;
		CGColorRef _strokeColor;
		CGColorSpaceRef _colorSpace;
		void _ReallocateContext(Number ldWidth, Number ldHeight);
		void _EnsureContext();
		void _SetColor(Color const& color, CGColorRef *storedColor);
		void _PrepareFill();
		void _PrepareStroke();
#endif
	public:
		static Platform *Default() {
			if (_default == nullptr) {
				_default = new Platform();
			}
			return _default;
		}

		bool ignoresKeyEvents = true;
		void (*onKeyDown)(String const& key);
		void (*onKeyUp)(String const& key);
		void (*onMouseDown)();
		void (*onMouseUp)();
		void (*onMouseMove)(Number x, Number y);

		Platform();
		~Platform();

		/* Run loop */
		void Run();
		void RunFor(Number milliseconds);

		/* Window control */
		void SetWindowVisible(bool visible);
		void SetCanResize(bool canResize);
		void SetTitle(String const& title);
		bool IsWindowVisible();
		void SetHeight(Number height);
		Number GetHeight();
		void SetWidth(Number width);
		Number GetWidth();

		/* Drawing */
		void SetBackgroundColor(Color const& color);
		void SetFillColor(Color const& color);
		void SetStrokeColor(Color const& color);
		void SetStrokeWidth(Number strokeWidth);
		void DrawRectangle(Number x, Number y, Number width, Number height, bool fill);
		void DrawEllipse(Number x, Number y, Number width, Number height, bool fill);
		void DrawTriangle(Number x1, Number y1, Number x2, Number y2, Number x3,
			Number y3, bool fill);
		void ClearWindow();
		Color GetPixel(Number x, Number y);
	};

	Platform *Platform::_default = nullptr;
}

#if defined(__APPLE__) && defined(__OBJC__)
#include "Platform_macOS.mm"
#endif

#endif