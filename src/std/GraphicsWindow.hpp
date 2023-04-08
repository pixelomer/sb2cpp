#ifndef _SMALLBASIC_GRAPHICSWINDOW_H
#define _SMALLBASIC_GRAPHICSWINDOW_H

#include "Mixed.hpp"
#include "Platform.hpp"
#include "RunLoop.hpp"
#include "Color.hpp"

namespace SmallBasic {
	class GraphicsWindow {
	private:
		static Platform *_platform;
		static Color _backgroundColor;
		static Color _brushColor;
		static Color _penColor;
		static Number _penWidth;
		static String _title;
		static bool _canResize;
		
		static void _RunLoop() {
			_platform->Run();
		}
		static Platform *_GetPlatform() {
			if (_platform == nullptr) {
				_platform = new Platform();
				RunLoop::_PrepareRunLoop(_RunLoop);
			}
			return _platform;
		}
	public:
		// - GraphicsWindow.BackgroundColor
		// Gets or sets the Background color of the Graphics Window.
		static void _SetBackgroundColor(String const& color) {
			_backgroundColor = Color(color);
			_GetPlatform()->SetBackgroundColor(_backgroundColor);
		}
		static Mixed _GetBackgroundColor() {
			return _backgroundColor.name;
		}

		// - GraphicsWindow.BrushColor
		// Gets or sets the brush color to be used to fill shapes drawn
		// on the Graphics Window.
		static void _SetBrushColor(String const& brushColor) {
			_brushColor = Color(brushColor);
			_GetPlatform()->SetFillColor(_brushColor);
		}
		static Mixed _GetBrushColor() {
			return _brushColor.name;
		}

		// - GraphicsWindow.CanResize
		// Specifies whether or not the Graphics Window can be resized
		// by the user. 
		static void _SetCanResize(bool canResize) {
			_canResize = canResize;
			_GetPlatform()->SetCanResize(canResize);
		}
		static Mixed _GetCanResize() {
			return _canResize;
		}

		// - GraphicsWindow.Title
		// Gets or sets the title for the graphics window.
		static void _SetTitle(String const& title) {
			_title = title;
			_GetPlatform()->SetTitle(title);
		}
		static Mixed _GetTitle() {
			return _title;
		}
		
		// - GraphicsWindow.PenWidth
		// Gets or sets the width of the pen used to draw shapes on the
		// Graphics Window.
		static void _SetPenWidth(Number penWidth) {
			_penWidth = penWidth;
			_GetPlatform()->SetStrokeWidth(penWidth);
		}
		static Mixed _GetPenWidth() {
			return _penWidth;
		}
		
		// - GraphicsWindow.PenColor
		// Gets or sets the color of the pen used to draw shapes on the
		// Graphics Window. 
		static void _SetPenColor(Color const& penColor) {
			_penColor = Color(penColor);
			_GetPlatform()->SetStrokeColor(_penColor);
		}
		static Mixed _GetPenColor() {
			return _penColor.name;
		}

		// Operations
		static void Show() {
			_GetPlatform()->SetWindowVisible(true);
		}
		static void DrawRectangle(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawRectangle(x, y, width, height, false);
		}
		static void FillRectangle(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawRectangle(x, y, width, height, true);
		}

		// Events
		static void _SetKeyDown(void(*callback)()) {

		}
	};

	Platform *GraphicsWindow::_platform = NULL;
	Color GraphicsWindow::_backgroundColor = Color(L"white");
	Color GraphicsWindow::_brushColor = Color(L"black");
	Color GraphicsWindow::_penColor = Color(L"black");
	String GraphicsWindow::_title = L"";
	bool GraphicsWindow::_canResize = false;
}

#endif