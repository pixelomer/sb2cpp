#ifndef SMALLBASIC_COMMON_DRAWABLE_H
#define SMALLBASIC_COMMON_DRAWABLE_H

#include "Color.hpp"
#include "Mixed.hpp"
#include "GraphicsState.hpp"
#include <cmath>
#include <vector>
#include <tuple>
#include <limits>

namespace SmallBasic {
	class Drawable {
	public:
		enum Type {
			PATH,
			OVAL,
			IMAGE,
			TEXT
		};

		// Drawable type.
		enum Type type;

		// Path points.
		std::vector<std::tuple<Number, Number>> points;
		
		// Text contents.
		String text;

		// Opacity (between 0-100).
		Number opacity = 100.L;

		// Drawable position.
		Number x, y;

		// Oval width or text boundary.
		Number width;

		// Oval height.
		Number height;

		// Drawable visibility.
		bool visible = true;

		/* Animation */
		bool animating = false;
		Number animationStart, animationDuration;
		Number xEnd, yEnd;

		/* Shape properties */
		bool filled = false;

		/* Text properties */
		bool boundText;

		/* Graphics context at the time of creation */
		GraphicsState graphics;

		Drawable() {}

		Drawable(Number x, Number y, std::vector<std::tuple<Number, Number>> const&
			points, bool filled): type(PATH), points(points), x(x), y(y),
			filled(filled) {}

		Drawable(std::vector<std::tuple<Number, Number>> const& points, bool filled):
			type(PATH), points(points), filled(filled)
		{
			if (points.size() == 0) {
				x = 0;
				y = 0;
				return;
			}
			x = std::numeric_limits<Number>::max();
			y = std::numeric_limits<Number>::max();
			for (auto const& point : points) {
				if (std::get<0>(point) < x) {
					x = std::get<0>(point);
				}
				if (std::get<1>(point) < y) {
					y = std::get<1>(point);
				}
			}
			for (auto &point : this->points) {
				std::get<0>(point) -= x;
				std::get<1>(point) -= y;
			}
		}

		// Returns absolute points. (Uses x, y and points)
		std::vector<std::tuple<Number, Number>> GetAbsolute() const {
			auto points = this->points;
			for (auto &point : points) {
				std::get<0>(point) += x;
				std::get<1>(point) += y;
			}
			return points;
		}
		
		Drawable(Number x, Number y, Number width, Number height, bool filled):
			type(OVAL), x(x), y(y), width(width), height(height), filled(filled) {}

		Drawable(Number x, Number y, String text):
			type(TEXT), x(x), y(y), text(text), boundText(false) {}

		Drawable(Number x, Number y, String text, Number width):
			type(TEXT), x(x), y(y), text(text), width(width), boundText(true) {}
	};
}

#endif