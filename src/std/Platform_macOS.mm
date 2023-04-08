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
		CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext]
			graphicsPort];
		CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image),
			CGImageGetHeight(image)), image);
		CGImageRelease(image);
	}
}

@end

namespace SmallBasic {
	static CGColorSpaceRef _colorSpace = NULL;

	static void ReallocateContext(CGContextRef *context, Number ldWidth, Number ldHeight) {
		size_t width = (size_t)std::ceill(ldWidth);
		size_t height = (size_t)std::ceill(ldHeight);
		if (*context != NULL) {
			size_t currentWidth = CGBitmapContextGetHeight(*context);
			size_t currentHeight = CGBitmapContextGetWidth(*context);
			if (width <= currentWidth && height <= currentHeight) {
				return;
			}
		}
		CGContextRef newContext = CGBitmapContextCreate(NULL, width, height, 8, 0,
			_colorSpace, kCGImageAlphaPremultipliedLast);
		if (*context != NULL) {
			CGImageRef oldData = CGBitmapContextCreateImage(*context);
			CGContextDrawImage(newContext, CGRectMake(0, 0, CGImageGetWidth(oldData),
				CGImageGetHeight(oldData)), oldData);
			CGImageRelease(oldData);
			CGContextRelease(*context);
			//FIXME: CGContext properties not copied
		}
		*context = newContext;
	}

	static void EnsureContext(CGContextRef *context) {
		ReallocateContext(context, INITIAL_WIDTH, INITIAL_HEIGHT);
	}

	void Platform::_Initialize() {
		if (_colorSpace == NULL) {
			_colorSpace = CGColorSpaceCreateDeviceRGB();
		}
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
			int mask = NSTitledWindowMask | NSClosableWindowMask;
			_window = [[NSWindow alloc] initWithContentRect:frame
				styleMask:mask backing:NSBackingStoreBuffered defer:YES];
			_window.contentView = [[SmallBasicView alloc] initWithFrame:frame
				context:&_drawContext];
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

	void Platform::SetFillColor(Color const& color) {
		EnsureContext(&_drawContext);
		CGFloat comps[] = { (CGFloat)color.r / 255.0, (CGFloat)color.g / 255.0,
			(CGFloat)color.b / 255.0, 1.0 };
		CGColorRef cgColor = CGColorCreate(_colorSpace, comps);
		CGContextSetFillColorWithColor(_drawContext, cgColor);
		CGColorRelease(cgColor);
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
		EnsureContext(&_drawContext);
		CGRect rect = CGRectMake(x, y, width, height);
		if (fill) {
			CGContextFillRect(_drawContext, rect);
		}
		else {
			CGContextStrokeRect(_drawContext, rect);
		}
		_GetWindow().contentView.needsDisplay = YES;
	}

	void Platform::SetStrokeWidth(Number strokeWidth) {
		EnsureContext(&_drawContext);
		CGContextSetLineWidth(_drawContext, strokeWidth);
	}
}

#undef INITIAL_WIDTH
#undef INITIAL_HEIGHT

#endif