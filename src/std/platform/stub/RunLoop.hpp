#ifndef SMALLBASIC_PLATFORM_STUB_RUNLOOP_HPP
#define SMALLBASIC_PLATFORM_STUB_RUNLOOP_HPP

#include "../RunLoop.hpp"

namespace SmallBasic {
	namespace Platform {
		void RunLoop::_Initialize() {
		}

		void RunLoop::_Activate() {
		}

		void RunLoop::_Run() {
		}

		void RunLoop::_Stop() {
		}

		void RunLoop::_RunFor(Number milliseconds) {
		}
	}
}

#endif