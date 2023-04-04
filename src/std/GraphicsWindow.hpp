#ifndef _SMALLBASIC_GRAPHICSWINDOW_H
#define _SMALLBASIC_GRAPHICSWINDOW_H

#include "Mixed.hpp"
#include <SDL2/SDL.h>
#include "RunLoop.hpp"
#include "Color.hpp"

namespace SmallBasic {
	class GraphicsWindow {
	private:
		static SDL_Window *_window;
		static SDL_Renderer *_renderer;
		static Color _background;
		static Color _brushColor;

		static std::vector<char> WStringToBytes(std::wstring const& str) {
			std::mbstate_t state = std::mbstate_t();
			const wchar_t *str_pt = str.c_str();
			std::size_t len = 1 + std::wcsrtombs(nullptr, &str_pt, 0, &state);
			std::vector<char> bytes(len);
			std::wcsrtombs(&bytes[0], &str_pt, bytes.size(), &state);
			return bytes;
		}
		static void _RunLoop() {
			bool close = false;
			while (!close) {
				SDL_Event event;

				// Events management
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
						case SDL_QUIT:
							close = true;
							break;
					}
				}
			}
		}
		static SDL_Window *_GetWindow() {
			if (_window == NULL) {
				SDL_Init(SDL_INIT_AUDIO);
				_window = SDL_CreateWindow("Title", SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED, 640, 480, 0);
				RunLoop::_PrepareRunLoop(_RunLoop);
			}
			return _window;
		}
		static SDL_Renderer *_GetRenderer() {
			if (_renderer == NULL) {
				_renderer = SDL_CreateRenderer(_GetWindow(), -1, 0);
			}
			return _renderer;
		}
	public:
		// Properties
		static void _SetBackgroundColor(String const& colorName) {
			_background = Color::_FromName(colorName);
		}
		static void _SetBrushColor(String const& colorName) {
			_brushColor = Color::_FromName(colorName);
		}
		static void _SetCanResize(Mixed const& canResize) {
			SDL_SetWindowResizable(_GetWindow(), (bool)canResize ? SDL_TRUE
				: SDL_FALSE);
		}
		static Mixed _GetHeight() {
			int oldWidth, oldHeight;
			SDL_GetWindowSize(_GetWindow(), &oldWidth, &oldHeight);
			return oldHeight;
		}
		static Mixed _GetWidth() {
			int oldWidth, oldHeight;
			SDL_GetWindowSize(_GetWindow(), &oldWidth, &oldHeight);
			return oldWidth;
		}
		static void _SetHeight(Number height) {
			SDL_SetWindowSize(_GetWindow(), (int)_GetWidth().GetNumber(), (int)height);
		}
		static void _SetWidth(Number width) {
			SDL_SetWindowSize(_GetWindow(), (int)width, (int)_GetHeight().GetNumber());
		}
		static void _SetTitle(String const& text) {
			SDL_SetWindowTitle(_GetWindow(), &WStringToBytes(text)[0]);
		}

		// Operations
		static void Show() {
			SDL_ShowWindow(_GetWindow());
		}
		static void Hide() {
			SDL_HideWindow(_GetWindow());
		}
		static void DrawRectangle(Number x, Number y, Number width, Number height) {

		}
		static void FillRectangle(Number x, Number y, Number width, Number height) {

		}

		// Events
		static void _SetKeyDown(void(*callback)()) {

		}
		static void ShowMessage(String const& text, String const& title) {
			SDL_ShowSimpleMessageBox(0, &WStringToBytes(title)[0],
				&WStringToBytes(text)[0], _GetWindow());
		}
	};

	SDL_Window *GraphicsWindow::_window = NULL;
	SDL_Renderer *GraphicsWindow::_renderer = NULL;
}

#endif