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
		static Number GetCount(Mixed const& stackName) {
			if (!StackExists(stackName)) return 0;
			return stacks[stackName].size();
		}
		static Mixed PopValue(Mixed const& stackName) {
			Mixed value = stacks[stackName].top();
			stacks[stackName].pop();
			return value;
		}
	};
}