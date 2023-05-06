#ifndef _SMALLBASIC_PLATFORM_H
#define _SMALLBASIC_PLATFORM_H

#include "common/Color.hpp"
#include "common/Mixed.hpp"
#include "common/Drawable.hpp"
#include "Shapes.hpp"
#include <cmath>
#include <queue>

#if defined(__APPLE__) && defined(__OBJC__)
#include <AppKit/AppKit.h>
@class SmallBasicWindow;
#endif

namespace SmallBasic {
	class Platform {
	private:
		static Platform *_default;
		std::queue<Event> _eventQueue;
		bool _waitingForEvents = false;
		Number _timerInterval = 10.L;
		void _Run();
		void _RunFor(Number milliseconds);
		void _Stop();
		void _UpdateTimerInterval();
		void _PostEvent(Event const& event) {
			_eventQueue.push(event);
			if (_waitingForEvents) {
				_Stop();
				_waitingForEvents = false;
			}
		}
#if defined(__APPLE__) && defined(__OBJC__)
		NSFont *__strong _font;
		SmallBasicWindow *__strong _window;
		NSTimer *__strong _timer;
		SmallBasicWindow *_GetWindow();
		CGContextRef _backgroundContext = NULL;
		CGContextRef _shapeContext = NULL;
		CGFloat _strokeWidth;
		CGColorRef _fillColor;
		CGColorRef _strokeColor;
		CGColorSpaceRef _colorSpace;
		void _ReallocateContext(CGContextRef &context, Number ldWidth, Number ldHeight);
		CGContextRef _EnsureContext(bool shapeLayer = false);
		void _SetColor(Color const& color, CGColorRef *storedColor);
		void _PrepareFill();
		void _PrepareStroke();
		uint8_t *_GetPixelAddress(Number x, Number y);
		void _DrawText(CGFloat x, CGFloat y, CGFloat width, String const& wText);
#endif
	public:
		bool ignoresKeyEvents = true;
		void (*onKeyDown)(String const& key);
		void (*onKeyUp)(String const& key);
		void (*onMouseDown)();
		void (*onMouseUp)();
		void (*onMouseMove)(Number x, Number y);
		void (*onTimerTick)();
		String fontName;
		Number fontSize = 20.L;
		bool boldText = false;
		bool italicText = false;
		bool fontChanged = true;

		static Platform *Default() {
			if (_default == nullptr) {
				_default = new Platform();
				_default->SetTimerInterval(_default->_timerInterval);
			}
			return _default;
		}

		Platform();
		~Platform();

		/* Window control */
		void SetWindowVisible(bool visible);
		void SetCanResize(bool canResize);
		void SetTitle(String const& title);
		void SetDimensions(Number width, Number height);
		void ShowMessage(String const& text, String const& title);

		/* Drawing */
		void SetBackgroundColor(Color const& color);
		void SetFillColor(Color const& color);
		void SetStrokeColor(Color const& color);
		void SetStrokeWidth(Number strokeWidth);
		void Draw(Drawable const& drawable, bool shapeLayer = false);
		void Clear(bool shapeLayer = false);
		Color GetPixel(Number x, Number y);
		void SetPixel(Number x, Number y, Color const& color);

		/* Timer */
		void SetTimerInterval(Number milliseconds);
		void SetTimerActive(bool active);
	};

	Platform *Platform::_default = nullptr;
}

#if defined(__APPLE__) && defined(__OBJC__)
#include "Platform_macOS.mm"
#endif

#endif