#include <gtest/gtest.h>
#include <string>
#include "../lib/parse.hpp"

using namespace sb2cpp;

/**** Valid/Expected inputs ****/

TEST(ParseTest, AssignNumber) {
    auto input = "num = 5";
    Parser parser(input);

    auto result = dynamic_cast<AST::Assign *>(parser.parse_next());
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->variable, "num");

    auto value = dynamic_cast<AST::NumberValue *>(result->value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->number, 5);

    delete result;

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);
}

TEST(ParseTest, AssignStdlibCall) {
    auto input = "num = Math.Remainder(10, 6)\n\n\n";
    Parser parser(input);

    auto result = dynamic_cast<AST::Assign *>(parser.parse_next());
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->variable, "num");

    auto stdlib_call = dynamic_cast<AST::StdlibCall *>(result->value);
    ASSERT_NE(stdlib_call, nullptr);
    ASSERT_EQ(stdlib_call->class_name, "Math");
    ASSERT_EQ(stdlib_call->method_name, "Remainder");
    ASSERT_EQ(stdlib_call->arguments.size(), 2);
    
    auto arg1 = dynamic_cast<AST::NumberValue *>(stdlib_call->arguments[0]);
    ASSERT_NE(arg1, nullptr);
    ASSERT_EQ(arg1->number, 10);
    
    auto arg2 = dynamic_cast<AST::NumberValue *>(stdlib_call->arguments[1]);
    ASSERT_NE(arg2, nullptr);
    ASSERT_EQ(arg2->number, 6);

    delete result;

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);
}

TEST(ParseTest, AssignMultiplication) {
    auto input = "result = 10 * 6";
    Parser parser(input);

    auto result = dynamic_cast<AST::Assign *>(parser.parse_next());
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->variable, "result");

    auto calc = dynamic_cast<AST::MultiplyValue *>(result->value);
    ASSERT_NE(calc, nullptr);

    auto lvalue = dynamic_cast<AST::NumberValue *>(calc->lvalue);
    ASSERT_NE(lvalue, nullptr);
    ASSERT_EQ(lvalue->number, 10);

    auto rvalue = dynamic_cast<AST::NumberValue *>(calc->rvalue);
    ASSERT_NE(rvalue, nullptr);
    ASSERT_EQ(rvalue->number, 6);

    delete result;

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);
}