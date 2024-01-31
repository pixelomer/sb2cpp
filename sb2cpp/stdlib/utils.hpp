#pragma once

#include <string>

namespace SmallBasic {

class Substr {
public:
    size_t index = std::string::npos;
    size_t length = 0;
    std::string str;

    Substr(size_t index, size_t length, std::string const& str):
        index(index), length(length), str(str) {}
    Substr() {}
};

Substr substr_until(size_t start, std::string const& str, char c) {
    std::string search_list = "\\" + std::string(&c, 1);
    size_t idx = start;
    while (true) {
        idx = str.find_first_of(search_list, idx);
        if (idx == std::string::npos) return Substr();
        else if (str[idx] == '\\') {
            idx += 2;
            continue;
        }
        else break;
    }
    Substr substr(start, idx-start, str.substr(start, idx-start));
    return substr;
}

}