#ifndef SMALLBASIC_COMMON_POS_H
#define SMALLBASIC_COMMON_POS_H

#include "Mixed.hpp"
#include <stdint.h>

namespace SmallBasic {
	class Pos {
	public:
		Number x=0, y=0;
		Pos(Number x, Number y): x(x), y(y) {}
	};
}

#endif