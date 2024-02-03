#include <gtest/gtest.h>
#include "../stdlib/SmallBasic.hpp"

using namespace SmallBasic;

TEST(StdlibTest, TextClass) {
    Obj str = "Lorem ipsum dolor sit amet.";

    auto len = Text::GetLength(str);
    ASSERT_EQ(len, 27);

    auto upper = Text::ConvertToUpperCase(str);
    ASSERT_EQ(upper, "LOREM IPSUM DOLOR SIT AMET.");

    auto lower = Text::ConvertToLowerCase(str);
    ASSERT_EQ(lower, "lorem ipsum dolor sit amet.");

    auto idx = Text::GetIndexOf(str, "ipsum");
    ASSERT_EQ(idx, 7);

    idx = Text::GetIndexOf(str, "foo");
    ASSERT_EQ(idx, 0);

    auto cond = Text::IsSubText(str, "sit amet");
    ASSERT_EQ(cond, SB_TRUE);

    cond = Text::EndsWith(str, ".");
    ASSERT_EQ(cond, SB_TRUE);

    cond = Text::EndsWith(str, "amet");
    ASSERT_EQ(cond, SB_FALSE);

    cond = Text::StartsWith(str, "Lorem");
    ASSERT_EQ(cond, SB_TRUE);

    cond = Text::StartsWith(str, "lorem");
    ASSERT_EQ(cond, SB_FALSE);

    auto substr = Text::GetSubText(str, 2, 4);
    ASSERT_EQ(substr, "orem");

    substr = Text::GetSubTextToEnd(str, 13);
    ASSERT_EQ(substr, "dolor sit amet.");

    str = Text::Append(Obj("10"), Obj("20"));
    ASSERT_EQ(str, "1020");
}

TEST(StdlibTest, StackClass) {
    auto foo = Obj("foo");
    auto bar = Obj("bar");

    ASSERT_EQ(Stack::GetCount(foo), 0);
    ASSERT_EQ(Stack::GetCount(bar), 0);

    Stack::PushValue(foo, 5000);
    Stack::PushValue(foo, "hello");
    Stack::PushValue(foo, -1337);

    Stack::PushValue(bar, "hi");
    Stack::PushValue(bar, -999);

    ASSERT_EQ(Stack::GetCount(foo), 3);
    ASSERT_EQ(Stack::PopValue(foo), -1337);
    ASSERT_EQ(Stack::GetCount(foo), 2);
    ASSERT_EQ(Stack::PopValue(foo), "hello");

    ASSERT_EQ(Stack::GetCount(bar), 2);
    ASSERT_EQ(Stack::PopValue(bar), -999);
    ASSERT_EQ(Stack::GetCount(bar), 1);
    ASSERT_EQ(Stack::PopValue(bar), "hi");
    ASSERT_EQ(Stack::GetCount(bar), 0);

    ASSERT_EQ(Stack::GetCount(foo), 1);
    ASSERT_EQ(Stack::PopValue(foo), 5000);
    ASSERT_EQ(Stack::GetCount(foo), 0);
}