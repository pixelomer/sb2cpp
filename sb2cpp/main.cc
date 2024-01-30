#include <iostream>
#include <sstream>
#include <fstream>
#include "lib/transpiler.hpp"

using namespace sb2cpp;

int main() {
    std::stringstream buffer;
    std::ifstream t("test.sb");
    buffer << t.rdbuf();
    auto input = buffer.str();

    try {
        Transpiler transpiler(input);
        auto output = transpiler.transpile();
        std::cout << output;
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
    return EXIT_SUCCESS;
}