#include "Types.hpp"

namespace SmallBasic {
	class Text {
	public:
		static String Append(String const& str1, String const& str2) {
			return str1 + str2;
		}
		static Number GetLength(String const& str) {
			return str.length();
		}
		static Number IsSubText(String const& str1, String const& str2) {
			return str1.find(str2) != String::npos;
		}
		static Number EndsWith(String const& str, String const &suffix) {
			if (str.length() < suffix.length()) return false;
			return str.substr(str.length() - suffix.length()) == suffix;
		}
		static Number StartsWith(String const& str, String const& prefix) {
			if (str.length() < prefix.length()) return false;
			return str.substr(0, prefix.length()) == prefix;
		}
		static String GetSubText(String const& str, Number start, Number length) {
			return str.substr(start - 1, length);
		}
		static String GetSubTextToEnd(String const& str, Number start) {
			return str.substr(start - 1);
		}
		static Number GetIndexOf(String const& haystack, String const& needle) {
			size_t pos = haystack.find(needle);
			if (pos == String::npos) return 0;
			return pos + 1;
		}
		static String ConvertToLowerCase(String const& str) {
			String newString(str.length(), 0);
			std::transform(str.begin(), str.end(), newString.begin(), std::towlower);
			return newString;
		}
		static String ConvertToUpperCase(String const& str) {
			String newString(str.length(), 0);
			std::transform(str.begin(), str.end(), newString.begin(), std::towupper);
			return newString;
		}
		static String GetCharacter(Number characterCode) {
			String str(1, characterCode);
			return str;
		}
		static Number GetCharacterCode(String const& str) {
			return str[0];
		}
	};
}