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
				MOUSE_MOVE,
				TIMER_TICK
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
		Number _timerInterval = 10.L;
		void _Run();
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
		SmallBasicWindow *__strong _window;
		NSTimer *__strong _timer;
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
		void PostTimerEvent() {
			_PostEvent(Event(Event::TIMER_TICK));
		}
		static Platform *Default() {
			if (_default == nullptr) {
				_default = new Platform();
				_default->SetTimerInterval(_default->_timerInterval);
			}
			return _default;
		}

		bool ignoresKeyEvents = true;
		void (*onKeyDown)(String const& key);
		void (*onKeyUp)(String const& key);
		void (*onMouseDown)();
		void (*onMouseUp)();
		void (*onMouseMove)(Number x, Number y);
		void (*onTimerTick)();

		Platform();
		~Platform();

		/* Run loop */
		void Run(void (*mainFunction)()) {
			mainFunction();

			while (IsTimerActive() || IsWindowVisible() || _eventQueue.size() > 0) {
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
						case Event::TIMER_TICK:
							if (onTimerTick != NULL)
								onTimerTick();
							break;
					}
					_eventQueue.pop();
				}

				// Wait for events
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
		void ShowMessage(String const& text, String const& title);

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

		/* Timer */
		void SetTimerInterval(Number milliseconds) {
			_timerInterval = milliseconds;
			_UpdateTimerInterval();
		}
		Number GetTimerInterval() {
			return _timerInterval;
		}
		bool IsTimerActive();
		void PauseTimer();
		void ResumeTimer();
	};

	Platform *Platform::_default = nullptr;
}

#if defined(__APPLE__) && defined(__OBJC__)
#include "Platform_macOS.mm"
#endif

#endif