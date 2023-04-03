#include "Types.hpp"
#include <thread>
#include <chrono>
#include <filesystem>

namespace SmallBasic {
	class Program {
	public:
		static Mixed _GetArgumentCount() {
			throw std::runtime_error("Not implemented");
		}
		static Mixed _GetDirectory() {
			return Mixed(std::filesystem::current_path());
		}
		static void End() {
			exit(0);
		}
		static Mixed GetArgument(Number index) {
			throw std::runtime_error("Not implemented");
		}
		static void Delay(Number milliseconds) {
			std::this_thread::sleep_for(std::chrono::milliseconds(
				(long long)milliseconds));
		}
	};
}