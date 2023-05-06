#ifndef SMALLBASIC_PLATFORM_TIMER_HPP
#define SMALLBASIC_PLATFORM_TIMER_HPP

#include "../common/Mixed.hpp"
#include "../common/Updatable.hpp"
#include "Event.hpp"
#include <queue>

namespace SmallBasic {
	namespace Platform {
		class RunLoop;

		class Timer {
		private:
			friend class RunLoop;
			static Timer *_default;
		public:
			static Timer *Default() {
				if (_default == nullptr) {
					_default = new Timer;
				}
				return _default;
			}
		};

		Timer *Timer::_default = nullptr;
	}
}

#endif