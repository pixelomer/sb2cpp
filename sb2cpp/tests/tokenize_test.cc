#include <gtest/gtest.h>
#include <string>
#include "../lib/tokenize.hpp"

using namespace sb2cpp;

TEST(TokenizeTest, SimpleSourceCode) {
    auto input =
        L"TextWindow.Show() ' Shows text window\n"
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
    auto expected = std::vector<std::wstring>({
        L"TextWindow", L".", L"Show", L"(", L")", L"\n",
        L"name", L"=", L"TextWindow", L".", L"Read", L"(", L")", L"\n",
        L"TextWindow", L".", L"WriteLine", L"(", L"\"Your name is: \"", L"+", L"name", L")", L"\n",
        L"If", L"(", L"name", L"=", L"\"John\"", L"Or", L"name", L"=", L"\"Michael\"", L")", L"Then", L"\n",
        L"TextWindow", L".", L"WriteLine", L"(", L"\"Welcome back!\"", L")", L"\n",
        L"EndIf", L"\n",
        L"ctemp", L"=", L"32", L"\n",
        L"ftemp", L"=", L"89.6"
    });
    ASSERT_TRUE(tokens == expected);
}

// An incomplete string should be tokenized as a token that starts
// with a quote but does not end with one.
TEST(TokenizeTest, IncompleteString) {
    auto input = 
        L"str = \"Hello, world\n"
        L"TextWindow.WriteLine(str)";
    auto tokens = tokenize(input);
    auto expected = std::vector<std::wstring>({
        L"str", L"=", L"\"Hello, world", L"\n",
        L"TextWindow", L".", L"WriteLine", L"(", L"str", L")"
    });
    ASSERT_TRUE(tokens == expected);
}
TEST(TokenizeTest, IncompleteStringEOF) {
    auto input = 
        L"str = \"Hello, world";
    auto tokens = tokenize(input);
    auto expected = std::vector<std::wstring>({
        L"str", L"=", L"\"Hello, world"
    });
    ASSERT_TRUE(tokens == expected);
}