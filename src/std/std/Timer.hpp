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
			static bool _active;
			static void (*_onTick)();
		public:
			static void _SetTick(void(*onTick)()) {
				_onTick = onTick;
			}
			static Mixed _GetInterval() {
				return 1000.L;
			}
			static void _SetInterval(Number milliseconds) {
			}
			static void Pause() {
			}
			static void Resume() {
			}
		};

		bool Timer::_active = false;
		void (*Timer::_onTick)() = NULL;
	}
}

#endif