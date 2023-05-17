#ifndef SMALLBASIC_PLATFORM_MACOS_UTILS_MM
#define SMALLBASIC_PLATFORM_MACOS_UTILS_MM

#include <Foundation/Foundation.h>
#include "../../common/StringUtils.hpp"

std::wstring NSStringToWString(NSString *str) {
	return StringToWString([str UTF8String]);
}

NSString *WStringToNSString(SmallBasic::String const& str) {
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
	return @(result.c_str());
}

CGColorRef CGColorFromSBColor(SmallBasic::Color const& color, SmallBasic::Number
	opacity = 100.L)
{
	CGColorRef cgColor = CGColorCreateGenericRGB((CGFloat)color.r / 255.f,
		(CGFloat)color.g / 255.f, (CGFloat)color.b / 255.f, opacity / 100.L);
	return cgColor;
}

#endif