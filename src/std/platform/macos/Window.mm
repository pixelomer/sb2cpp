#ifndef SMALLBASIC_PLATFORM_MACOS_WINDOW_MM
#define SMALLBASIC_PLATFORM_MACOS_WINDOW_MM

#include "../Window.hpp"
#include "Utils.mm"
#include "SmallBasicWindow.mm"

namespace SmallBasic {
	namespace Platform {
		void Window::SetVisible(bool visible) {
			_window.isVisible = visible;
		}

		void Window::SetResizable(bool canResize) {
			if (canResize) {
				_window.styleMask |= NSWindowStyleMaskResizable;
			}
			else {
				_window.styleMask &= ~NSWindowStyleMaskResizable;
			}
		}

		void Window::SetTitle(String const& title) {
			_window.title = WStringToNSString(title);
		}

		void Window::SetRect(Rect const& newRect) {
			NSRect rect;
			rect.origin.x = newRect.x;
			rect.origin.y = newRect.y;
			rect.size.width = newRect.width;
			rect.size.height = newRect.height;
			[_window setFrame:rect display:YES];
		}

		Rect Window::GetRect() {
			NSRect rect = _window.frame;
			return Rect(rect.origin.x, rect.origin.y, rect.size.width,
				rect.size.height);
		}

		void Window::ShowMessage(String const& text, String const& title) {
			NSAlert *alert = [[NSAlert alloc] init];
			[alert setMessageText:WStringToNSString(text)];
			[alert setInformativeText:WStringToNSString(title)];
			[alert addButtonWithTitle:@"Ok"];
			[alert runModal];
		}

		void Window::RedrawIfNeeded() {
			if (renderer->changed) {
				_window.contentView.needsDisplay = YES;
				renderer->changed = false;
			}
		}
		
		void Window::_Initialize() {
			NSRect frame = NSMakeRect(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
			_window = [[SmallBasicWindow alloc] initWithFrame:frame windowContext:this];
			[[NSNotificationCenter defaultCenter] addObserver:NSApp
				selector:@selector(terminate:) name:NSWindowWillCloseNotification
				object:_window];
		}

		Window::~Window() {
			[[NSNotificationCenter defaultCenter] removeObserver:NSApp
				name:NSWindowWillCloseNotification object:_window];
			[_window disconnect];
			_window = nil;
		}
	}
}

#endif