#include "code_writer.hpp"

namespace sb2cpp {

const std::string CodeWriter::endl = "\n";

CodeWriter &CodeWriter::operator<<(std::string const& str) {
    if (str == endl) {
        this->stream << std::endl;
        this->new_line = true;
    }
    else if (str.find('\n') != std::string::npos) {
        throw CodeWriterError("Expected CodeWriter::endl or "
            "text without newline");
    }
    else {
        if (this->new_line) {
            for (int i=0; i<indent; i++) {
                this->stream << this->indent_str;
            }
            this->new_line = false;
        }
        this->stream << str;
    }
    return *this;
}
CodeWriter &CodeWriter::operator<<(double num) {
    this->stream << num;
    return *this;
}
std::string CodeWriter::str() {
    return this->stream.str();
}
void CodeWriter::clear() {
    this->stream.clear();
}

}