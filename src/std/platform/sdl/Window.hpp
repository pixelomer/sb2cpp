#ifndef SMALLBASIC_PLATFORM_SDL_WINDOW_MM
#define SMALLBASIC_PLATFORM_SDL_WINDOW_MM

#include "../Window.hpp"
#include "../../common/StringUtils.hpp"
#include <SDL2/SDL.h>

namespace SmallBasic {
	namespace Platform {
		void Window::SetVisible(bool visible) {
			if (visible) {
				SDL_ShowWindow(_window);
			}
			else {
				SDL_HideWindow(_window);
			}
		}

		void Window::SetResizable(bool canResize) {
			SDL_SetWindowResizable(_window, canResize ? SDL_TRUE : SDL_FALSE);
		}

		void Window::SetTitle(String const& title) {
			SDL_SetWindowTitle(_window, WStringToString(title).c_str());
		}

		void Window::SetRect(Rect const& newRect) {
			SDL_SetWindowPosition(_window, (int)newRect.x, (int)newRect.y);
			SDL_SetWindowSize(_window, (int)newRect.width, (int)newRect.height);
		}

		Rect Window::GetRect() {
			int width, height, x, y;
			SDL_GetWindowSizeInPixels(_window, &width, &height);
			SDL_GetWindowPosition(_window, &x, &y);
			return Rect(width, height, x, y);
		}

		void Window::ShowMessage(String const& text, String const& title) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
				WStringToString(title).c_str(), WStringToString(text).c_str(), _window);
		}

		void Window::RedrawIfNeeded() {
			if (renderer->changed) {
				renderer->Render(_windowRenderer);
				renderer->changed = false;
			}
		}
		
		void Window::_Initialize() {
			_window = SDL_CreateWindow("Small Basic", SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, 0);
			_windowRenderer = SDL_CreateRenderer(_window, -1, 0);
		}

		Window::~Window() {
			SDL_DestroyWindow(_window);
			_window = NULL;
		}
	}
}

#endif