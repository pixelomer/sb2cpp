#ifndef SMALLBASIC_PLATFORM_EVENTS_HPP
#define SMALLBASIC_PLATFORM_EVENTS_HPP

#include <queue>
#include "Event.hpp"

namespace SmallBasic {
	namespace Platform {
		class EventQueue {
		private:
			static EventQueue *_default;
			std::queue<Event> _queue;
		public:
			std::function<void(EventQueue *)> onEvent;

			size_t QueueSize() {
				return _queue.size();
			}
			
			void PostEvent(Event const& event) {
				_queue.push(event);
				if (onEvent != nullptr) {
					onEvent(this);
				}
			}
			void PostMouseEvent(Number x, Number y) {
				PostEvent(Event(Event::MOUSE_MOVE, x, y));
			}
			void PostMouseEvent(bool down) {
				PostEvent(Event(down ? Event::MOUSE_DOWN : Event::MOUSE_UP));
			}
			void PostKeyEvent(bool down, String const& key) {
				PostEvent(Event(down ? Event::KEY_DOWN : Event::KEY_UP, key));
			}
			void PostTimerEvent() {
				PostEvent(Event(Event::TIMER_TICK));
			}

			Event Next() {
				Event event = _queue.back();
				_queue.pop();
				return event;
			}

			static EventQueue *Default() {
				if (_default == nullptr) {
					_default = new EventQueue;
				}
				return _default;
			}
		};

		EventQueue *EventQueue::_default = nullptr;
	}
}

#endif