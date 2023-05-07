#ifndef SMALLBASIC_PLATFORM_RENDERER_MM
#define SMALLBASIC_PLATFORM_RENDERER_MM

#include "Utils.mm"
#include <Cocoa/Cocoa.h>

namespace SmallBasic {
	namespace Platform {
		Renderer::Renderer() {
			_colorSpace = CGColorSpaceCreateDeviceRGB();
		}

		Renderer::~Renderer() {
			CGColorSpaceRelease(_colorSpace);
			for (auto pair : _layers) {
				CGContextRelease(pair.second);
			}
		}

		CGContextRef Renderer::_GetLayer(enum Layer layer, Number width,
			Number height)
		{
			size_t minWidth = (width > 0) ? width : 0;
			size_t minHeight = (height > 0) ? height : 0;
			CGContextRef &context = _layers[layer];
			
			if (context != NULL) {
				size_t currentWidth = CGBitmapContextGetWidth(context);
				size_t currentHeight = CGBitmapContextGetHeight(context);
				if (minWidth <= currentWidth && minHeight <= currentHeight) {
					return context;
				}
			}
			CGContextRef newContext = CGBitmapContextCreate(NULL, minWidth, minHeight,
				8, 0, _colorSpace, kCGImageAlphaPremultipliedLast);
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
			}
			context = newContext;
			return context;
		}

		CGContextRef Renderer::_GetLayer(enum Layer layer) {
			return _GetLayer(layer, 0, 0);
		}

		void Renderer::_SetBackgroundColor(Color const& backgroundColor) {

		}

		void Renderer::_Draw(enum Layer layer, Drawable const& drawable) {
			CGContextRef context = _GetLayer(layer, drawable.graphics.width,
				drawable.graphics.height);

			CGColorRef cgColor = CGColorFromSBColor(drawable.graphics.brushColor);
			CGContextSetFillColorWithColor(context, cgColor);
			CGColorRelease(cgColor);
			cgColor = CGColorFromSBColor(drawable.graphics.penColor);
			CGContextSetStrokeColorWithColor(context, cgColor);
			CGColorRelease(cgColor);
			cgColor = NULL;

			CGContextSetLineWidth(context, (CGFloat)drawable.graphics.penWidth);

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
					if (pointCount == 2 || !drawable.filled) {
						CGContextStrokePath(context);
					}
					else {
						CGContextDrawPath(context, kCGPathFillStroke);
					}
					break;
				}
				case Drawable::OVAL: {
					CGRect rect = CGRectMake(drawable.x, drawable.y, drawable.width,
						drawable.height);
					if (drawable.filled) {
						CGContextFillEllipseInRect(context, rect);
					}
					CGContextStrokeEllipseInRect(context, rect);
					break;
				}
				case Drawable::TEXT: {
					CGFloat width = CGFLOAT_MAX;
					if (drawable.boundText) {
						width = drawable.width;
					}
					NSString *text = WStringToNSString(drawable.text);
					
					NSFontManager *fontManager = [NSFontManager sharedFontManager];
					NSFontTraitMask traits = 0;
					if (drawable.graphics.fontBold) {
						traits |= NSBoldFontMask;
					}
					if (drawable.graphics.fontItalic) {
						traits |= NSItalicFontMask;
					}
					NSFont *font = [fontManager
						fontWithFamily:WStringToNSString(drawable.graphics.fontName)
						traits:traits weight:0 size:(CGFloat)drawable.graphics.fontSize];
					if (font == nil) {
						font = [NSFont systemFontOfSize:(CGFloat)drawable.graphics.fontSize];
						font = [fontManager convertFont:font toHaveTrait:traits];
						font = [fontManager convertFont:font toHaveTrait:traits];
					}

					// Create attributed string
					auto &penColor = drawable.graphics.penColor;
					NSAttributedString *attributedStr = [[NSAttributedString alloc]
						initWithString:text attributes:@{ NSFontAttributeName: font,
						NSForegroundColorAttributeName:(__bridge NSColor *)
						CGColorFromSBColor(penColor) }];
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
					break;
				}
				case Drawable::IMAGE: {
					break;
				}
			}
		}

		void Renderer::_Clear(enum Layer layer) {
			CGContextRef context = _GetLayer(layer);
			CGContextClearRect(context, CGRectMake(0, 0, CGBitmapContextGetWidth(context),
				CGBitmapContextGetHeight(context)));
		}

		void Renderer::_ClearAll() {
			for (auto &pair : _layers) {
				_Clear(pair.first);
			}
		}

		uint8_t *Renderer::_GetPixelAddress(Number x, Number y) {
			CGContextRef context = _GetLayer(BACKGROUND_LAYER);
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

		Color Renderer::GetPixel(Number x, Number y) {
			uint8_t *pixel = _GetPixelAddress(x, y);
			if (pixel == NULL) {
				return Color(L"white");
			}
			return Color(pixel[0], pixel[1], pixel[2]);
		}

		void Renderer::_SetPixel(Number x, Number y, Color const& color) {
			uint8_t *pixel = _GetPixelAddress(x, y);
			if (pixel == NULL) {
				return;
			}
			pixel[0] = color.r;
			pixel[1] = color.g;
			pixel[2] = color.b;
			pixel[3] = 0xFF;
		}

		void Renderer::Render() {
			CGContextRef current = [[NSGraphicsContext currentContext] CGContext];
			for (auto pair : _layers) {
				CGContextRef context = pair.second;
				CGImageRef image = CGBitmapContextCreateImage(context);
				CGContextDrawImage(current, CGRectMake(0, 0, CGImageGetWidth(image),
					CGImageGetHeight(image)), image);
				CGImageRelease(image);
			}
		}
	}
}

#endif