#include "util.hpp"

namespace SmallBasic {

std::string strtolower(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return input;
}

std::string strtoupper(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(),
        [](unsigned char c){ return std::toupper(c); });
    return input;
}

}