#ifndef SMALLBASIC_PLATFORM_SDL_RENDERER_MM
#define SMALLBASIC_PLATFORM_SDL_RENDERER_MM

#include "../Renderer.hpp"

namespace SmallBasic {
	namespace Platform {
		Renderer::Renderer() {
		}

		Renderer::~Renderer() {
		}

		void Renderer::_SetBackgroundColor(Color const& backgroundColor) {
		}

		void Renderer::_Draw(enum Layer layer, Drawable const& drawable) {
		}

		void Renderer::_Clear(enum Layer layer) {
		}

		void Renderer::_ClearAll() {
		}

		Color Renderer::GetPixel(Number x, Number y) {
			return Color(0xFF, 0xFF, 0xFF);
		}

		void Renderer::_SetPixel(Number x, Number y, Color const& color) {
		}
	}
}

#endif