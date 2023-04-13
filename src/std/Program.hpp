#ifndef _SMALLBASIC_PROGRAM_H
#define _SMALLBASIC_PROGRAM_H

#include "Mixed.hpp"
#include <thread>
#include <chrono>

#if __cplusplus >= 201703
#include <filesystem>
#else
#include <unistd.h>
#endif

namespace SmallBasic {
	class Program {
	public:
		static Mixed _GetArgumentCount() {
			throw std::runtime_error("Not implemented");
		}
		static Mixed _GetDirectory() {
#if __cplusplus >= 201703
			return Mixed(std::filesystem::current_path());
#else
			char *cwd = getwd(NULL);
			Mixed mixed(cwd);
			free(cwd);
			return mixed;
#endif
		}
		static void End() {
			exit(0);
		}
		static Mixed GetArgument(Number index) {
			throw std::runtime_error("Not implemented");
		}
		static void Delay(Number milliseconds) {
			//FIXME: GUI needs to be interactive during Delay()
			std::this_thread::sleep_for(std::chrono::milliseconds(
				(long long)milliseconds));
		}
	};
}

#endif