#ifndef SMALLBASIC_PLATFORM_RENDERER_H
#define SMALLBASIC_PLATFORM_RENDERER_H

#include "config.hpp"
#include "../common/Color.hpp"
#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include <cmath>
#include <queue>

#if defined(SMALLBASIC_APPLE)
#include <CoreGraphics/CoreGraphics.h>
@class NSView;
#elif defined(SMALLBASIC_SDL)
#include <SDL.h>
#include <SDL_ttf.h>
#endif

namespace SmallBasic {
	namespace Platform {
		class Renderer {
		private:
			struct Resource {
				String name;
				void *data;
				int size;

				Resource(): data(NULL) {}

				void Set(String const& name, void *data, int size) {
					this->data = data;
					this->size = size;
					this->name = name;
				}
				Resource &operator=(Resource const& res) {
					Set(res.name, res.data, res.size);
					return *this;
				}

				static String NormalizedFontName(String name, bool bold, bool italic) {
					String fullName = name + L"_";
					if (bold) fullName += L"Bold";
					if (italic) fullName += L"Italic";
					if (!bold && !italic) fullName += L"Regular";
					return fullName + L".ttf";
				}
			};

			static std::map<String, Resource> _resources;
			static String _defaultFontName;
		public:
			enum Layer {
				BACKGROUND_LAYER = 0,
				SHAPE_LAYER = 1,
				CONTROL_LAYER = 2
			};

#if defined(SMALLBASIC_APPLE)
		private:
			std::map<enum Layer, CGContextRef> _layers; //FIXME: sorted maps?
			CGColorSpaceRef _colorSpace;
			Number _yOffset;
			CGContextRef _GetLayer(enum Layer layer);
			CGContextRef _GetLayer(enum Layer layer, Number minWidth, Number minHeight);
			uint8_t *_GetPixelAddress(Number x, Number y);
		public:
			Renderer(Number yOffset);
			~Renderer();
			void Render(NSView *view);
#elif defined(SMALLBASIC_SDL)
		private:
			SDL_Renderer *_renderer;
			struct {
				String name;
				int size;
				bool bold;
				bool italic;
				TTF_Font *font = NULL;
			} _activeFont;

			SDL_Texture *_GetLayer(enum Layer layer);
			SDL_Texture *_GetLayer(enum Layer layer, Number minWidth, Number minHeight);
			Uint32 *_GetPixelAddress(Number x, Number y);
			std::map<enum Layer, SDL_Texture *> _layers;
		public:
			void Render();
			Renderer(SDL_Renderer *renderer): _renderer(renderer) {}
			~Renderer();
#endif

		private:
			Color _backgroundColor;
			void _SetBackgroundColor(Color const& backgroundColor);
			void _Draw(enum Layer layer, Drawable const& drawable);
			void _Clear(enum Layer layer);
			void _ClearAll();
			void _SetPixel(Number x, Number y, Color const& color);
		public:
			bool changed;

			static void RegisterResource(String const& name, void *src, int size) {
				_resources[name].Set(name, src, size);
			}
			static void LinkResource(String const& orig, String const& dest) {
				_resources[dest] = _resources.at(orig);
			}
			static void SetDefaultFontName(String const& name) {
				_defaultFontName = name;
			}
			void SetBackgroundColor(Color const& backgroundColor) {
				changed = true;
				_backgroundColor = backgroundColor;
				_SetBackgroundColor(backgroundColor);
			}
			void Draw(enum Layer layer, Drawable const& drawable) {
				changed = true;
				if (!drawable.visible) {
					return;
				}
				_Draw(layer, drawable);
			}
			void Clear(enum Layer layer) {
				changed = true;
				_Clear(layer);
			}
			void ClearAll() {
				changed = true;
				_ClearAll();
			}
			Color GetPixel(Number x, Number y);
		};
		std::map<String, Renderer::Resource> Renderer::_resources = {};
		String Renderer::_defaultFontName = L"Arial";
	}
}

#if defined(SMALLBASIC_APPLE)
#include "macos/Renderer.mm"
#elif defined(SMALLBASIC_SDL)
#include "sdl/Renderer.hpp"
#endif

#endif