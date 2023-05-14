#ifndef SMALLBASIC_COMMON_STRINGUTILS_H
#define SMALLBASIC_COMMON_STRINGUTILS_H

#include <algorithm>
#include "Mixed.hpp"

std::wstring StringToWString(std::string const& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

SmallBasic::String WStringToLower(SmallBasic::String str) {
	std::transform(str.begin(), str.end(), str.begin(), std::towlower);
	return str;
}

std::string WStringToString(SmallBasic::String const& str) {
	char buf[MB_CUR_MAX + 1];
	std::string result;
	std::wctomb(NULL, 0);
	for (wchar_t c : str) {
		int len = std::wctomb(buf, c);
		if (len < 0) {
			buf[0] = '?';
			len = 1;
		}
		buf[len] = '\0';
		result += buf;
	}
	return result;
}

#endif