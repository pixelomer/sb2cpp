#ifndef SMALLBASIC_PLATFORM_RUNLOOP_HPP
#define SMALLBASIC_PLATFORM_RUNLOOP_HPP

#include "config.hpp"
#include "../common/Mixed.hpp"
#include "../std/GraphicsWindow.hpp"
#include "../std/Shapes.hpp"
#include "../std/Timer.hpp"
#include "Window.hpp"
#include "Renderer.hpp"
#include "EventQueue.hpp"
#include "Event.hpp"
#include <queue>
#include <chrono>

#if defined(SMALLBASIC_SDL)
#include <SDL.h>
#endif

namespace SmallBasic {
	namespace Platform {
		class RunLoop {
		private:
			static RunLoop *_default;
			bool _shouldActivate = false;

#if defined(SMALLBASIC_SDL)
			SDL_TimerID _timer = 0;
			static Uint32 SDLTimerCallback(Uint32 interval, void *param);
#endif

			void _Activate();
			void _RunFor(Number milliseconds);
			void _Run();
			void _Stop();
			void _Initialize();

			void _PrepareRun() {
				EventQueue::Default()->onEvent = std::bind(&RunLoop::Stop, this);
			}
			void _EndRun() {
				EventQueue::Default()->onEvent = nullptr;
			}
		public:
			void Stop() { _Stop(); }

			bool WillTerminate() {
				return (!Std::Timer::_active || Std::Timer::_onTick == NULL) &&
					!Std::GraphicsWindow::_visible.get() &&
					EventQueue::Default()->QueueSize() <= 0;
			}

			void Run(void (*mainFunction)()) {
				mainFunction();

				while (!WillTerminate()) {
					Update();
					if (!WillTerminate()) {
						_PrepareRun();
						_Run();
						_EndRun();
					}
				}
			}

			Number RunForAtLeast(Number milliseconds) {
				if (_shouldActivate) {
					_Activate();
					_shouldActivate = false;
				}

				auto start = std::chrono::high_resolution_clock::now();
				
				Update();
				_PrepareRun();
				_RunFor(milliseconds);
				_EndRun();

				auto end = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration<Number, std::milli>(end - start).count();
				return elapsed;
			}

			void RunFor(Number milliseconds) {
				while (milliseconds >= 0) {
					milliseconds -= RunForAtLeast(milliseconds);
				}
			}

			void Update() {
				Window *window = Window::Default();
				Renderer *renderer = window->renderer;

				// Process events
				while (EventQueue::Default()->QueueSize() > 0) {
					Event event = EventQueue::Default()->Next();
					switch (event.type) {
						case Event::KEY_DOWN:
							Std::GraphicsWindow::_lastKey = event.key;
							if (Std::GraphicsWindow::_keyDown != NULL) {
								Std::GraphicsWindow::_keyDown();
							}
							break;
						case Event::KEY_UP:
							Std::GraphicsWindow::_lastKey = event.key;
							if (Std::GraphicsWindow::_keyUp != NULL) {
								Std::GraphicsWindow::_keyUp();
							}
							break;
						case Event::MOUSE_DOWN:
							if (Std::GraphicsWindow::_mouseDown != NULL) {
								Std::GraphicsWindow::_mouseDown();
							}
							break;
						case Event::MOUSE_UP:
							break;
						case Event::MOUSE_MOVE:
							Std::GraphicsWindow::_mouse.x = event.x;
							Std::GraphicsWindow::_mouse.y = event.y;
							if (Std::GraphicsWindow::_mouseMove != NULL) {
								Std::GraphicsWindow::_mouseMove();
							}
							break;
						case Event::TIMER_TICK:
							if (Std::Timer::_onTick != NULL) {
								Std::Timer::_onTick();
							}
							break;
					}
				}

				if (Std::Shapes::_shapes.changed) {
					renderer->Clear(Renderer::SHAPE_LAYER);
					for (auto &pair : Std::Shapes::_shapes.use()) {
						auto &drawable = pair.second;
						renderer->Draw(Renderer::SHAPE_LAYER, drawable);
					}
				}

				if (Std::GraphicsWindow::_title.changed) {
					Window::Default()->SetTitle(Std::GraphicsWindow::_title.use());
				}

				if (Std::GraphicsWindow::_visible.changed) {
					bool visible = Std::GraphicsWindow::_visible.use();
					Window::Default()->SetVisible(visible);
					if (visible) {
						_shouldActivate = true;
					}
				}

				if (Std::GraphicsWindow::_backgroundColor.changed) {
					renderer->SetBackgroundColor(Std::GraphicsWindow::
						_backgroundColor.use());
				}

				if (Std::GraphicsWindow::_canResize.changed) {
					Window::Default()->SetResizable(Std::GraphicsWindow::_canResize.use());
				}

				if (Std::GraphicsWindow::_rect.changed) {
					Window::Default()->SetRect(Std::GraphicsWindow::_rect.use());
				}

				Window::Default()->RedrawIfNeeded();
			}

			static RunLoop *Default() {
				if (_default == nullptr) {
					_default = new RunLoop;
				}
				return _default;
			}

			RunLoop() {
				_Initialize();
			}
		};

		RunLoop *RunLoop::_default = nullptr;
	}
}

#if defined(SMALLBASIC_APPLE)
#include "macos/RunLoop.mm"
#elif defined(SMALLBASIC_SDL)
#include "sdl/RunLoop.hpp"
#endif

#endif