#pragma once

#include "macros.hpp"
#include "util.hpp"

SB_CLASS(Text)

SB_METHOD_2(Append) (std::string const& str1, std::string const& str2) {
    return str1 + str2;
}

SB_METHOD_1(GetLength) (std::string const& str) {
    return (long)str.length();
}

SB_METHOD_2(IsSubText) (std::string const& haystack, std::string const& needle) {
    if (haystack.find(needle) != std::string::npos) {
        return SB_TRUE;
    }
    return SB_FALSE;
}

SB_METHOD_2(EndsWith) (std::string const& text, std::string const& subtext) {
    if (text.size() < subtext.size()) {
        return SB_FALSE;
    }
    auto substr = text.substr(text.size() - subtext.size(), subtext.size());
    if (substr != subtext) {
        return SB_FALSE;
    }
    return SB_TRUE;
}

SB_METHOD_2(StartsWith) (std::string const& text, std::string const& subtext) {
    if (text.size() < subtext.size()) {
        return SB_FALSE;
    }
    auto substr = text.substr(0, subtext.size());
    if (substr != subtext) {
        return SB_FALSE;
    }
    return SB_TRUE;
}

SB_METHOD_3(GetSubText) (std::string const& text, long pos, long len) {
    if (pos-1 >= text.length()) {
        return Obj("");
    }
    return text.substr(pos-1, std::min(len, (long)(text.length()-pos+1)));
}

SB_METHOD_2(GetSubTextToEnd) (std::string const& text, long pos) {
    if (pos-1 >= text.length()) {
        return Obj("");
    }
    return text.substr(pos-1);
}

SB_METHOD_2(GetIndexOf) (std::string const& text, std::string const& subtext) {
    auto pos = text.find(subtext);
    if (pos == std::string::npos) {
        return 0;
    }
    return (long)(pos + 1);
}

SB_METHOD_1(ConvertToLowerCase) (std::string const& text) {
    return strtolower(text);
}

SB_METHOD_1(ConvertToUpperCase) (std::string const& text) {
    return strtoupper(text);
}

#warning Unicode is not supported
SB_METHOD_1(GetCharacter) (long num) {
    char str[2] = { (char)num, 0 };
    return std::string(str);
}

SB_METHOD_1(GetCharacterCode) (std::string const& text) {
    return (long)text[0];
}

SB_CLASS_END