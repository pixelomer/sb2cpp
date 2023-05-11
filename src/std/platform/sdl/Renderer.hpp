#ifndef SMALLBASIC_PLATFORM_SDL_RENDERER_HPP
#define SMALLBASIC_PLATFORM_SDL_RENDERER_HPP

#include <SDL2/SDL.h>
#include "../Renderer.hpp"
#include "../../common/StringUtils.hpp"
#include "SDL2_gfx/SDL2_gfx.h"

#define COLOR_ARGS(color, opacity) color.r, color.g, color.b, \
	(Uint8)((opacity / 100.0) * 255.0)

namespace SmallBasic {
	namespace Platform {
		SDL_Texture *Renderer::_GetLayer(enum Layer layer, Number minWidth,
			Number minHeight)
		{
			SDL_Texture *&texture = _layers[layer];
			int oldWidth = -1, oldHeight = -1;
			if (texture != NULL) {
				SDL_QueryTexture(texture, NULL, NULL, &oldWidth, &oldHeight);
			}
			if (texture == NULL || oldWidth < minWidth || oldHeight < minHeight) {
				SDL_Texture *newTexture = SDL_CreateTexture(_renderer,
					SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)minWidth,
					(int)minHeight);
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
				SDL_SetRenderTarget(_renderer, newTexture);
			
				if (texture != NULL) {
					SDL_Rect rect = { .x = 0, .y = 0, .w = oldWidth, .h = oldHeight };
					SDL_RenderCopy(_renderer, texture, &rect, &rect);
					SDL_DestroyTexture(texture);
				}
				texture = newTexture;
			}
			return texture;
		}

		Renderer::~Renderer() {
			SDL_SetRenderTarget(_renderer, NULL);
			for (auto &pair : _layers) {
				SDL_DestroyTexture(pair.second);
			}
		}

		SDL_Texture *Renderer::_GetLayer(enum Layer layer) {
			return _GetLayer(layer, 1, 1);
		}

		void Renderer::_SetBackgroundColor(Color const& backgroundColor) {
		}

		void Renderer::_Draw(enum Layer layer, Drawable const& drawable) {
			SDL_Texture *texture = _GetLayer(layer, drawable.graphics.width,
				drawable.graphics.height);
			SDL_SetRenderTarget(_renderer, texture);

			switch (drawable.type) {
				case Drawable::PATH: {
					auto points = drawable.GetAbsolute();
					Sint16 points_x[drawable.points.size()];
					Sint16 points_y[drawable.points.size()];
					for (size_t i=0; i<points.size(); i++) {
						points_x[i] = std::get<0>(points[i]);
						points_y[i] = std::get<1>(points[i]);
					}
					if (points.size() == 2) {
						if (drawable.graphics.penWidth <= 0) break;
						thickLineRGBA(_renderer, points_x[0], points_y[0], points_x[1],
							points_y[1], (Uint8)drawable.graphics.penWidth,
							COLOR_ARGS(drawable.graphics.penColor, drawable.opacity));
					}
					else {
						if (drawable.filled) {
							filledPolygonRGBA(_renderer, points_x, points_y, drawable.points.size(),
								COLOR_ARGS(drawable.graphics.brushColor, drawable.opacity));
						}
						polygonRGBA(_renderer, points_x, points_y, drawable.points.size(),
							COLOR_ARGS(drawable.graphics.penColor, drawable.opacity));
					}
					break;
				}
				case Drawable::OVAL: {
					if (drawable.filled) {
						aaFilledEllipseRGBA(_renderer, drawable.x + (drawable.width / 2),
							drawable.y + (drawable.height / 2), drawable.width / 2,
							drawable.height / 2, COLOR_ARGS(drawable.graphics.brushColor,
							drawable.opacity));
					}
					aaellipseRGBA(_renderer, drawable.x + (drawable.width / 2),
						drawable.y + (drawable.height / 2), drawable.width / 2,
						drawable.height / 2, COLOR_ARGS(drawable.graphics.penColor,
						drawable.opacity));
					break;
				}
				case Drawable::TEXT: {
					stringRGBA(_renderer, (Sint16)drawable.x, (Sint16)drawable.y,
						WStringToString(drawable.text).c_str(),
						COLOR_ARGS(drawable.graphics.penColor, drawable.opacity));
					break;
				}
				case Drawable::IMAGE: {
					break;
				}
			}
		}

		void Renderer::_Clear(enum Layer layer) {
			SDL_Texture *texture = _GetLayer(layer);
			SDL_SetRenderTarget(_renderer, texture);
			SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
			SDL_RenderClear(_renderer);
		}

		void Renderer::_ClearAll() {
			for (auto &pair : _layers) {
				_Clear(pair.first);
			}
		}

		Color Renderer::GetPixel(Number x, Number y) {
			SDL_Texture *texture = _GetLayer(BACKGROUND_LAYER);
			int width, height;
			SDL_QueryTexture(texture, NULL, NULL, &width, &height);
			if (y >= height || x >= width) {
				return Color(L"white");
			}
			SDL_Rect rect = { .x = (int)x, .y = (int)y, .w = 1, .h = 1 };
			uint8_t bytes[4];
			SDL_RenderReadPixels(_renderer, &rect, SDL_PIXELFORMAT_RGBA8888,
				bytes, sizeof(bytes));
			return Color(bytes[0], bytes[1], bytes[2]);
		}

		void Renderer::Render() {
			SDL_SetRenderTarget(_renderer, NULL);
			SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(_renderer, _backgroundColor.r,
				_backgroundColor.g, _backgroundColor.b, 255);
			SDL_RenderClear(_renderer);
			for (auto &pair : _layers) {
				SDL_Texture *texture = pair.second;
				SDL_Rect rect = { .x = 0, .y = 0 };
				SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
				if (SDL_RenderCopy(_renderer, texture, &rect, &rect)) std::wcerr << SDL_GetError() << std::endl;
			}
			SDL_RenderPresent(_renderer);
		}
	}
}

#endif