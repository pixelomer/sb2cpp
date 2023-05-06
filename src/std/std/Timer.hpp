#ifndef _SMALLBASIC_TIMER_H
#define _SMALLBASIC_TIMER_H

#include "../common/Mixed.hpp"
#include "../common/Updatable.hpp"

namespace SmallBasic {
	namespace Platform {
		class RunLoop;
	}
	namespace Std {
		class Timer {
		private:
			friend class Platform::RunLoop;
			static Updatable<bool> _active;
			static Updatable<Number> _interval;
			static void (*_onTick)();
		public:
			static void _SetTick(void(*onTick)()) {
				_onTick = onTick;
			}
			static Mixed _GetInterval() {
				return _interval.get();
			}
			static void _SetInterval(Number milliseconds) {
				_interval = milliseconds;
			}
			static void Pause() {
				_active = false;
			}
			static void Resume() {
				_active = true;
			}
		};

		Updatable<bool> Timer::_active = false;
		Updatable<Number> Timer::_interval = 1000;
		void (*Timer::_onTick)() = NULL;
	}
}

#endif