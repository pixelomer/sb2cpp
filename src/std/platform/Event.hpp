#ifndef SMALLBASIC_PLATFORM_EVENT_HPP
#define SMALLBASIC_PLATFORM_EVENT_HPP

#include "config.hpp"
#include "../common/Mixed.hpp"

namespace SmallBasic {
	namespace Platform {
		class Event {
		public:
			enum EventType {
				MOUSE_DOWN,
				MOUSE_UP,
				KEY_DOWN,
				KEY_UP,
				MOUSE_MOVE,
				TIMER_TICK
			};
			const EventType type;
			const String key = {};
			const Number x = 0;
			const Number y = 0;
			Event(EventType type, Number x, Number y): type(type), x(x), y(y) {}
			Event(EventType type, String key): type(type), key(key) {}
			Event(EventType type): type(type) {}
		};
	}
}

#endif