#include <gtest/gtest.h>
#include <string>
#include "../lib/source.hpp"

using namespace sb2cpp;

TEST(SourceTest, SimpleSourceCode) {
    auto input =
        "Start:\n"
        "Result = 0\n"
        "Sub Add\n"
        "RESULT = A + b\n"
        "EndSub\n"
        "Sub Mult\n"
        "result = a * B\n"
        "EndSub\n"
        "a = 10\n"
        "b = 20\n"
        "add()\n"
        "mult()\n"
        "Goto start\n"
        "Goto NEXT\n"
        "next:\n";
    Source source(input);
    
    ASSERT_EQ(source.subroutines.size(), 2);
    ASSERT_EQ(source.subroutines.at("Add").cname, "Add");
    ASSERT_EQ(source.subroutines.at("Mult").cname, "Mult");

    ASSERT_EQ(source.variables.size(), 3);
    ASSERT_EQ(source.variables.at("A").cname, "A");
    ASSERT_EQ(source.variables.at("b").cname, "b");
    ASSERT_EQ(source.variables.at("Result").cname, "Result");

    ASSERT_EQ(source.goto_labels.size(), 2);
    ASSERT_EQ(source.goto_labels.at("Start").cname, "Start");
    ASSERT_EQ(source.goto_labels.at("NEXT").cname, "NEXT");
}

TEST(SourceTest, SubroutineDefinedTwice) {
    auto input = "Sub add\n"
        "result = a + b\n"
        "EndSub\n"
        "Sub ADD\n"
        "EndSub\n"
        "a = 0\n"
        "b = 0\n"
        "Add()";
    
    ASSERT_THROW(Source source(input), Source::SourceError);
}

TEST(SourceTest, UndefinedVariable) {
    auto input = "TextWindow.WriteLine(str)";

    ASSERT_THROW(Source source(input), Source::SourceError);
}