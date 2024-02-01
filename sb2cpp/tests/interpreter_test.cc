#include <gtest/gtest.h>
#include <string>
#include "../libsb2cpp/interpreter.hpp"

using namespace sb2cpp;
using namespace SmallBasic;

TEST(InterpreterTest, ForLoop) {
    auto code =
        "Sub main\n"
        "a = -3\n"
        "For i = 5 To 10\n"
        "A = A + I\n"
        "EndFor\n"
        "EndSub\n"
        "main()\n"
        "TextWindow.WriteLine(\"Hello, world!\")";
    Interpreter interpreter(code);
    interpreter.run();

    Obj a = interpreter.var("a");
    Obj i = interpreter.var("i");

    ASSERT_EQ(a, 42);
}