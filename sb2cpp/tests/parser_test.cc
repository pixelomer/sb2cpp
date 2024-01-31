#include <gtest/gtest.h>
#include <string>
#include "../libsb2cpp/parser.hpp"

using namespace sb2cpp;

TEST(ParserTest, AssignNumber) {
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

TEST(ParserTest, StdlibAssign) {
    auto input = "TextWindow.Title = \"Hello\"\n";
    Parser parser(input);

    auto result = parser.parse_next();
    ASSERT_NE(result, nullptr);
    
    auto assign = dynamic_cast<AST::StdlibAssign *>(result);
    ASSERT_NE(assign, nullptr);
    ASSERT_EQ(assign->class_name, "TextWindow");
    ASSERT_EQ(assign->property, "Title");

    auto value = dynamic_cast<AST::StringValue *>(assign->value);
    ASSERT_NE(value, nullptr);
    ASSERT_EQ(value->value, "Hello");

    delete result;

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);
}

TEST(ParserTest, AssignStdlibCall) {
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

TEST(ParserTest, AssignMultiplication) {
    auto input = "result = 10 * 6";
    Parser parser(input);

    auto result = dynamic_cast<AST::Assign *>(parser.parse_next());
    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->variable, "result");

    auto calc = dynamic_cast<AST::BinaryValueOp *>(result->value);
    ASSERT_NE(calc, nullptr);
    ASSERT_EQ(calc->op, AST::Multiply);

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

TEST(ParserTest, AssignMixedOperations) {
    auto input = "output = +-+-----5 -+-- (((10))) +--+ ((30 - 5) * (1 + Math.Log(10) / a * 30) / Math.Abs(-20))";
    Parser parser(input);

    auto result = parser.parse_next();
    ASSERT_NE(result, nullptr);

    auto assign = dynamic_cast<AST::Assign *>(result);
    ASSERT_NE(assign, nullptr);
    ASSERT_EQ(assign->variable, "output");

    auto add_group = dynamic_cast<AST::AddGroup *>(assign->value);
    ASSERT_NE(add_group, nullptr);
    ASSERT_EQ(add_group->elements.size(), 3);

    auto elem1 = add_group->elements[0];
    ASSERT_EQ(elem1.sign, AST::Positive);
    
    auto elem1value = dynamic_cast<AST::NumberValue *>(elem1.value);
    ASSERT_NE(elem1value, nullptr);
    ASSERT_EQ(elem1value->number, 5);

    auto elem2 = add_group->elements[1];
    ASSERT_EQ(elem2.sign, AST::Negative);
    
    auto elem2value = dynamic_cast<AST::NumberValue *>(elem2.value);
    ASSERT_NE(elem2value, nullptr);
    ASSERT_EQ(elem2value->number, 10);

    auto elem3 = add_group->elements[2];
    ASSERT_EQ(elem3.sign, AST::Positive);
    
    auto elem3value = dynamic_cast<AST::BinaryValueOp *>(elem3.value);
    ASSERT_NE(elem3value, nullptr);
    ASSERT_EQ(elem3value->op, AST::Divide);

    auto div_rvalue = dynamic_cast<AST::StdlibCall *>(elem3value->rvalue);
    ASSERT_NE(div_rvalue, nullptr);
    ASSERT_EQ(div_rvalue->class_name, "Math");
    ASSERT_EQ(div_rvalue->method_name, "Abs");
    ASSERT_EQ(div_rvalue->arguments.size(), 1);
    
    auto arg1 = dynamic_cast<AST::AddGroup *>(div_rvalue->arguments[0]);
    ASSERT_NE(arg1, nullptr);
    ASSERT_EQ(arg1->elements.size(), 1);
    ASSERT_EQ(arg1->elements[0].sign, AST::Negative);

    auto arg1value = dynamic_cast<AST::NumberValue *>(arg1->elements[0].value);
    ASSERT_NE(arg1value, nullptr);
    ASSERT_EQ(arg1value->number, 20);

    auto div_lvalue = dynamic_cast<AST::BinaryValueOp *>(elem3value->lvalue);
    ASSERT_NE(div_lvalue, nullptr);
    ASSERT_EQ(div_lvalue->op, AST::Multiply);
    
    auto lvalue = dynamic_cast<AST::AddGroup *>(div_lvalue->lvalue);
    ASSERT_NE(lvalue, nullptr);
    ASSERT_EQ(lvalue->elements.size(), 2);
    ASSERT_EQ(lvalue->elements[0].sign, AST::NoSignOp);
    ASSERT_EQ(lvalue->elements[1].sign, AST::Negative);

    delete result;

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);
}

TEST(ParserTest, AssignInvalidOperations) {
    Parser parser("test = 10 - * 20");
    ASSERT_THROW(parser.parse_next(), Parser::SyntaxError);

    Parser parser2("test = - 20 / 5 *");
    ASSERT_THROW(parser2.parse_next(), Parser::SyntaxError);
}

TEST(ParserTest, ArrayValue) {
    auto input = "var[1 + 2] = 3 + 4\nvar[5] = Math.Floor(var[3 + var5])";
    Parser parser(input);

    auto assign1 = dynamic_cast<AST::ArrayAssign *>(parser.parse_next());
    ASSERT_NE(assign1, nullptr);
    ASSERT_EQ(assign1->variable, "var");
    
    auto assign1key = dynamic_cast<AST::AddGroup *>(assign1->key);
    ASSERT_NE(assign1key, nullptr);
    ASSERT_EQ(assign1key->elements.size(), 2);

    auto elem1 = dynamic_cast<AST::NumberValue *>(assign1key->elements[0].value);
    ASSERT_NE(elem1, nullptr);
    ASSERT_EQ(elem1->number, 1);

    auto assign2 = dynamic_cast<AST::ArrayAssign *>(parser.parse_next());
    ASSERT_NE(assign2, nullptr);

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete assign1;
    delete assign2;
}

TEST(ParserTest, WhileLoop) {
    auto input = "a = 1\n"
        "b = -5\n"
        "While (a + b) < 10 And (5 - 6 > 2 Or (\"yes\" <> Test.Method(\"no\")))\n"
        "a = a + 1\n"
        "EndWhile";

    Parser parser(input);

    ASSERT_NE(parser.parse_next(), nullptr);
    ASSERT_NE(parser.parse_next(), nullptr);

    auto next = parser.parse_next();
    ASSERT_NE(next, nullptr);

    auto while_loop = dynamic_cast<AST::WhileLoop *>(next);
    ASSERT_NE(while_loop, nullptr);

    auto while_condition = dynamic_cast<AST::BinaryLogicOp *>(while_loop->condition);
    ASSERT_NE(while_condition, nullptr);
    ASSERT_EQ(while_condition->op, AST::And);

    auto lvalue = dynamic_cast<AST::BinaryCompareOp *>(while_condition->lvalue);
    ASSERT_NE(lvalue, nullptr);
    ASSERT_EQ(lvalue->op, AST::LessThan);

    auto rvalue = dynamic_cast<AST::BinaryLogicOp *>(while_condition->rvalue);
    ASSERT_NE(rvalue, nullptr);
    ASSERT_EQ(rvalue->op, AST::Or);

    auto statement = dynamic_cast<AST::Assign *>(while_loop->statement);
    ASSERT_NE(statement, nullptr);
    ASSERT_EQ(statement->variable, "a");

    auto value = dynamic_cast<AST::AddGroup *>(statement->value);
    ASSERT_NE(value, nullptr);

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete while_loop;
}

TEST(ParserTest, EmptyStatements) {
    auto input = "Sub test\nEndSub\nWhile 2 > 1\nEndWhile";
    Parser parser(input);

    auto sub = dynamic_cast<AST::Subroutine *>(parser.parse_next());
    ASSERT_NE(sub, nullptr);

    auto while_loop = dynamic_cast<AST::WhileLoop *>(parser.parse_next());
    ASSERT_NE(while_loop, nullptr);

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete sub;
    delete while_loop;
}

TEST(ParserTest, ForLoop) {
    auto input = "For i = 1 - 5 To 5 Step Math.floor(a)\n"
        "TextWindow.WriteLine(i)\n"
        "EndFor\n";
    Parser parser(input);

    auto for_loop = dynamic_cast<AST::ForLoop *>(parser.parse_next());
    ASSERT_NE(for_loop, nullptr);
    ASSERT_NE(for_loop->step_value, nullptr);

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete for_loop;
}

TEST(ParserTest, Goto) {
    auto input = "Sub main\n"
        "i = 0\n"
        "Loop:\n"
        "TextWindow.WriteLine(i)\n"
        "i = i + 1\n"
        "Goto Loop\n"
        "EndSub\n"
        "main()\n";
    
    Parser parser(input);
    auto subroutine = dynamic_cast<AST::Subroutine *>(parser.parse_next());
    ASSERT_NE(subroutine, nullptr);
    ASSERT_EQ(subroutine->subroutine_name, "main");

    auto group = dynamic_cast<AST::StatementGroup *>(subroutine->contents);
    ASSERT_NE(group, nullptr);
    ASSERT_EQ(group->statements.size(), 5);

    auto label = dynamic_cast<AST::GotoLabel *>(group->statements[1]);
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->name, "Loop");

    auto goto_statement = dynamic_cast<AST::GotoStatement *>(group->statements[4]);
    ASSERT_NE(goto_statement, nullptr);
    ASSERT_EQ(goto_statement->label, "Loop");

    auto sub_call = dynamic_cast<AST::SubroutineCall *>(parser.parse_next());
    ASSERT_NE(sub_call, nullptr);
    ASSERT_EQ(sub_call->subroutine_name, "main");

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete sub_call;
    delete subroutine;
}

TEST(ParserTest, IfStatement) {
    auto input = "If Math.Floor(a) > 5 Then\n"
        "TextWindow.WriteLine(\"wow\")\n"
        "a = a / 5\n"
        "ElseIf a < 0 Then\n"
        "Else\n"
        "Program.End()\n"
        "EndIf\n"
        "a = 1 / a\n"
        "If a = 1 Then\n"
        "a = 0\n"
        "EndIf\n";
        
    Parser parser(input);
    auto if_statement = dynamic_cast<AST::IfStatement *>(parser.parse_next());
    ASSERT_NE(if_statement, nullptr);

    auto assign = dynamic_cast<AST::Assign *>(parser.parse_next());
    ASSERT_NE(assign, nullptr);

    auto if_statement2 = dynamic_cast<AST::IfStatement *>(parser.parse_next());
    ASSERT_NE(if_statement2, nullptr);

    auto end = parser.parse_next();
    ASSERT_EQ(end, nullptr);

    delete assign;
    delete if_statement;
}