#ifndef SMALLBASIC_PLATFORM_STUB_WINDOW_HPP
#define SMALLBASIC_PLATFORM_STUB_WINDOW_HPP

#include "../Window.hpp"

namespace SmallBasic {
	namespace Platform {
		void Window::SetVisible(bool visible) {
		}

		void Window::SetResizable(bool canResize) {
		}

		void Window::SetTitle(String const& title) {
		}

		void Window::SetRect(Rect const& newRect) {
		}

		Rect Window::GetRect() {
			return Rect(0.L, 0.L, 0.L, 0.L);
		}

		void Window::ShowMessage(String const& text, String const& title) {
		}

		void Window::RedrawIfNeeded() {
		}
		
		void Window::_Initialize() {
			
		}

		Window::~Window() {
		}
	}
}

#endif