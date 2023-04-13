#ifndef _SMALLBASIC_PLATFORM_H
#define _SMALLBASIC_PLATFORM_H

#include "Color.hpp"
#include "Mixed.hpp"
#include <cmath>
#include <queue>

#if defined(__APPLE__) && defined(__OBJC__)
#include <AppKit/AppKit.h>
@class SmallBasicWindow;
#endif

namespace SmallBasic {
	class Platform {
	private:
		class Event {
		public:
			enum EventType {
				MOUSE_DOWN,
				MOUSE_UP,
				KEY_DOWN,
				KEY_UP,
				MOUSE_MOVE
			};
			EventType type;
			String key;
			Number x;
			Number y;
			Event(EventType type, Number x, Number y): type(type), x(x), y(y) {}
			Event(EventType type, String key): type(type), key(key) {}
			Event(EventType type): type(type) {}
		};
		static Platform *_default;
		std::queue<Event> _eventQueue;
		bool _waitingForEvents = false;
		void _Run();
		void _Stop();
		void _PostEvent(Event const& event) {
			_eventQueue.push(event);
			if (_waitingForEvents) {
				_Stop();
				_waitingForEvents = false;
			}
		}
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
		void PostMouseEvent(Number x, Number y) {
			_PostEvent(Event(Event::MOUSE_MOVE, x, y));
		}
		void PostMouseEvent(bool down) {
			_PostEvent(Event(down ? Event::MOUSE_DOWN : Event::MOUSE_UP));
		}
		void PostKeyEvent(bool down, String const& key) {
			_PostEvent(Event(down ? Event::KEY_DOWN : Event::KEY_UP, key));
		}
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
		void Run(void (*mainFunction)()) {
			mainFunction();

			while (IsWindowVisible() || _eventQueue.size() > 0) {
				// Process events
				while (_eventQueue.size() > 0) {
					Event &event = _eventQueue.front();
					switch (event.type) {
						case Event::KEY_DOWN:
							if (onKeyDown != NULL)
								onKeyDown(event.key);
							break;
						case Event::KEY_UP:
							if (onKeyUp != NULL)
								onKeyUp(event.key);
							break;
						case Event::MOUSE_DOWN:
							if (onMouseDown != NULL)
								onMouseDown();
							break;
						case Event::MOUSE_UP:
							if (onMouseUp != NULL)
								onMouseUp();
							break;
						case Event::MOUSE_MOVE:
							if (onMouseMove != NULL)
								onMouseMove(event.x, event.y);
							break;
					}
					_eventQueue.pop();
				}

				// Wait for user input
				_waitingForEvents = true;
				_Run();
			}
		}
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