#ifndef SMALLBASIC_PLATFORM_SDL_RUNLOOP_MM
#define SMALLBASIC_PLATFORM_SDL_RUNLOOP_MM

#include <SDL2/SDL.h>
#include "../RunLoop.hpp"

namespace SmallBasic {
	namespace Platform {
		Uint32 RunLoop::SDLTimerCallback(Uint32 interval, void *param) {
			RunLoop *runLoop = (RunLoop *)param;
			runLoop->Stop();
			SDL_RemoveTimer(runLoop->_timer);
			runLoop->_timer = 0;
			return 0;
		}

		void RunLoop::_Initialize() {
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
				std::wcout << "sdl init" << std::endl;
				std::wcerr << "SDL_Init() failed: " << SDL_GetError() << std::endl;
				exit(1);
			}
		}

		void RunLoop::_Activate() {
		}

		void RunLoop::_Run() {
			bool stop = false;
			SDL_Event event;
			while (!stop) {
				while (SDL_PollEvent(&event) != 0) {
					switch (event.type) {
						case SDL_QUIT:
							SDL_Quit();
							exit(0);
							break;
						case SDL_USEREVENT:
							stop = true;
							break;
					}
				}
			}
		}

		void RunLoop::_Stop() {
			SDL_Event event;
			SDL_memset(&event, 0, sizeof(event));
			event.type = SDL_USEREVENT;
			SDL_PushEvent(&event);
		}

		void RunLoop::_RunFor(Number milliseconds) {
			if (_timer != 0) {
				SDL_RemoveTimer(_timer);
			}
			_timer = SDL_AddTimer((Uint32)milliseconds, &SDLTimerCallback,
				(void *)this);
			if (_timer == 0) {
				std::wcerr << "Failed to create timer: " << SDL_GetError() << std::endl;
				exit(1);
			}
			_Run();
		}
	}
}

#endif