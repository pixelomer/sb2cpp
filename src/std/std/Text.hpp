#ifndef _SMALLBASIC_TEXT_H
#define _SMALLBASIC_TEXT_H

#include "../common/Mixed.hpp"

namespace SmallBasic {
	namespace Std {
		class Text {
		public:
			static Mixed Append(String const& str1, String const& str2) {
				return str1 + str2;
			}
			static Mixed GetLength(String const& str) {
				return str.length();
			}
			static Mixed IsSubText(String const& str1, String const& str2) {
				return str1.find(str2) != String::npos;
			}
			static Mixed EndsWith(String const& str, String const &suffix) {
				if (str.length() < suffix.length()) return false;
				return str.substr(str.length() - suffix.length()) == suffix;
			}
			static Mixed StartsWith(String const& str, String const& prefix) {
				if (str.length() < prefix.length()) return false;
				return str.substr(0, prefix.length()) == prefix;
			}
			static Mixed GetSubText(String const& str, Number start, Number length) {
				return str.substr(start - 1, length);
			}
			static Mixed GetSubTextToEnd(String const& str, Number start) {
				return str.substr(start - 1);
			}
			static Mixed GetIndexOf(String const& haystack, String const& needle) {
				size_t pos = haystack.find(needle);
				if (pos == String::npos) return 0.L;
				return pos + 1;
			}
			static Mixed ConvertToLowerCase(String str) {
				std::transform(str.begin(), str.end(), str.begin(), std::towlower);
				return str;
			}
			static Mixed ConvertToUpperCase(String str) {
				std::transform(str.begin(), str.end(), str.begin(), std::towupper);
				return str;
			}
			static Mixed GetCharacter(Number characterCode) {
				String str(1, characterCode);
				return str;
			}
			static Mixed GetCharacterCode(String const& str) {
				return str[0];
			}
		};
	}
}

#endif