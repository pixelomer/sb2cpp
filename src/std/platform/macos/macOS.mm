#ifndef SMALLBASIC_PLATFORM_MACOS_MM
#define SMALLBASIC_PLATFORM_MACOS_MM

#include "Platform.hpp"
#include "common/Mixed.hpp"
#include <AppKit/AppKit.h>
#include <cmath>

#define INITIAL_WIDTH 640
#define INITIAL_HEIGHT 480

std::wstring NSStringToWString(NSString *str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8 = [str UTF8String];
	return converter.from_bytes(utf8);
}

NSString *WStringToNSString(SmallBasic::String const& str) {
	//FIXME: duplicated code
	char buf[MB_CUR_MAX + 1];
	std::string result;
	std::wctomb(NULL, 0);
	for (wchar_t c : str) {
		int len = std::wctomb(buf, c);
		if (len < 0) {
			buf[0] = '?';
			len = 1;
		}
		buf[len] = '\0';
		result += buf;
	}
	return @(result.c_str());
}

namespace SmallBasic {
	Platform::Platform() {
		_colorSpace = CGColorSpaceCreateDeviceRGB();
		fontName = L"Verdana";
	}

	Platform::~Platform() {
		if (_window != nil) {
			[_window disconnect];
			_window = nil;
		}
		if (_backgroundContext != NULL) {
			CGContextRelease(_backgroundContext);
			_backgroundContext = NULL;
		}
		if (_shapeContext != NULL) {
			CGContextRelease(_shapeContext);
			_shapeContext = NULL;
		}
	}

	void Platform::_ReallocateContext(CGContextRef &context, Number ldWidth,
		Number ldHeight)
	{
		size_t width = (size_t)std::ceill(ldWidth);
		size_t height = (size_t)std::ceill(ldHeight);
		if (context != NULL) {
			size_t currentWidth = CGBitmapContextGetWidth(context);
			size_t currentHeight = CGBitmapContextGetHeight(context);
			if (width <= currentWidth && height <= currentHeight) {
				return;
			}
		}
		CGContextRef newContext = CGBitmapContextCreate(NULL, width, height, 8, 0,
			_colorSpace, kCGImageAlphaPremultipliedLast);
		CGContextSetTextMatrix(newContext, CGAffineTransformMake(1.0, 0.0, 0.0,
			-1.0, 0.0, 0.0));
		if (context != NULL) {
			// Copy old contents to the new context
			CGImageRef oldData = CGBitmapContextCreateImage(context);
			CGContextDrawImage(newContext, CGRectMake(0, 0, CGImageGetWidth(oldData),
				CGImageGetHeight(oldData)), oldData);
			CGImageRelease(oldData);

			// Release old context
			CGContextRelease(context);

			// Set properties on new context
			CGContextSetLineWidth(newContext, _strokeWidth);
		}
		context = newContext;
	}

	void Platform::_PrepareFill() {
		CGContextSetFillColorWithColor(_backgroundContext, _fillColor);
	}

	void Platform::_PrepareStroke() {
		CGContextSetStrokeColorWithColor(_backgroundContext, _strokeColor);
	}

	CGContextRef Platform::_EnsureContext(bool shapeLayer) {
		CGContextRef *context;
		if (shapeLayer) {
			context = &_shapeContext;
		}
		else {
			context = &_backgroundContext;
		}
		_ReallocateContext(*context, INITIAL_WIDTH, INITIAL_HEIGHT);
		return *context;
	}

	void Platform::SetTitle(String const& title) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		std::string str = converter.to_bytes(title);
		NSString *nsstr = @(str.c_str());
		_GetWindow().title = nsstr;
	}

	void Platform::_Run() {
		@autoreleasepool {
			[NSApp run];
		}
	}

	void Platform::_Stop() {
		NSApplication *app = [NSApplication sharedApplication];
		[app stop:nil];
		// -[NSApplication stop:] only takes effect after an event is
		// processed
		NSEvent *event = [NSEvent otherEventWithType:NSApplicationDefined
			location:{} modifierFlags:0 timestamp:0 windowNumber:0
			context:NULL subtype:0 data1:0 data2:0];
		[app postEvent:event atStart:YES];
	}

	void Platform::_RunFor(Number milliseconds) {
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
		CGContextRef context = _EnsureContext();
		_strokeWidth = (CGFloat)strokeWidth;
		CGContextSetLineWidth(context, _strokeWidth);
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

	bool Platform::IsWindowVisible() {
		return _window != nil;
	}

	void Platform::SetBackgroundColor(Color const& color) {
		_GetWindow().contentView.layer.backgroundColor = [[NSColor colorWithCalibratedRed:color.r
			green:color.g blue:color.b alpha:1.0] CGColor];
	}

	uint8_t *Platform::_GetPixelAddress(Number x, Number y) {
		CGContextRef context = _EnsureContext();
		size_t height = CGBitmapContextGetHeight(context);
		size_t width = CGBitmapContextGetWidth(context);
		if ((size_t)x >= width || (size_t)y >= height) {
			return NULL;
		}
		y = height - y;
		size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
		uint8_t *imageData = (uint8_t *)CGBitmapContextGetData(context);
		imageData += (bytesPerRow * (size_t)y) + (4 * (size_t)x);
		return imageData;
	}

	Color Platform::GetPixel(Number x, Number y) {
		uint8_t *pixel = _GetPixelAddress(x, y);
		if (pixel == NULL) {
			return Color(L"white");
		}
		return Color(pixel[0], pixel[1], pixel[2]);
	}

	void Platform::SetPixel(Number x, Number y, Color const& color) {
		uint8_t *pixel = _GetPixelAddress(x, y);
		if (pixel == NULL) {
			return;
		}
		pixel[0] = color.r;
		pixel[1] = color.g;
		pixel[2] = color.b;
		pixel[3] = 0xFF;
		_GetWindow().contentView.needsDisplay = YES;
	}

	void Platform::Clear(bool shapeLayer) {
		CGContextRef *context = shapeLayer ? &_shapeContext : &_backgroundContext;
		if (*context != NULL) {
			CGContextClearRect(*context, CGRectMake(0, 0,
				CGBitmapContextGetWidth(*context),
				CGBitmapContextGetHeight(*context)));
		}
	}

	void Platform::SetDimensions(Number width, Number height) {
		NSRect frame = _GetWindow().frame;
		frame.size.width = (CGFloat)width;
		frame.size.height = (CGFloat)height;
		[_GetWindow() setFrame:frame display:YES animate:NO];
	}

	void Platform::PauseTimer() {
		[_timer invalidate];
		_timer = nil;
	}

	bool Platform::IsTimerActive() {
		return _timer != nil;
	}

	void Platform::ResumeTimer() {
		_timer = [NSTimer scheduledTimerWithTimeInterval:(_timerInterval / 1000.)
			repeats:YES
			block:^(NSTimer *timer){ PostTimerEvent(); } ];
	}

	void Platform::_UpdateTimerInterval() {
		if (_timer != NULL) {
			PauseTimer();
			ResumeTimer();
		}
	}
	
	void Platform::ShowMessage(String const& text, String const& title) {
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setMessageText:WStringToNSString(text)];
		[alert setInformativeText:WStringToNSString(title)];
		[alert addButtonWithTitle:@"Ok"];
		[alert runModal];
	}

	void Platform::Draw(Drawable const& drawable, bool shapeLayer) {
		CGContextRef context = _EnsureContext(shapeLayer);
		if (drawable.type == Drawable::OVAL || drawable.type == Drawable::PATH) {
			if (drawable.filled) {
				_PrepareFill();
			}
			else {
				_PrepareStroke();
			}
		}
		switch (drawable.type) {
			case Drawable::PATH: {
				auto points = drawable.GetAbsolute();
				size_t pointCount = drawable.points.size();
				CGPoint cgPoints[pointCount];
				for (size_t i=0; i<pointCount; i++) {
					cgPoints[i] = CGPointMake(std::get<0>(points[i]),
						std::get<1>(points[i]));
				}
				CGContextAddLines(context, cgPoints, pointCount);
				CGContextClosePath(context);
				if (pointCount == 2) {
					CGContextDrawPath(context, kCGPathStroke);
				}
				else if (drawable.filled) {
					CGContextFillPath(context);
				}
				else {
					CGContextStrokePath(context);
				}
				break;
			}
			case Drawable::OVAL: {
				CGRect rect = CGRectMake(drawable.x, drawable.y, drawable.width,
					drawable.height);
				if (drawable.filled) {
					CGContextFillEllipseInRect(context, rect);
				}
				else {
					CGContextStrokeEllipseInRect(context, rect);
				}
				_GetWindow().contentView.needsDisplay = YES;
				break;
			}
			case Drawable::TEXT: {
				CGFloat width = CGFLOAT_MAX;
				if (drawable.boundText) {
					width = drawable.width;
				}
				NSString *text = WStringToNSString(drawable.text);
				if (fontChanged) {
					NSFontManager *fontManager = [NSFontManager sharedFontManager];
					NSFontTraitMask traits = 0;
					if (boldText) {
						traits |= NSBoldFontMask;
					}
					if (italicText) {
						traits |= NSItalicFontMask;
					}
					_font = [fontManager fontWithFamily:WStringToNSString(fontName)
						traits:traits weight:0 size:(CGFloat)fontSize];
				}

				// Create attributed string
				NSAttributedString *attributedStr = [[NSAttributedString alloc]
					initWithString:text attributes:@{ NSFontAttributeName: _font,
					NSForegroundColorAttributeName: [NSColor colorWithCGColor:_strokeColor] }];
				CFAttributedStringRef cfAttributedStr = (__bridge CFAttributedStringRef)
					attributedStr;

				// Create framesetter
				CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(
					cfAttributedStr);

				// Calculate frame
				CGRect rect;
				rect.size = CTFramesetterSuggestFrameSizeWithConstraints(framesetter,
					CFRangeMake(0, 0), NULL, CGSizeMake(width, CGFLOAT_MAX), NULL);
				rect.origin.x = drawable.x;
				rect.origin.y = drawable.y;

				// Draw
				CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0),
					CGPathCreateWithRect(rect, NULL), NULL);
				CTFrameDraw(frame, context);

				// Cleanup
				CFRelease(frame);
				CFRelease(framesetter);
				CGContextRestoreGState(context);
				break;
			}
			case Drawable::IMAGE: {
				break;
			}
		}
		_GetWindow().contentView.needsDisplay = YES;
	}
}

#undef INITIAL_WIDTH
#undef INITIAL_HEIGHT

#endif