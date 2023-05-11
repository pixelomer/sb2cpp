#ifndef _SMALLBASIC_SHAPES_H
#define _SMALLBASIC_SHAPES_H

#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include "../common/Updatable.hpp"
#include <cmath>

namespace SmallBasic {
	namespace Platform {
		class RunLoop;
	}
	namespace Std {
		class Shapes {
		private:
			friend class Platform::RunLoop;
			static Updatable<std::map<Mixed, Drawable>> _shapes;
			static Mixed _AddShape(Drawable const& drawable);
		public:
			static Mixed AddRectangle(Number width, Number height) {
				return _AddShape(Drawable(0, 0, {{0,0}, {width,0}, {width,height},
					{0,height}}, true));
			}
			
			static Mixed AddEllipse(Number width, Number height) {
				return _AddShape(Drawable(0, 0, width, height, true));
			}

			static Mixed AddText(String const& text) {
				return _AddShape(Drawable(0, 0, text));
			}

			static Mixed GetLeft(Mixed const& key) {
				if (_shapes.get().count(key) == 0) {
					std::wcerr << "Shapes::GetLeft(): invalid shape" << std::endl;
					return 0.L;
				}
				const auto& shape = _shapes.get().at(key);
				return shape.x;
			}

			static Mixed GetTop(Mixed const& key) {
				if (_shapes.get().count(key) == 0) {
					std::wcerr << "Shapes::GetLeft(): invalid shape" << std::endl;
					return 0.L;
				}
				const auto& shape = _shapes.get().at(key);
				return shape.y;
			}

			static void Remove(Mixed const& key) {
				_shapes.mut().erase(key);
			}
			
			static Mixed AddTriangle(Number x1, Number y1, Number x2, Number y2,
				Number x3, Number y3)
			{
				return _AddShape(Drawable({{x1, y1}, {x2, y2}, {x3, y3}}, true));
			}

			static Mixed AddLine(Number x1, Number y1, Number x2, Number y2) {
				return _AddShape(Drawable({ {x1, y1}, {x2, y2} }, false));
			}

			static void Move(Mixed const& shapeName, Number x, Number y) {
				_shapes.mut()[shapeName].x = x;
				_shapes.mut()[shapeName].y = y;
			}

			static void _HideAll() {
				for (auto &pair : _shapes.mut()) {
					pair.second.visible = false;
				}
			}

			static void SetOpacity(Mixed const& shapeName, Number opacity) {
				_shapes.mut().at(shapeName).opacity = opacity;
			}
		};

		Updatable<std::map<Mixed, Drawable>> Shapes::_shapes = {};
	}
}

#include "GraphicsWindow.hpp"

namespace SmallBasic {
	namespace Std {
		Mixed Shapes::_AddShape(Drawable const& drawable) {
			Mixed id = Mixed::MakeUnique();
			_shapes.mut()[id] = GraphicsWindow::_MakeDrawable(drawable);
			return id;
		}
	}
}

#endif