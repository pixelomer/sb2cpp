#ifndef SMALLBASIC_PLATFORM_MACOS_SMALLBASICWINDOW_MM
#define SMALLBASIC_PLATFORM_MACOS_SMALLBASICWINDOW_MM

#include "SmallBasicView.mm"

@interface SmallBasicWindow : NSWindow
@end

@implementation SmallBasicWindow {
	SmallBasic::Platform::Window *_windowContext;
	id _keyMonitor;
}

- (instancetype)initWithFrame:(NSRect)frame
	windowContext:(SmallBasic::Platform::Window *)windowContext
{
	self = [super initWithContentRect:frame styleMask:(NSWindowStyleMaskTitled |
		NSWindowStyleMaskClosable) backing:NSBackingStoreBuffered defer:YES];
	if (self != nil) {
		_windowContext = windowContext;
		self.contentView = [[SmallBasicView alloc] initWithFrame:frame
			renderer:&windowContext->renderer];
		self.contentView.wantsLayer = YES;
		self.title = @"SmallBasic";
		[self makeKeyAndOrderFront:nil];
		[self makeFirstResponder:self.contentView];
		NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect |  
			NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
		NSTrackingArea *trackingArea = [[NSTrackingArea alloc]
			initWithRect:[self.contentView bounds] options:options owner:self
			userInfo:nil];
		[self.contentView addTrackingArea:trackingArea];

		_keyMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSKeyDownMask |
			NSKeyDownMask handler:^(NSEvent *event) {
				NSString *key = [event.characters substringWithRange:NSMakeRange(0, 1)];
				switch (event.keyCode) {
					case 123: key = @"Left"; break;
					case 124: key = @"Right"; break;
					case 125: key = @"Down"; break;
					case 126: key = @"Up"; break;
				}
				_windowContext->eventQueue->PostKeyEvent(event.type == NSEventTypeKeyDown,
					NSStringToWString(key));
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