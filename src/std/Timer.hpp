#ifndef _SMALLBASIC_TIMER_H
#define _SMALLBASIC_TIMER_H

#include "Mixed.hpp"
#include "Platform.hpp"

namespace SmallBasic {
	class Timer {
	public:
		static void _SetTick(void(*onTick)()) {
			Platform::Default()->onTimerTick = onTick;
		}
		static Mixed _GetInterval() {
			return Platform::Default()->GetTimerInterval();
		}
		static void _SetInterval(Number milliseconds) {
			Platform::Default()->SetTimerInterval(milliseconds);
		}
		static void Pause() {
			Platform::Default()->PauseTimer();
		}
		static void Resume() {
			Platform::Default()->ResumeTimer();
		}
	};
}

#endif