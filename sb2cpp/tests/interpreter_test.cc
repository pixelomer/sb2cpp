#include <gtest/gtest.h>
#include <string>
#include "../libsb2cpp/interpreter.hpp"
#include "../stdlib/Runtime.hpp"

using namespace sb2cpp;
using namespace SmallBasic;

static std::string output;

Interpreter make_interpreter(std::string const& code) {
    Interpreter interpreter(code);
    auto &cls = Runtime::get_class("TextWindow");
    auto &writeline = cls.get_method("WriteLine");
    auto &write = cls.get_method("Write");
    output.clear();

    writeline.handler = [](std::vector<Obj> const& args) -> Obj {
        output += (std::string)args[0] + "\n";
        return Obj();
    };
    write.handler = [](std::vector<Obj> const& args) -> Obj {
        output += (std::string)args[0];
        return Obj();
    };

    return interpreter;
}

static Obj foo;
static std::function<void()> foo_callback;

static void foo_setter(Obj const& obj) {
    foo = obj * Obj(2);
}
static Obj foo_getter() {
    if (foo_callback != nullptr) {
        foo_callback();
    }
    return foo / Obj(3);
}
static Obj bar_call(std::vector<Obj> const& args) {
    return foo * args[0];
}
static void callback_setter(std::function<void()> cb) {
    foo_callback = cb;
}

TEST(InterpreterTest, ForLoop) {
    auto code =
        "Sub main\n"
        "a = -3\n"
        "For i = 5 To 10\n"
        "A = A + I\n"
        "EndFor\n"
        "EndSub\n"
        "main()\n";
    auto runner = make_interpreter(code);
    runner.run();

    Obj a = runner.var("a");
    Obj i = runner.var("i");

    ASSERT_EQ(a, 42);
    ASSERT_EQ(i, 11);
}

TEST(InterpreterTest, StdlibCalls) {
    Class cls("StdlibCalls");
    Property foo_prop("Foo", foo_getter, foo_setter, nullptr);
    Property callback_prop("Callback", nullptr, nullptr, callback_setter);
    Method bar_method("Bar", bar_call, 1, true);
    cls.register_method(bar_method);
    cls.register_property(foo_prop);
    cls.register_property(callback_prop);
    Runtime::register_class(cls);

    auto code =
        "Sub callback\n"
        "TextWindow.Write(\"!\")\n"
        "EndSub\n"
        "StdlibCalls.Foo = 150\n"
        "StdlibCalls.Callback = callback\n"
        "TextWindow.WriteLine(StdlibCalls.Foo)\n"
        "TextWindow.Write(StdlibCalls.Bar(3))\n";
    
    Interpreter interpreter = make_interpreter(code);
    interpreter.run();

    ASSERT_EQ(foo, 300);
    ASSERT_EQ(output, "!100\n900");
}