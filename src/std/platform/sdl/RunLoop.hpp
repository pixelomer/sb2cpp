#ifndef SMALLBASIC_PLATFORM_SDL_RUNLOOP_HPP
#define SMALLBASIC_PLATFORM_SDL_RUNLOOP_HPP

#include <SDL2/SDL.h>
#include "../RunLoop.hpp"
#include "../Window.hpp"
#include "../../common/StringUtils.hpp"

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
				std::wcerr << "SDL_Init() failed: " << SDL_GetError() << std::endl;
				exit(1);
			}
			if (TTF_Init() != 0) {
				std::wcerr << "TTF_Init() failed: " << TTF_GetError() << std::endl;
				exit(1);
			}
		}

		void RunLoop::_Activate() {
		}

		void RunLoop::_Run() {
			bool stop = false;
			SDL_Event event;
			while (!stop) {
				Window::Default()->RedrawIfNeeded();
				
				while (SDL_PollEvent(&event) != 0) {
					switch (event.type) {
						case SDL_QUIT:
							TTF_Quit();
							SDL_Quit();
							exit(0);
							break;
						case SDL_KEYDOWN:
						case SDL_KEYUP: {
							String keyName = StringToWString(SDL_GetKeyName(event.key.keysym.sym));
							if (keyName[0] >= L'0' && keyName[0] <= L'9') {
								keyName = L"D" + keyName;
							}
							EventQueue::Default()->PostKeyEvent(event.type == SDL_KEYDOWN,
								keyName);
							break;
						}
						case SDL_MOUSEBUTTONDOWN:
						case SDL_MOUSEBUTTONUP:
							if (event.button.button != SDL_BUTTON_LEFT) break;
							EventQueue::Default()->PostMouseEvent(event.type ==
								SDL_MOUSEBUTTONDOWN);
							break;
						case SDL_MOUSEMOTION:
							EventQueue::Default()->PostMouseEvent(event.motion.x,
								event.motion.y);
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