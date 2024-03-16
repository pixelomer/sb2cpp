#pragma once

#include <string>
#include <sstream>

namespace sb2cpp {

class CodeWriter {
public:
    static const std::string endl;
    class CodeWriterError : public std::runtime_error {
    public:
        CodeWriterError(const char *msg): std::runtime_error(msg) {}
    };
private:
    std::stringstream stream;
    bool new_line = true;
public:
    int indent = 0;
    std::string indent_str = "    ";

    CodeWriter &operator<<(std::string const& str);
    CodeWriter &operator<<(double num);
    std::string str();
    void clear();
};

}

