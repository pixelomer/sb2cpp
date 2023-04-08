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

		static Number _lastMouseX;
		static Number _lastMouseY;
		static String _lastKey;

		static void (*_handleMouseDown)();
		static void (*_handleMouseUp)();
		static void (*_handleKeyDown)();
		static void (*_handleKeyUp)();
		static void (*_handleMouseMove)();
		
		static void _RunLoop() {
			_platform->Run();
		}
		static void _OnKeyDown(String const& key) {
			_lastKey = key;
			if (_handleKeyDown != NULL) {
				_handleKeyDown();
			}
		}
		static void _OnKeyUp(String const& key) {
			_lastKey = key;
			if (_handleKeyUp != NULL) {
				_handleKeyUp();
			}
		}
		static void _OnMouseMove(Number x, Number y) {
			_lastMouseX = x;
			_lastMouseY = y;
			if (_handleMouseMove != NULL) {
				_handleMouseMove();
			}
		}
		static void _OnMouseDown() {
			if (_handleMouseDown != NULL) {
				_handleMouseDown();
			}
		}
		static void _OnMouseUp() {
			if (_handleMouseUp != NULL) {
				_handleMouseUp();
			}
		}
		static Platform *_GetPlatform() {
			if (_platform == nullptr) {
				_platform = new Platform();
				_platform->OnKeyDown = _OnKeyDown;
				_platform->OnKeyUp = _OnKeyUp;
				_platform->OnMouseDown = _OnMouseDown;
				_platform->OnMouseUp = _OnMouseUp;
				_platform->OnMouseMove = _OnMouseMove;
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
		static void _SetPenColor(String const& penColor) {
			_penColor = Color(penColor);
			_GetPlatform()->SetStrokeColor(_penColor);
		}
		static Mixed _GetPenColor() {
			return _penColor.name;
		}

		// - GraphicsWindow.LastKey
		// Gets the last key that was pressed or released.
		static String _GetLastKey() {
			return _lastKey;
		}

		// Operations
		static void Show() {
			_GetPlatform()->SetWindowVisible(true);
		}

		// - GraphicsWindow.DrawRectangle(x, y, width, height)
		// Draws a rectangle on the screen using the selected Pen.
		static void DrawRectangle(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawRectangle(x, y, width, height, false);
		}

		// - GraphicsWindow.FillRectangle(x, y, width, height)
		// Fills a rectangle on the screen using the selected Brush.
		static void FillRectangle(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawRectangle(x, y, width, height, true);
		}

		// - GraphicsWindow.DrawTriangle(x1, y1, x2, y2, x3, y3)
		// Draws a triangle on the screen using the selected pen.
		static void DrawTriangle(Number x1, Number y1, Number x2, Number y2, 
			Number x3, Number y3)
		{
			_GetPlatform()->DrawTriangle(x1, y1, x2, y2, x3, y3, false);
		}

		// - GraphicsWindow.FillTriangle(x1, y1, x2, y2, x3, y3)
		// Draws and fills a triangle on the screen using the selected brush.
		static void FillTriangle(Number x1, Number y1, Number x2, Number y2, 
			Number x3, Number y3)
		{
			_GetPlatform()->DrawTriangle(x1, y1, x2, y2, x3, y3, true);
		}

		// - GraphicsWindow.Clear()
		// Clears the window.
		static void Clear() {
			_GetPlatform()->ClearWindow();
		}

		// - GraphicsWindow.KeyDown
		// Raises an event when a key is pressed down on the keyboard.
		static void _SetKeyDown(void (*keyDown)()) {
			_handleKeyDown = keyDown;
		}

		// - GraphicsWindow.KeyUp
		// Raises an event when a key is released on the keyboard.
		static void _SetKeyUp(void (*keyUp)()) {
			_handleKeyUp = keyUp;
		}

		// - GraphicsWindow.MouseDown
		// Raises an event when the mouse button is clicked down.
		static void _SetMouseDown(void (*mouseDown)()) {
			_handleMouseDown = mouseDown;
		}

		// - GraphicsWindow.MouseUp
		// Raises an event when the mouse button is released.
		static void _SetMouseUp(void (*mouseUp)()) {
			_handleMouseDown = mouseUp;
		}

		// - GraphicsWindow.MouseMove
		// Raises an event when the mouse is moved around.
		static void _SetMouseMove(void (*mouseMove)()) {
			_handleMouseMove = mouseMove;
		}
	};

	Platform *GraphicsWindow::_platform = NULL;
	Color GraphicsWindow::_backgroundColor = Color(L"white");
	Color GraphicsWindow::_brushColor = Color(L"black");
	Color GraphicsWindow::_penColor = Color(L"black");
	Number GraphicsWindow::_penWidth = 0.L;
	String GraphicsWindow::_title = L"";
	bool GraphicsWindow::_canResize = false;
	Number GraphicsWindow::_lastMouseX = 0.L;
	Number GraphicsWindow::_lastMouseY = 0.L;
	String GraphicsWindow::_lastKey = L"";
	void (*GraphicsWindow::_handleMouseDown)() = NULL;
	void (*GraphicsWindow::_handleMouseUp)() = NULL;
	void (*GraphicsWindow::_handleKeyDown)() = NULL;
	void (*GraphicsWindow::_handleKeyUp)() = NULL;
	void (*GraphicsWindow::_handleMouseMove)() = NULL;
}

#endif