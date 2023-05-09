#ifndef SMALLBASIC_PLATFORM_MACOS_SMALLBASICVIEW_MM
#define SMALLBASIC_PLATFORM_MACOS_SMALLBASICVIEW_MM

#include "../Renderer.hpp"

@interface SmallBasicView : NSView
@end

@implementation SmallBasicView {
	SmallBasic::Platform::Renderer **_renderer;
}

- (instancetype)initWithFrame:(NSRect)frame
	renderer:(SmallBasic::Platform::Renderer **)renderer
{
	if ((self = [super initWithFrame:frame])) {
		_renderer = renderer;
		self.wantsLayer = YES;
	}
	return self;
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (BOOL)isFlipped { return YES; }

- (void)drawRect:(NSRect)dirtyRect {
	if (_renderer != nullptr && *_renderer != nullptr) {
		(*_renderer)->Render(self);
	}
}

- (void)disconnect {
	_renderer = NULL;
}

@end

#endif