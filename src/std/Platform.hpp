#ifndef _SMALLBASIC_PLATFORM_H
#define _SMALLBASIC_PLATFORM_H

#include "Color.hpp"
#include "Mixed.hpp"
#include <cmath>

#if defined(__APPLE__) && defined(__OBJC__)
#include <AppKit/AppKit.h>
#endif

namespace SmallBasic {
	class Platform {
	private:
#if defined(__APPLE__) && defined(__OBJC__)
		NSWindow *__strong _window;
		NSWindow *_GetWindow();
		CGContextRef _context = NULL;
		CGFloat _strokeWidth;
		CGColorRef _fillColor;
		CGColorRef _strokeColor;
		CGColorSpaceRef _colorSpace;
		void _ReallocateContext(Number ldWidth, Number ldHeight);
		void _EnsureContext();
		void _SetColor(Color const& color, void(*setter)(CGContextRef, CGColorRef),
			CGColorRef *storedColor);
#endif
		void _Initialize();
	public:
		Platform() {
			_Initialize();
		}
		void Run();

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
	};
}

#if defined(__APPLE__) && defined(__OBJC__)
#include "Platform_macOS.mm"
#endif

#endif