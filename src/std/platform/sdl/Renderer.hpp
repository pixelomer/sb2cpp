#ifndef SMALLBASIC_PLATFORM_SDL_RENDERER_MM
#define SMALLBASIC_PLATFORM_SDL_RENDERER_MM

#include <SDL2/SDL.h>
#include "../Renderer.hpp"

namespace SmallBasic {
	namespace Platform {
		static Uint32 SDLColorFromSBColor(Color const& color, Number opacity = 100.0) {
			return (color.r << 24) | (color.g << 16) || (color.b << 8) |
				(Uint8)((opacity / 100.0) * 255.0);
		}
		static Color SBColorFromSDLColor(Uint32 color) {
			return Color(color >> 24, (color >> 16) & 0xFF, (color >> 8) & 0xFF);
		}
		SDL_Texture *Renderer::SDLLayer::GetTexture() {
			if (texture) {
				return texture;
			}
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			return texture;
		}

		void Renderer::SDLLayer::Destroy() {
			if (surface != NULL) {
				SDL_DestroyRenderer(renderer);
				renderer = NULL;
				SDL_FreeSurface(surface);
				surface = NULL;
			}
			if (texture != NULL) {
				SDL_DestroyTexture(texture);
				texture = NULL;
			}
		}

		void Renderer::SDLLayer::Reset(int width, int height) {
			SDL_Surface *newSurface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0,
				0, 0);
			SDL_Renderer *newRenderer = SDL_CreateSoftwareRenderer(newSurface);
			if (surface != NULL) {
				SDL_Rect rect = { .x = 0, .y = 0, .w = surface->w, .h = surface->h };
				SDL_Texture *texture = GetTexture();
				SDL_RenderCopy(newRenderer, texture, &rect, &rect);
				SDL_DestroyRenderer(renderer);
				SDL_FreeSurface(surface);
			}
			surface = newSurface;
			renderer = newRenderer;
		}
		
		Renderer::Renderer() {
		}

		Renderer::SDLLayer &Renderer::_GetLayer(enum Layer layer, Number minWidth,
			Number minHeight)
		{
			SDLLayer &layerData = _layers[layer];
			SDL_Surface *surface = layerData.surface;
			if (surface == NULL || surface->w < minWidth || surface->h < minHeight) {
				layerData.Reset((int)minWidth, (int)minHeight);
			}
			return layerData;
		}

		Renderer::~Renderer() {
			for (auto &pair : _layers) {
				pair.second.Destroy();
			}
		}

		Renderer::SDLLayer &Renderer::_GetLayer(enum Layer layer) {
			return _GetLayer(layer, 0, 0);
		}

		void Renderer::_SetBackgroundColor(Color const& backgroundColor) {
		}

		void Renderer::_Draw(enum Layer layer, Drawable const& drawable) {
		}

		void Renderer::_Clear(enum Layer layer) {
			SDL_Surface *surface = _GetLayer(layer).surface;
			SDL_FillRect(surface, NULL, 0x00000000);
		}

		void Renderer::_ClearAll() {
			for (auto &pair : _layers) {
				_Clear(pair.first);
			}
		}

		Uint32 *Renderer::_GetPixelAddress(Number x, Number y) {
			SDL_Surface *surface = _GetLayer(BACKGROUND_LAYER).surface;
			if (y >= surface->h || x >= surface->w) {
				return NULL;
			}
			return (Uint32 *)((Uint8 *)surface->pixels + (int)y * surface->pitch +
				(int)x * surface->format->BytesPerPixel);
		}

		Color Renderer::GetPixel(Number x, Number y) {
			Uint32 *pixel = _GetPixelAddress(x, y);
			if (pixel == NULL) return Color(L"white");
			return SBColorFromSDLColor(*pixel);
		}

		void Renderer::_SetPixel(Number x, Number y, Color const& color) {
			Uint32 *pixel = _GetPixelAddress(x, y);
			if (pixel == NULL) return;
			_GetLayer(BACKGROUND_LAYER).texture = NULL;
			*pixel = SDLColorFromSBColor(color);
		}

		void Renderer::Render(SDL_Renderer *renderer) {
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, _backgroundColor.r,
				_backgroundColor.g, _backgroundColor.b, 255);
			SDL_RenderClear(renderer);
			for (auto pair : _layers) {
				auto &layer = pair.second;
				SDL_Texture *texture = layer.GetTexture();
				SDL_Rect rect = { .x = 0, .y = 0 };
				SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
				SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
				SDL_RenderCopy(renderer, texture, NULL, &rect);
			}
			SDL_RenderPresent(renderer);
		}
	}
}

#endif