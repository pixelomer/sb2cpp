#ifndef SMALLBASIC_PLATFORM_RUNLOOP_MM
#define SMALLBASIC_PLATFORM_RUNLOOP_MM

#include <Cocoa/Cocoa.h>

namespace SmallBasic {
	namespace Platform {
		void RunLoop::_Initialize() {
			NSApplication *app = [NSApplication sharedApplication];
			[app setActivationPolicy:NSApplicationActivationPolicyRegular];
			[app activateIgnoringOtherApps:YES];
		}

		void RunLoop::_Run() {
			@autoreleasepool {
				NSApplication *app = [NSApplication sharedApplication];
				[app run];
			}
		}

		void RunLoop::_Stop() {
			NSApplication *app = [NSApplication sharedApplication];
			[app stop:nil];
			// -[NSApplication stop:] only takes effect after an event is
			// processed
			NSEvent *event = [NSEvent otherEventWithType:NSApplicationDefined
				location:{} modifierFlags:0 timestamp:0 windowNumber:0
				context:NULL subtype:0 data1:0 data2:0];
			[app postEvent:event atStart:YES];
		}

		void RunLoop::_RunFor(Number milliseconds) {
			@autoreleasepool {
				NSApplication *app = [NSApplication sharedApplication];
				NSTimer *__unused timer = [NSTimer
					scheduledTimerWithTimeInterval:(milliseconds / 1000.)
					repeats:NO block:^(NSTimer *timer){
						_Stop();
					}];
				[app run];
			}
		}
	}
}

#endif