#include "Types.hpp"

#include <map>
#include <stack>

namespace SmallBasic {
	class Stack {
	private:
		static std::map<Mixed, std::stack<Mixed>> stacks;
		static bool StackExists(Mixed const& stackName) {
			return stacks.count(stackName) > 0;
		}
	public:
		static void PushValue(Mixed const& stackName, Mixed const& value) {
			stacks[stackName].push(value);
		}
		static Mixed GetCount(Mixed const& stackName) {
			if (!StackExists(stackName)) return 0;
			return stacks[stackName].size();
		}
		static Mixed PopValue(Mixed const& stackName) {
			if (GetCount(stackName) == Mixed(0.L)) {
				Die("Stack empty.");
			}
			Mixed value = stacks[stackName].top();
			stacks[stackName].pop();
			return value;
		}
	};

	auto Stack::stacks = std::map<Mixed, std::stack<Mixed>>{};
}