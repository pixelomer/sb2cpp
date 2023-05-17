#ifndef SMALLBASIC_PLATFORM_MACOS_SMALLBASICWINDOW_MM
#define SMALLBASIC_PLATFORM_MACOS_SMALLBASICWINDOW_MM

#include "SmallBasicView.mm"

@interface SmallBasicWindow : NSWindow
@end

@implementation SmallBasicWindow {
	SmallBasic::Platform::Window *_windowContext;
	id _keyMonitor;
	NSMutableSet<NSString *> *_keysDown;
}

- (instancetype)initWithFrame:(NSRect)frame
	windowContext:(SmallBasic::Platform::Window *)windowContext
{
	self = [super initWithContentRect:frame styleMask:(NSWindowStyleMaskTitled |
		NSWindowStyleMaskClosable) backing:NSBackingStoreBuffered defer:YES];
	if (self != nil) {
		_windowContext = windowContext;
		_keysDown = [NSMutableSet new];
		self.contentView = [[SmallBasicView alloc] initWithFrame:frame
			renderer:&windowContext->renderer];
		self.contentView.wantsLayer = YES;
		[self makeKeyAndOrderFront:nil];
		[self makeFirstResponder:self.contentView];
		NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect |  
			NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
		NSTrackingArea *trackingArea = [[NSTrackingArea alloc]
			initWithRect:[self.contentView bounds] options:options owner:self
			userInfo:nil];
		[self.contentView addTrackingArea:trackingArea];

		_keyMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown |
			NSEventMaskKeyUp handler:^(NSEvent *event) {
				NSString *key = [event.characters substringWithRange:NSMakeRange(0, 1)];
				switch (event.keyCode) {
					case 123: key = @"Left"; break;
					case 124: key = @"Right"; break;
					case 125: key = @"Down"; break;
					case 126: key = @"Up"; break;
				}
				BOOL keyDownEvent = (event.type == NSEventTypeKeyDown);
				if (keyDownEvent && ![_keysDown containsObject:key]) {
					_windowContext->eventQueue->PostKeyEvent(true, NSStringToWString(key));
					[_keysDown addObject:key];
				}
				else if (!keyDownEvent && [_keysDown containsObject:key]) {
					_windowContext->eventQueue->PostKeyEvent(false, NSStringToWString(key));
					[_keysDown removeObject:key];
				}
				return event;
			}
		];
	}
	return self;
}

- (void)mouseMoved:(NSEvent *)event {
	if (_windowContext != nullptr) {
		NSPoint point = [event locationInWindow];
		point = [self.contentView convertPoint:point fromView:nil];
		_windowContext->eventQueue->PostMouseEvent(point.x, point.y);
	}
	[super mouseMoved:event];
}

- (void)mouseDown:(NSEvent *)event {
	if (_windowContext != nullptr) {
		_windowContext->eventQueue->PostMouseEvent(true);
	}
	[super mouseDown:event];
}

- (void)mouseUp:(NSEvent *)event {
	if (_windowContext != nullptr) {
		_windowContext->eventQueue->PostMouseEvent(false);
	}
	[super mouseUp:event];
}

- (BOOL)performKeyEquivalent:(NSEvent *)event {
	return [super performKeyEquivalent:event] || true;
}

- (void)disconnect {
	_windowContext = nullptr;
	[(SmallBasicView *)self.contentView disconnect];
}

@end

#endif