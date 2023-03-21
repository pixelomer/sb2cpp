#include "Types.hpp"
#include <thread>
#include <chrono>
#include <filesystem>

namespace SmallBasic {
	class Program {
	public:
		Number _GetArgumentCount() {
			throw std::runtime_error("Not implemented");
		}
		Mixed _GetDirectory() {
			return Mixed(std::filesystem::current_path());
		}
		void End() {
			exit(0);
		}
		String GetArgument(Number index) {
			throw std::runtime_error("Not implemented");
		}
		void Delay(Number milliseconds) {
			std::this_thread::sleep_for(std::chrono::milliseconds(
				(long long)milliseconds));
		}
	};
}