#ifndef _SMALLBASIC_PROGRAM_H
#define _SMALLBASIC_PROGRAM_H

#include "../common/Mixed.hpp"
#include "../platform/RunLoop.hpp"
#include <thread>
#include <chrono>
#include <random>

#if __cplusplus >= 201703
#include <filesystem>
#else
#include <unistd.h>
#endif

namespace SmallBasic {
	namespace Std {
		class Program {
		private:
			static Mixed _arguments;
		public:
			static Mixed _GetArgumentCount() {
				return _arguments.ArrayLength();
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
				if (!_arguments.HasElement(index)) {
					throw std::runtime_error("No such argument");
				}
				return _arguments[index];
			}
			static void Delay(Number milliseconds) {
				Platform::RunLoop::Default()->RunFor(milliseconds);
			}
			static void _Run(int argc, char **argv, void(*mainFunction)()) {
				srandom(time(NULL));
				for (int i=0; i<argc; i++) {
					_arguments[i] = argv[i];
				}
				Platform::RunLoop::Default()->Run(mainFunction);
			}
		};

		Mixed Program::_arguments = {};
	}
}

#endif