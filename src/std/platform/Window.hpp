#ifndef SMALLBASIC_PLATFORM_WINDOW_HPP
#define SMALLBASIC_PLATFORM_WINDOW_HPP

#include "config.hpp"
#include "../common/Color.hpp"
#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include "../common/Rect.hpp"
#include "EventQueue.hpp"
#include "Renderer.hpp"
#include <cmath>
#include <queue>

#if defined(SMALLBASIC_APPLE)
@class SmallBasicWindow;
#elif defined(SMALLBASIC_SDL)
#include <SDL2/SDL.h>
#endif

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

namespace SmallBasic {
	namespace Platform {
		class Window {
		private:
			void _Initialize();
#if defined(SMALLBASIC_APPLE)
			SmallBasicWindow *__strong _window;
#elif defined(SMALLBASIC_SDL)
			SDL_Window *_window;
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

			Window(EventQueue *eventQueue): eventQueue(eventQueue) {
				_Initialize();
			}
			~Window();

			static Window *Default() {
				if (_default == nullptr) {
					_default = new Window(EventQueue::Default());
				}
				return _default;
			}
		};

		Window *Window::_default = nullptr;
	}
}

#if defined(SMALLBASIC_APPLE)
#include "macos/Window.mm"
#elif defined(SMALLBASIC_SDL)
#include "sdl/Window.hpp"
#endif

#endif