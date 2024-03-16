#include <iostream>
#include <sstream>
#include <fstream>
#include "libsb2cpp/interpreter.hpp"
#include "libsb2cpp/transpiler.hpp"

using namespace sb2cpp;

int main() {
    std::stringstream buffer;
    std::ifstream t("test.sb");
    buffer << t.rdbuf();
    auto input = buffer.str();

    try {
        auto source = std::make_shared<Source>(input, true);
        if (source->errors.size() != 0) {
            for (auto &error : source->errors) {
                std::cerr << error << std::endl;
            }
            return EXIT_FAILURE;
        }
        Interpreter interpreter(source);
        interpreter.run(true);
    }
    catch (Parser::SyntaxError err) {
        std::cerr << "source(line " << err.line << "): error: " << err.what()
            << std::endl;
        return EXIT_FAILURE;
    }
    catch (Source::SourceError err) {
        std::cerr << "source: error: " << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::runtime_error err) {
        std::cerr << "source: error: " << err.what() << std::endl;
    }
    return EXIT_SUCCESS;
}