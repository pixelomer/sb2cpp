#include <gtest/gtest.h>
#include <string>
#include "../stdlib/Obj.hpp"

using namespace SmallBasic;

TEST(ObjectTest, DifferentNumberRepresentations) {
    Obj value1("100.500");
    Obj value2(100.50);
    Obj value3("100.5");

    ASSERT_EQ((std::string)value1, "100.500");
    ASSERT_EQ((std::string)value2, "100.5");
    ASSERT_EQ((std::string)value3, "100.5");

    ASSERT_DOUBLE_EQ((double)value1, 100.5);
    ASSERT_DOUBLE_EQ((double)value2, 100.5);
    ASSERT_DOUBLE_EQ((double)value3, 100.5);

    ASSERT_EQ(value1, value2);
    ASSERT_EQ(value2, value3);
    ASSERT_EQ(value1, value3);

    Obj value4("50.50");
    ASSERT_EQ((std::string)value4, "50.50");
    ASSERT_DOUBLE_EQ((double)value4, 50.5);
    ASSERT_GT(value3, value4);
}

TEST(ObjectTest, Arrays) {
    Obj array("0=apple;1=pear;");

    ASSERT_EQ(array[0], "apple");
    ASSERT_EQ(array[1], "pear");
    ASSERT_EQ(array[2], "");

    array[0] = "banana";
    array[1] = "apple";
    array[Obj("bee")] = "honey";

    ASSERT_EQ(array[0], "banana");
    ASSERT_EQ(array[1], "apple");
    ASSERT_EQ(array[Obj("bee")], "honey");
    ASSERT_EQ(array, "0=banana;1=apple;bee=honey;");

    array.array_delete(1.0);
    ASSERT_EQ(array, "0=banana;bee=honey;");
}

TEST(ObjectTest, EscapedArrays) {
    Obj array = "";

    array[Obj("\\=.=\\")] = "\\;-;\\";
    ASSERT_EQ(array, "\\\\\\=.\\=\\\\=\\\\\\;-\\;\\\\;");
    ASSERT_EQ(array[Obj("\\=.=\\")], "\\;-;\\");
    
    array.array_delete("\\=.=\\");
    ASSERT_EQ(array, "");
}

TEST(ObjectTest, Compare) {
    Obj str1 = "hello";
    Obj str2 = "John";
    Obj num1 = 500.0;
    Obj num2 = -25.0;

    ASSERT_TRUE(num2 <= num1);
    ASSERT_TRUE(num1 >= num2);
    ASSERT_FALSE(str1 < str2); // (0 < 0) == false
    ASSERT_FALSE(str1 > str2); // (0 > 0) == false
    ASSERT_FALSE(str1 == str2); // ("hello" == "John") == false
    ASSERT_TRUE(str1 != str2);
}