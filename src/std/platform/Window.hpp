#ifndef SMALLBASIC_PLATFORM_WINDOW_HPP
#define SMALLBASIC_PLATFORM_WINDOW_HPP

#include "../common/Color.hpp"
#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include "../common/Rect.hpp"
#include "EventQueue.hpp"
#include "Renderer.hpp"
#if defined(__APPLE__) && defined(__OBJC__)
@class SmallBasicWindow;
#endif
#include <cmath>
#include <queue>

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

namespace SmallBasic {
	namespace Platform {
		class Window {
		private:
#if defined(__APPLE__) && defined(__OBJC__)
			SmallBasicWindow *__strong _window;
#endif
			static Window *_default;
		public:
			Renderer *renderer;
			EventQueue *eventQueue;
			void SetVisible(bool visible);
			void SetResizable(bool canResize);
			void SetTitle(String const& title);
			void SetRect(Rect const& rect);
			void RedrawIfNeeded();
			Rect GetRect();
			void ShowMessage(String const& text, String const& title);

			Window(Renderer *renderer, EventQueue *eventQueue);
			~Window();

			static Window *Default() {
				if (_default == nullptr) {
					_default = new Window(Renderer::Default(), EventQueue::Default());
				}
				return _default;
			}
		};

		Window *Window::_default = nullptr;
	}
}

#if defined(__APPLE__) && defined(__OBJC__)
#include "macos/Window.mm"
#endif

#endif