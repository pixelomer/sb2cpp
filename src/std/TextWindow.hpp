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
		static String Read() {
			String line;
			std::getline(std::wcin, line);
			return line;
		}
		static String ReadKey() {
			throw "Not implemented";
		}
		static Number ReadNumber() {
			String line = TextWindow::Read();
			return std::stold(line);
		}
		static void WriteLine(Mixed const& mixed) {
			Write(mixed);
			std::wcout << std::endl;
		}
		static void Write(Mixed const& mixed) {
			std::wcout << mixed.Describe();
		}
		static void VerifyAccess() {}
	};

}