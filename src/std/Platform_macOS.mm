#ifndef _SMALLBASIC_PLATFORM_MACOS_MM
#define _SMALLBASIC_PLATFORM_MACOS_MM

#include "Platform.hpp"
#include "Mixed.hpp"
#include <AppKit/AppKit.h>
#include <cmath>

#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480

@interface SmallBasicView : NSView
- (instancetype)initWithFrame:(NSRect)frame context:(CGContextRef *)context;
@end

@implementation SmallBasicView {
	CGContextRef *_context;
}

- (instancetype)initWithFrame:(NSRect)frame context:(CGContextRef *)context {
	if ((self = [super initWithFrame:frame])) {
		_context = context;
	}
	return self;
}

- (BOOL)isFlipped { return YES; }

- (void)drawRect:(NSRect)dirtyRect {
	if (*_context != NULL) {
		CGImageRef image = CGBitmapContextCreateImage(*_context);
		CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
		CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image),
			CGImageGetHeight(image)), image);
		CGImageRelease(image);
	}
}

@end

namespace SmallBasic {
	void Platform::_ReallocateContext(Number ldWidth, Number ldHeight) {
		size_t width = (size_t)std::ceill(ldWidth);
		size_t height = (size_t)std::ceill(ldHeight);
		if (_context != NULL) {
			size_t currentWidth = CGBitmapContextGetHeight(_context);
			size_t currentHeight = CGBitmapContextGetWidth(_context);
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
			CGContextSetFillColorWithColor(_context, _fillColor);
			CGContextSetStrokeColorWithColor(_context, _strokeColor);
			CGContextSetLineWidth(_context, _strokeWidth);
		}
		_context = newContext;
	}

	void Platform::_EnsureContext() {
		_ReallocateContext(INITIAL_WIDTH, INITIAL_HEIGHT);
	}

	void Platform::_Initialize() {
		_colorSpace = CGColorSpaceCreateDeviceRGB();
		
	}

	void Platform::SetTitle(String const& title) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		std::string str = converter.to_bytes(title);
		NSString *nsstr = @(str.c_str());
		_GetWindow().title = nsstr;
	}

	NSWindow *Platform::_GetWindow() {
		if (_window == nil) {
			NSApplication *app = [NSApplication sharedApplication];
			[app setActivationPolicy:NSApplicationActivationPolicyRegular];

			NSRect frame = NSMakeRect(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);
			_window = [[NSWindow alloc] initWithContentRect:frame
				styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable)
				backing:NSBackingStoreBuffered defer:YES];
			_window.contentView = [[SmallBasicView alloc] initWithFrame:frame
				context:&_context];
			//[_window cascadeTopLeftFromPoint:NSMakePoint(10, 10)];
			_window.contentView.wantsLayer = YES;
			_window.title = @"SmallBasic";
			[_window makeKeyAndOrderFront:nil];

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

	void Platform::_SetColor(Color const& color, void(*setter)(CGContextRef, CGColorRef),
		CGColorRef *storedColor)
	{
		_EnsureContext();
		CGFloat comps[] = { (CGFloat)color.r / 255.0, (CGFloat)color.g / 255.0,
			(CGFloat)color.b / 255.0, 1.0 };
		CGColorRef cgColor = CGColorCreate(_colorSpace, comps);
		if (*storedColor != NULL) {
			CGColorRelease(*storedColor);
		}
		*storedColor = cgColor;
		setter(_context, cgColor);
	}	

	void Platform::SetFillColor(Color const& color) {
		_SetColor(color, CGContextSetFillColorWithColor, &_fillColor);
	}

	void Platform::SetStrokeColor(Color const& color) {
		_SetColor(color, CGContextSetStrokeColorWithColor, &_strokeColor);
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

	void Platform::SetBackgroundColor(Color const& color) {
		_GetWindow().contentView.layer.backgroundColor = [[NSColor colorWithCalibratedRed:color.r
			green:color.g blue:color.b alpha:1.0] CGColor];
	}

	void Platform::DrawRectangle(Number x, Number y, Number width, Number height, bool fill) {
		_EnsureContext();
		CGRect rect = CGRectMake(x, y, width, height);
		if (fill) {
			CGContextFillRect(_context, rect);
		}
		else {
			CGContextStrokeRect(_context, rect);
		}
		_GetWindow().contentView.needsDisplay = YES;
	}
}

#undef INITIAL_WIDTH
#undef INITIAL_HEIGHT

#endif