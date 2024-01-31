#include <gtest/gtest.h>
#include <string>
#include "../libsb2cpp/tokenize.hpp"

using namespace sb2cpp;

void assertTokenize(std::string input, std::vector<std::string> const& expected) {
    auto tokens = tokenize(input);
    for (int i = 0; i<tokens.size() && i<expected.size(); i++) {
        ASSERT_EQ(tokens[i], expected[i]) << "Different value at index " << i;
    }
    ASSERT_EQ(tokens.size(), expected.size());
}

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
    auto expected = std::vector<std::string>({
        "TextWindow", ".", "Show", "(", ")", "\n\n\n",
        "name", "=", "TextWindow", ".", "Read", "(", ")", "\n",
        "TextWindow", ".", "WriteLine", "(", "\"Your name is: \"", "+", "name", ")", "\n",
        "If", "(", "name", "=", "\"John\"", "Or", "name", "=", "\"Michael\"", ")", "Then", "\n",
        "TextWindow", ".", "WriteLine", "(", "\"Welcome back!\"", ")", "\n",
        "EndIf", "\n",
        "ctemp", "=", "32", "\n",
        "ftemp", "=", "89.6"
    });
    assertTokenize(input, expected);
}

/**** Edge cases ****/

// An incomplete string should be tokenized as a token that starts
// with a quote but does not end with one.
TEST(TokenizeTest, IncompleteString) {
    auto input = 
        "str = \"Hello, world\n"
        "TextWindow.WriteLine(str)";
    std::vector<std::string> expected = {
        "str", "=", "\"Hello, world", "\n",
        "TextWindow", ".", "WriteLine", "(", "str", ")"
    };
    assertTokenize(input, expected);
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
    std::vector<std::string> expected = { "10.2" };
    assertTokenize(input, expected);
}
TEST(TokenizeTest, InvalidNumberEOF) {
    auto input = "10.2.5";
    std::vector<std::string> expected = { "10.2", ".", "5" };
    assertTokenize(input, expected);
}