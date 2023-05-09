#ifndef SMALLBASIC_PLATFORM_RENDERER_H
#define SMALLBASIC_PLATFORM_RENDERER_H

#include "config.hpp"
#include "../common/Color.hpp"
#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include <cmath>
#include <queue>
#ifdef SMALLBASIC_APPLE
#include <CoreGraphics/CoreGraphics.h>
@class NSView;
#endif

namespace SmallBasic {
	namespace Platform {
		class Renderer {
		public:
			enum Layer {
				BACKGROUND_LAYER = 0,
				SHAPE_LAYER = 1,
				CONTROL_LAYER = 2
			};

#ifdef SMALLBASIC_APPLE
		private:
			std::map<enum Layer, CGContextRef> _layers; //FIXME: sorted maps?
			CGColorSpaceRef _colorSpace;
			CGContextRef _GetLayer(enum Layer layer);
			CGContextRef _GetLayer(enum Layer layer, Number minWidth, Number minHeight);
			uint8_t *_GetPixelAddress(Number x, Number y);
		public:
			void Render(NSView *view);
#endif

		private:
			static Renderer *_default;
			Color _backgroundColor;
			void _SetBackgroundColor(Color const& backgroundColor);
			void _Draw(enum Layer layer, Drawable const& drawable);
			void _Clear(enum Layer layer);
			void _ClearAll();
			void _SetPixel(Number x, Number y, Color const& color);
		public:
			bool changed;

			static Renderer *Default() {
				if (_default == nullptr) {
					_default = new Renderer;
				}
				return _default;
			}
			Renderer();
			~Renderer();
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
			void SetPixel(Number x, Number y, Color const& color) {
				changed = true;
				_SetPixel(x, y, color);
			}
			Color GetPixel(Number x, Number y);
		};

		Renderer *Renderer::_default = nullptr;
	}
}

#if defined(SMALLBASIC_APPLE)
#include "macos/Renderer.mm"
#elif defined(SMALLBASIC_SDL)
#include "sdl/Renderer.hpp"
#endif

#endif