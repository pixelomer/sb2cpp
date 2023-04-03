#ifndef _SMALLBASIC_RUNLOOP_H
#define _SMALLBASIC_RUNLOOP_H

#include "Types.hpp"

namespace SmallBasic {
	class RunLoop {
	private:
		static void (*_runner)();
	public:
		static void _PrepareRunLoop(void(*runner)()) {
			_runner = runner;
		}
		static void _Run() {
			if (_runner != NULL) {
				_runner();
			}
		}
	};

	void (*RunLoop::_runner)() = NULL;
}

#endif