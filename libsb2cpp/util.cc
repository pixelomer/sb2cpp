#include "util.hpp"
#include <string>
#include <algorithm>
#include <cctype>

namespace sb2cpp {

std::string strtolower(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return input;
}

}