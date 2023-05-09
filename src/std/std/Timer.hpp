#ifndef _SMALLBASIC_TIMER_H
#define _SMALLBASIC_TIMER_H

#include "../common/Mixed.hpp"
#include "../common/Updatable.hpp"
#include <chrono>
#include <cmath>

namespace SmallBasic {
	namespace Platform {
		class RunLoop;
	}
	namespace Std {
		class Timer {
		private:
			friend class Platform::RunLoop;
			static bool _active;
			static Updatable<Number> _interval;
			static std::chrono::time_point<std::chrono::steady_clock,
				std::chrono::steady_clock::duration> _timerStart;
			static void (*_onTick)();
		public:
			static void _SetTick(void(*onTick)()) {
				_onTick = onTick;
			}
			static Mixed _GetInterval() {
				return _interval.get();
			}
			static void _SetInterval(Number milliseconds) {
				_timerStart = std::chrono::high_resolution_clock::now();
				_interval = milliseconds;
			}
			static void Pause() {
				_active = false;
			}
			static void Resume() {
				_timerStart = std::chrono::high_resolution_clock::now();
				_active = true;
			}
		};

		bool Timer::_active = true;
		Updatable<Number> Timer::_interval = 1000;
		std::chrono::time_point<std::chrono::steady_clock,
			std::chrono::steady_clock::duration> Timer::_timerStart = {};
		void (*Timer::_onTick)() = NULL;
	}
}

#endif