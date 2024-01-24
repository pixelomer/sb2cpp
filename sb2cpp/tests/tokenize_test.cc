#include <gtest/gtest.h>
#include <string>
#include "../lib/tokenize.hpp"

using namespace sb2cpp;

/**** Valid/Expected inputs ****/

TEST(TokenizeTest, SimpleSourceCode) {
    auto input =
        "TextWindow.Show() ' Shows text window\n"
        "\n"
        "' Get user name\n"
        "name = TextWindow.Read()\n"
        "TextWindow.WriteLine(\"Your name is: \" + name)\n"
        "If (name = \"John\" Or name = \"Michael\") Then\n"
        "  TextWindow.WriteLine(\"Welcome back!\")\n"
        "EndIf\n"
        "ctemp = 32\n"
        "ftemp = 89.6";
    auto tokens = tokenize(input);
    auto expected = std::vector<std::string>({
        "TextWindow", ".", "Show", "(", ")", "\n",
        "name", "=", "TextWindow", ".", "Read", "(", ")", "\n",
        "TextWindow", ".", "WriteLine", "(", "\"Your name is: \"", "+", "name", ")", "\n",
        "If", "(", "name", "=", "\"John\"", "Or", "name", "=", "\"Michael\"", ")", "Then", "\n",
        "TextWindow", ".", "WriteLine", "(", "\"Welcome back!\"", ")", "\n",
        "EndIf", "\n",
        "ctemp", "=", "32", "\n",
        "ftemp", "=", "89.6"
    });
    ASSERT_TRUE(tokens == expected);
}

/**** Edge cases ****/

// An incomplete string should be tokenized as a token that starts
// with a quote but does not end with one.
TEST(TokenizeTest, IncompleteString) {
    auto input = 
        "str = \"Hello, world\n"
        "TextWindow.WriteLine(str)";
    auto tokens = tokenize(input);
    std::vector<std::string> expected = {
        "str", "=", "\"Hello, world", "\n",
        "TextWindow", ".", "WriteLine", "(", "str", ")"
    };
    ASSERT_TRUE(tokens == expected);
}
TEST(TokenizeTest, IncompleteStringEOF) {
    auto input = 
        "str = \"Hello, world";
    auto tokens = tokenize(input);
    auto expected = std::vector<std::string>({
        "str", "=", "\"Hello, world"
    });
    ASSERT_TRUE(tokens == expected);
}

// Numbers
TEST(TokenizeTest, NumberEOF) {
    auto input = "10.2";
    auto tokens = tokenize(input);
    std::vector<std::string> expected = { "10.2" };
    ASSERT_TRUE(tokens == expected);
}
TEST(TokenizeTest, InvalidNumberEOF) {
    auto input = "10.2.5";
    auto tokens = tokenize(input);
    std::vector<std::string> expected = { "10.2", ".", "5" };
    ASSERT_TRUE(tokens == expected);
}