#ifndef _SMALLBASIC_GRAPHICSWINDOW_H
#define _SMALLBASIC_GRAPHICSWINDOW_H

#include "Mixed.hpp"
#include "Platform.hpp"
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

		static void (*_handleKeyDown)();
		static void (*_handleKeyUp)();
		static void (*_handleMouseMove)();
		
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
		static Platform *_GetPlatform() {
			if (_platform == nullptr) {
				_platform = Platform::Default();
				_platform->onKeyDown = _OnKeyDown;
				_platform->onKeyUp = _OnKeyUp;
				_platform->onMouseMove = _OnMouseMove;
				_platform->SetFillColor(_brushColor);
				_platform->SetStrokeColor(_penColor);
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
			return Mixed::Boolean(_canResize);
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

		// - GraphicsWindow.Height
		// Gets or sets the Height of the graphics window.
		static void _SetHeight(Number height) {
			_GetPlatform()->SetHeight(height);
		}
		static Number _GetHeight() {
			return _GetPlatform()->GetHeight();
		}

		// - GraphicsWindow.Width
		// Gets or sets the Width of the graphics window.
		static void _SetWidth(Number width) {
			_GetPlatform()->SetWidth(width);
		}
		static Number _GetWidth() {
			return _GetPlatform()->GetWidth();
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

		// - GraphicsWindow.FontName
		// Gets or sets the Font Name to be used when drawing text on the
		// Graphics Window.
		static void _SetFontName(String const& fontName) {
			_GetPlatform()->fontName = fontName;
			_GetPlatform()->fontChanged = true;
		}
		static Mixed _GetFontName() {
			return _GetPlatform()->fontName;
		}

		// - GraphicsWindow.FontSize
		// Gets or sets the Font Size to be used when drawing text on the
		// Graphics Window.
		static void _SetFontSize(Number fontSize) {
			_GetPlatform()->fontSize = fontSize;
			_GetPlatform()->fontChanged = true;
		}
		static Mixed _GetFontSize() {
			return _GetPlatform()->fontSize;
		}

		// - GraphicsWindow.FontBold
		// Gets or sets whether or not the font to be used when drawing text
		// on the Graphics Window, is bold.
		static void _SetFontBold(bool boldText) {
			_GetPlatform()->boldText = boldText;
			_GetPlatform()->fontChanged = true;
		}
		static Mixed _GetFontBold() {
			return Mixed::Boolean(_GetPlatform()->boldText);
		}

		// - GraphicsWindow.FontItalic
		// Gets or sets whether or not the font to be used when drawing text
		// on the Graphics Window, is italic.
		static void _SetFontItalic(bool italicText) {
			_GetPlatform()->italicText = italicText;
			_GetPlatform()->fontChanged = true;
		}
		static Mixed _GetFontItalic() {
			return Mixed::Boolean(_GetPlatform()->italicText);
		}

		// - GraphicsWindow.LastKey
		// Gets the last key that was pressed or released.
		static Mixed _GetLastKey() {
			return _lastKey;
		}

		// - GraphicsWindow.MouseX
		// Gets the x-position of the mouse relative to the Graphics Window.
		static Mixed _GetMouseX() {
			return _lastMouseX;
		}

		// - GraphicsWindow.MouseY
		// Gets the y-position of the mouse relative to the Graphics Window.
		static Mixed _GetMouseY() {
			return _lastMouseY;
		}

		// - GraphicsWindow.Show()
		// Shows the Graphics window to enable interactions with it.
		static void Show() {
			_GetPlatform()->SetWindowVisible(true);
		}

		// - GraphicsWindow.Hide()
		// Hides the Graphics window.
		static void Hide() {
			_GetPlatform()->SetWindowVisible(false);
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

		// - GraphicsWindow.DrawEllipse(x, y, width, height)
		// Draws an ellipse on the screen using the selected Pen.
		static void DrawEllipse(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawEllipse(x, y, width, height, false);
		}

		// - GraphicsWindow.FillEllipse(x, y, width, height)
		// Fills an ellipse on the screen using the selected Brush.
		static void FillEllipse(Number x, Number y, Number width, Number height) {
			_GetPlatform()->DrawEllipse(x, y, width, height, true);
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

		// - GraphicsWindow.DrawLine(x1, y1, x2, y2)
		// Draws a line from one point to another.
		static void DrawLine(Number x1, Number y1, Number x2, Number y2) {
			_GetPlatform()->DrawLine(x1, y1, x2, y2);
		}

		// - GraphicsWindow.DrawText(x, y, text)
		// Draws a line of text on the screen at the specified location.
		static void DrawText(Number x, Number y, String const& text) {
			_GetPlatform()->DrawText(x, y, text);
		}

		// - GraphicsWindow.Clear()
		// Clears the window.
		static void Clear() {
			_GetPlatform()->ClearWindow();
		}

		// - GraphicsWindow.KeyDown
		// Raises an event when a key is pressed down on the keyboard.
		static void _SetKeyDown(void (*keyDown)()) {
			_platform->ignoresKeyEvents = false;
			_handleKeyDown = keyDown;
		}

		// - GraphicsWindow.KeyUp
		// Raises an event when a key is released on the keyboard.
		static void _SetKeyUp(void (*keyUp)()) {
			_platform->ignoresKeyEvents = false;
			_handleKeyUp = keyUp;
		}

		// - GraphicsWindow.MouseDown
		// Raises an event when the mouse button is clicked down.
		static void _SetMouseDown(void (*mouseDown)()) {
			_GetPlatform()->onMouseDown = mouseDown;
		}

		// - GraphicsWindow.MouseUp
		// Raises an event when the mouse button is released.
		static void _SetMouseUp(void (*mouseUp)()) {
			_GetPlatform()->onMouseUp = mouseUp;
		}

		// - GraphicsWindow.MouseMove
		// Raises an event when the mouse is moved around.
		static void _SetMouseMove(void (*mouseMove)()) {
			_handleMouseMove = mouseMove;
		}

		// - GraphicsWindow.GetPixel(x, y)
		// Gets the color of the pixel at the specified x and y co-ordinates.
		static Mixed GetPixel(Number x, Number y) {
			return _GetPlatform()->GetPixel(x, y).name;
		}

		// - GraphicsWindow.SetPixel(x, y, color)
		// Draws the pixel specified by the x and y co-ordinates using
		// the specified color.
		static void SetPixel(Number x, Number y, String const& colorName) {
			Color color = Color(colorName);
			_GetPlatform()->SetPixel(x, y, color);
		}

		// - GraphicsWindow.GetRandomColor()
		// Gets a valid random color.
		static Mixed GetRandomColor() {
			return GetColorFromRGB(random() % 256, random() % 256, random() % 256);
		}

		// - GraphicsWindow.GetColorFromRGB(red, green, blue)
		// Constructs a color given the Red, Green and Blue values.
		static Mixed GetColorFromRGB(Number r, Number g, Number b) {
			return Color::_NameFor(r, g, b);
		}

		// - GraphicsWindow.ShowMessage(text, title)
		// Displays a message box to the user.
		static void ShowMessage(String const& text, String const& title) {
			_GetPlatform()->ShowMessage(text, title);
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
	void (*GraphicsWindow::_handleKeyDown)() = NULL;
	void (*GraphicsWindow::_handleKeyUp)() = NULL;
	void (*GraphicsWindow::_handleMouseMove)() = NULL;
}

#endif