#ifndef _SMALLBASIC_GRAPHICSWINDOW_H
#define _SMALLBASIC_GRAPHICSWINDOW_H

#include "../common/Mixed.hpp"
#include "../common/Drawable.hpp"
#include "../common/Updatable.hpp"
#include "../common/Color.hpp"
#include "../common/Rect.hpp"
#include "../common/Pos.hpp"
#include "../platform/Renderer.hpp"
#include "../common/GraphicsState.hpp"

//FIXME: Rewrite this entire thing

namespace SmallBasic {
	namespace Platform {
		class RunLoop;
	}
	namespace Std {
		class GraphicsWindow {
		private:
			friend class Platform::RunLoop;
			static Updatable<bool> _visible;
			static Updatable<bool> _canResize;
			static Updatable<bool> _fontBold;
			static Updatable<bool> _fontItalic;
			static Updatable<String> _title;
			static Updatable<Number> _penWidth;
			static Updatable<Color> _penColor;
			static Updatable<Color> _backgroundColor;
			static Updatable<Color> _brushColor;
			static Updatable<String> _fontName;
			static Updatable<Number> _fontSize;
			static Updatable<Rect> _rect;

			static void (*_keyDown)();
			static void (*_mouseDown)();
			static String _lastKey;
			static Pos _mouse;
		public:
			static Drawable _MakeDrawable(Drawable drawable) {
				drawable.graphics = _ExportState();
				return drawable;
			}

			static GraphicsState _ExportState() {
				return GraphicsState {
					.brushColor = _brushColor.get(),
					.fontBold = _fontBold.get(),
					.fontItalic = _fontItalic.get(),
					.fontName = _fontName.get(),
					.fontSize = _fontSize.get(),
					.width = _rect.get().width,
					.height = _rect.get().height,
					.penColor = _penColor.get(),
					.penWidth = _penWidth.get()
				};
			}

			// KeyDown
			static void _SetKeyDown(void(*keyDown)()) {
				_keyDown = keyDown;
			}

			// MouseDown
			static void _SetMouseDown(void(*mouseDown)()) {
				_mouseDown = mouseDown;
			}

			// BackgroundColor
			static Mixed _GetBackgroundColor() {
				return _backgroundColor.get().name;
			}
			static void _SetBackgroundColor(String const& name) {
				_backgroundColor = Color(name);
			}

			// BrushColor
			static Mixed _GetBrushColor() {
				return _brushColor.get().name;
			}
			static void _SetBrushColor(String const& name) {
				_brushColor = Color(name);
			}

			// CanResize
			static Mixed _GetCanResize() {
				return Mixed::Boolean(_canResize.get());
			}
			static void _SetCanResize(bool canResize) {
				_canResize = canResize;
			}

			// PenWidth
			static Mixed _GetPenWidth() {
				return _penWidth.get();
			}
			static void _SetPenWidth(Number width) {
				_penWidth = width;
			}

			// PenColor
			static Mixed _GetPenColor() {
				return _penWidth.get();
			}
			static void _SetPenColor(String const& name) {
				_penColor = Color(name);
			}

			// FontName
			static Mixed _GetFontName() {
				return _fontName.get();
			}
			static void _SetFontName(String const& name) {
				_fontName = name;
			}

			// FontSize
			static Mixed _GetFontSize() {
				return _fontSize.get();
			}
			static void _SetFontSize(Number size) {
				_fontSize = size;
			}

			// FontBold
			static Mixed _GetFontBold() {
				return Mixed::Boolean(_fontBold.get());
			}
			static void _SetFontBold(bool bold) {
				_fontBold = bold;
			}

			// FontItalic
			static Mixed _GetFontItalic() {
				return Mixed::Boolean(_fontItalic.get());
			}
			static void _SetFontItalic(bool italic) {
				_fontItalic = italic;
			}

			// Left
			static Mixed _GetLeft() {
				return _rect.get().x;
			}
			static void _SetLeft(Number left) {
				_rect.mut().x = left;
			}

			// Top
			static Mixed _GetTop() {
				return _rect.get().y;
			}
			static void _SetTop(Number top) {
				_rect.mut().y = top;
			}

			// Title
			static Mixed _GetTitle() {
				return _title.get();
			}
			static void _SetTitle(String const& title) {
				_title = title;
			}

			// Height
			static Mixed _GetHeight() {
				return _rect.get().height;
			}
			static void _SetHeight(Number height) {
				_rect.mut().height = height;
			}

			// Width
			static Mixed _GetWidth() {
				return _rect.get().width;
			}
			static void _SetWidth(Number width) {
				_rect.mut().width = width;
			}

			// LastKey
			static Mixed _GetLastKey() {
				return _lastKey;
			}

			// MouseX
			static Mixed _GetMouseX() {
				return _mouse.x;
			}

			// MouseY
			static Mixed _GetMouseY() {
				return _mouse.y;
			}

			static void Show() {
				_visible = true;
			}

			static void Hide() {
				_visible = false;
			}

			static void DrawRectangle(Number x, Number y, Number width, Number height) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, { {0,0}, {width,0}, {width,height},
					{0,height} }, false)));
			}

			static void FillRectangle(Number x, Number y, Number width, Number height) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, {{0,0}, {width,0}, {width,height},
					{0,height}}, true)));
			}

			static void DrawTriangle(Number x1, Number y1, Number x2, Number y2,
				Number x3, Number y3)
			{
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable({{x1,y1}, {x2,y2}, {x3,y3}}, false)));
			}

			static void FillTriangle(Number x1, Number y1, Number x2, Number y2,
				Number x3, Number y3)
			{
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable({{x1,y1}, {x2,y2}, {x3,y3}}, true)));
			}

			static void DrawEllipse(Number x, Number y, Number width, Number height) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, width, height, false)));
			}

			static void FillEllipse(Number x, Number y, Number width, Number height) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, width, height, true)));
			}

			static void DrawLine(Number x1, Number y1, Number x2, Number y2) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable({{x1,y1}, {x2,y2}}, false)));
			}

			static void DrawText(Number x, Number y, String const& text) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, text)));
			}

			static void DrawBoundText(Number x, Number y, Number width, String const& text) {
				Platform::Renderer::Default()->Draw(Platform::Renderer::BACKGROUND_LAYER,
					_MakeDrawable(Drawable(x, y, text, width)));
			}

			static Mixed GetRandomColor() {
				return GetColorFromRGB(random() % 256, random() % 256, random() % 256);
			}

			static Mixed GetColorFromRGB(Number r, Number g, Number b) {
				return Color::_NameFor(r, g, b);
			}

			static void ShowMessage(String const& message, String const& title) {
				std::wcerr << "[" << title << "] " << message << std::endl;
			}

			static void Clear();
		};

		Updatable<bool> GraphicsWindow::_visible = false;
		Updatable<bool> GraphicsWindow::_canResize = false;
		Updatable<bool> GraphicsWindow::_fontBold = false;
		Updatable<bool> GraphicsWindow::_fontItalic = false;
		Updatable<String> GraphicsWindow::_title = String(L"SmallBasic");
		Updatable<Number> GraphicsWindow::_penWidth = 2.L;
		Updatable<Color> GraphicsWindow::_penColor = Color(L"black");
		Updatable<Color> GraphicsWindow::_backgroundColor = Color(L"white");
		Updatable<Color> GraphicsWindow::_brushColor = Color(L"black");
		Updatable<String> GraphicsWindow::_fontName = String(L"Arial");
		Updatable<Number> GraphicsWindow::_fontSize = 10.L;
		Updatable<Rect> GraphicsWindow::_rect = Rect(100, 100, 640, 480);

		String GraphicsWindow::_lastKey = L"";
		Pos GraphicsWindow::_mouse = { 0, 0 };
		void (*GraphicsWindow::_keyDown)() = NULL;
		void (*GraphicsWindow::_mouseDown)() = NULL;
	}
}

#include "Shapes.hpp"

namespace SmallBasic {
	namespace Std {
		void GraphicsWindow::Clear() {
			Platform::Renderer::Default()->ClearAll();
			Shapes::_HideAll();
		}
	}
}

#endif