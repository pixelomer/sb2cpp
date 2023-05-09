#ifndef SMALLBASIC_PLATFORM_MACOS_UTILS_MM
#define SMALLBASIC_PLATFORM_MACOS_UTILS_MM

#include <Foundation/Foundation.h>

std::wstring NSStringToWString(NSString *str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string utf8 = [str UTF8String];
	return converter.from_bytes(utf8);
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