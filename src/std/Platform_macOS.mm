#ifndef _SMALLBASIC_PLATFORM_MACOS_MM
#define _SMALLBASIC_PLATFORM_MACOS_MM

#include "Platform.hpp"
#include "Mixed.hpp"
#include <AppKit/AppKit.h>
#include <cmath>

#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480

std::wstring NSStringToWString(NSString *str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8 = [str UTF8String];
	return converter.from_bytes(utf8);
}

@interface SmallBasicView : NSView
@end

@implementation SmallBasicView {
	CGContextRef *_context;
	SmallBasic::Platform *_platform;
}

- (instancetype)initWithFrame:(NSRect)frame context:(CGContextRef *)context {
	if ((self = [super initWithFrame:frame])) {
		_context = context;
	}
	return self;
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (BOOL)isFlipped { return YES; }

- (void)drawRect:(NSRect)dirtyRect {
	if (_context != NULL && *_context != NULL) {
		CGImageRef image = CGBitmapContextCreateImage(*_context);
		CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
		CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image),
			CGImageGetHeight(image)), image);
		CGImageRelease(image);
	}
}

- (void)disconnect {
	_context = NULL;
}

@end

@interface SmallBasicWindow : NSWindow
@end

@implementation SmallBasicWindow {
	SmallBasic::Platform *_platform;
}

- (instancetype)initWithFrame:(NSRect)frame platform:(SmallBasic::Platform *)platform
	context:(CGContextRef *)context
{
	self = [super initWithContentRect:frame styleMask:(NSWindowStyleMaskTitled |
		NSWindowStyleMaskClosable) backing:NSBackingStoreBuffered defer:YES];
	if (self != nil) {
		_platform = platform;
		self.contentView = [[SmallBasicView alloc] initWithFrame:frame context:context];
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
	}
	return self;
}

- (void)mouseMoved:(NSEvent *)event {
	if (_platform != nullptr && _platform->onMouseMove != NULL) {
		NSPoint point = [NSEvent mouseLocation];
		_platform->onMouseMove(point.x, point.y);
	}
	[super mouseMoved:event];
}

- (void)mouseDown:(NSEvent *)event {
	if (_platform != nullptr && _platform->onMouseDown != NULL) {
		_platform->onMouseDown();
	}
	[super mouseDown:event];
}

- (void)mouseUp:(NSEvent *)event {
	if (_platform != nullptr && _platform->onMouseUp != NULL) {
		_platform->onMouseUp();
	}
	[super mouseUp:event];
}

- (void)keyDown:(NSEvent *)event {
	if (_platform != nullptr && !_platform->ignoresKeyEvents &&
		_platform->onKeyDown != NULL)
	{
		NSString *key = event.characters;
		_platform->onKeyDown(NSStringToWString(key));
	}
	[super keyDown:event];
}

- (void)keyUp:(NSEvent *)event {
	if (_platform != nullptr && !_platform->ignoresKeyEvents &&
		_platform->onMouseUp != NULL)
	{
		NSString *key = event.characters;
		_platform->onKeyUp(NSStringToWString(key));
	}
	[super keyUp:event];
}

- (BOOL)performKeyEquivalent:(NSEvent *)event {
	return [super performKeyEquivalent:event] || !_platform->ignoresKeyEvents;
}

- (void)disconnect {
	_platform = nullptr;
	[(SmallBasicView *)self.contentView disconnect];
}

@end

namespace SmallBasic {
	Platform::Platform() {
		_colorSpace = CGColorSpaceCreateDeviceRGB();
	}

	Platform::~Platform() {
		if (_window != nil) {
			[_window disconnect];
			_window = nil;
		}
		if (_context != NULL) {
			CGContextRelease(_context);
			_context = NULL;
		}
	}

	void Platform::_ReallocateContext(Number ldWidth, Number ldHeight) {
		size_t width = (size_t)std::ceill(ldWidth);
		size_t height = (size_t)std::ceill(ldHeight);
		if (_context != NULL) {
			size_t currentWidth = CGBitmapContextGetWidth(_context);
			size_t currentHeight = CGBitmapContextGetHeight(_context);
			if (width <= currentWidth && height <= currentHeight) {
				return;
			}
		}
		CGContextRef newContext = CGBitmapContextCreate(NULL, width, height, 8, 0,
			_colorSpace, kCGImageAlphaPremultipliedLast);
		if (_context != NULL) {
			// Copy old contents to the new context
			CGImageRef oldData = CGBitmapContextCreateImage(_context);
			CGContextDrawImage(newContext, CGRectMake(0, 0, CGImageGetWidth(oldData),
				CGImageGetHeight(oldData)), oldData);
			CGImageRelease(oldData);

			// Release old context
			CGContextRelease(_context);

			// Set properties on new context
			CGContextSetLineWidth(newContext, _strokeWidth);
		}
		_context = newContext;
	}

	void Platform::_PrepareFill() {
		CGContextSetFillColorWithColor(_context, _fillColor);
	}

	void Platform::_PrepareStroke() {
		CGContextSetStrokeColorWithColor(_context, _strokeColor);
	}

	void Platform::_EnsureContext() {
		_ReallocateContext(INITIAL_WIDTH, INITIAL_HEIGHT);
	}

	void Platform::SetTitle(String const& title) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		std::string str = converter.to_bytes(title);
		NSString *nsstr = @(str.c_str());
		_GetWindow().title = nsstr;
	}

	SmallBasicWindow *Platform::_GetWindow() {
		if (_window == nil) {
			NSApplication *app = [NSApplication sharedApplication];
			[app setActivationPolicy:NSApplicationActivationPolicyRegular];

			NSRect frame = NSMakeRect(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);
			_window = [[SmallBasicWindow alloc] initWithFrame:frame
				platform:this context:&_context];

			[[NSNotificationCenter defaultCenter] addObserver:app
				selector:@selector(terminate:) name:NSWindowWillCloseNotification
				object:_window];

			[app activateIgnoringOtherApps:YES];
		}
		return _window;
	}

	void Platform::Run() {
		@autoreleasepool {
			NSApplication *app = [NSApplication sharedApplication];
			[app run];
		}
	}

	void Platform::RunFor(Number milliseconds) {
		@autoreleasepool {
			NSApplication *__block app = [NSApplication sharedApplication];
			NSTimer *__unused timer = [NSTimer
				scheduledTimerWithTimeInterval:(milliseconds / 1000.)
				repeats:NO block:^(NSTimer *timer){
					[app stop:nil];
					// -[NSApplication stop:] only takes effect after an event is
					// processed
					NSEvent *event = [NSEvent
						otherEventWithType:NSEventTypeApplicationDefined
						location:{} modifierFlags:0 timestamp:0 windowNumber:0
						context:NULL subtype:0 data1:0 data2:0];
					[app postEvent:event atStart:YES];
				}];
			[app run];
		}
	}

	void Platform::_SetColor(Color const& color, CGColorRef *storedColor) {
		_EnsureContext();
		CGColorRef cgColor = CGColorCreateGenericRGB((CGFloat)color.r / 255.f,
			(CGFloat)color.g / 255.f, (CGFloat)color.b / 255.f, 1.0);
		if (*storedColor != NULL) {
			CGColorRelease(*storedColor);
		}
		*storedColor = cgColor;
	}	

	void Platform::SetFillColor(Color const& color) {
		_SetColor(color, &_fillColor);
	}

	void Platform::SetStrokeColor(Color const& color) {
		_SetColor(color, &_strokeColor);
	}

	void Platform::SetStrokeWidth(Number strokeWidth) {
		_EnsureContext();
		_strokeWidth = (CGFloat)strokeWidth;
		CGContextSetLineWidth(_context, _strokeWidth);
	}
	
	void Platform::SetWindowVisible(bool visible) {
		// has no effect
		_GetWindow();
	}

	void Platform::SetCanResize(bool canResize) {
		if (canResize) {
			_GetWindow().styleMask |= NSWindowStyleMaskResizable;
		}
		else {
			_GetWindow().styleMask &= ~NSWindowStyleMaskResizable;
		}
	}

	void Platform::SetBackgroundColor(Color const& color) {
		_GetWindow().contentView.layer.backgroundColor = [[NSColor colorWithCalibratedRed:color.r
			green:color.g blue:color.b alpha:1.0] CGColor];
	}

	void Platform::DrawRectangle(Number x, Number y, Number width, Number height, bool fill) {
		_EnsureContext();
		CGRect rect = CGRectMake(x, y, width, height);
		if (fill) {
			_PrepareFill();
			CGContextFillRect(_context, rect);
		}
		else {
			_PrepareStroke();
			CGContextStrokeRect(_context, rect);
		}
		_GetWindow().contentView.needsDisplay = YES;
	}

	Color Platform::GetPixel(Number x, Number y) {
		size_t height = CGBitmapContextGetHeight(_context);
		size_t width = CGBitmapContextGetWidth(_context);
		if ((size_t)x >= width || (size_t)y >= height) {
			return Color(L"white");
		}
		size_t bytesPerRow = CGBitmapContextGetBytesPerRow(_context);
		uint8_t *imageData = (uint8_t *)CGBitmapContextGetData(_context);
		imageData += bytesPerRow * (width * (size_t)y + (size_t)x);
		return Color(imageData[0], imageData[1], imageData[2]);
	}

	void Platform::ClearWindow() {
		if (_context != NULL) {
			CGContextClearRect(_context, CGRectMake(0, 0, CGBitmapContextGetWidth(_context),
				CGBitmapContextGetHeight(_context)));
		}
	}

	void Platform::DrawEllipse(Number x, Number y, Number width, Number height, bool fill) {
		_EnsureContext();
		CGRect rect = CGRectMake(x, y, width, height);
		if (fill) {
			_PrepareFill();
			CGContextFillEllipseInRect(_context, rect);
		}
		else {
			_PrepareStroke();
			CGContextStrokeEllipseInRect(_context, rect);
		}
		_GetWindow().contentView.needsDisplay = YES;
	}

	void Platform::DrawTriangle(Number x1, Number y1, Number x2, Number y2,
		Number x3, Number y3, bool fill)
	{
		CGPoint lines[3] = { CGPointMake(x1, y1), CGPointMake(x2, y2),
			CGPointMake(x3, y3) };
		CGContextAddLines(_context, lines, 3);
		CGContextClosePath(_context);
		if (fill) {
			_PrepareFill();
			CGContextFillPath(_context);
		}
		else {
			_PrepareStroke();
			CGContextStrokePath(_context);
		}
		_GetWindow().contentView.needsDisplay = YES;
	}

	void Platform::SetHeight(Number height) {
		NSRect frame = _GetWindow().frame;
		frame.size.height = (CGFloat)height;
		[_GetWindow() setFrame:frame display:YES animate:NO];
	}

	Number Platform::GetHeight() {
		return _GetWindow().frame.size.height;
	}

	void Platform::SetWidth(Number width) {
		NSRect frame = _GetWindow().frame;
		frame.size.width = (CGFloat)width;
		[_GetWindow() setFrame:frame display:YES animate:NO];
	}

	Number Platform::GetWidth() {
		return _GetWindow().frame.size.width;
	}
}

#undef INITIAL_WIDTH
#undef INITIAL_HEIGHT

#endif