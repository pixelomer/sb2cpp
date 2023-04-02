#include "Types.hpp"

#include <iostream>
#include <string>

namespace SmallBasic {

	class TextWindow {
	public:
		static void Show() {}
		static void Hide() {}
		static void Clear() {}
		static void Pause() {
			std::wcout << "Press ENTER to continue..." << std::endl;
			PauseWithoutMessage();
		}
		static void PauseIfVisible() {
			Pause();
		}
		static void PauseWithoutMessage() {
			Read();
		}
		static Mixed Read() {
			String line;
			std::getline(std::wcin, line);
			return line;
		}
		static Mixed ReadKey() {
			throw "Not implemented";
		}
		static Mixed ReadNumber() {
			String line = TextWindow::Read();
			return std::stold(line);
		}
		static void WriteLine(Mixed const& mixed) {
			Write(mixed);
			std::wcout << std::endl;
		}
		static void Write(Mixed const& mixed) {
			std::wcout << mixed.GetString();
		}
		static void VerifyAccess() {}
	};

}