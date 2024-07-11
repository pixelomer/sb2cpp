#pragma once

#include "macros.hpp"
#include <cstdlib>
#include <chrono>
#include <thread>
#include <climits>
#include <unistd.h>
#include <errno.h>

SB_CLASS(Program)

#warning Program.ArgumentCount not implemented
SB_VALUE_GETTER(ArgumentCount) () {
    return 0;
}

SB_VALUE_GETTER(Directory) () {
    char path[PATH_MAX];
    if (getcwd(path, PATH_MAX) == NULL) {
        throw std::runtime_error("getcwd() failed: " +
            std::string(strerror(errno)));
    }
    return std::string(path);
}

#warning Program.GetArgument not implemented
SB_METHOD_1(GetArgument) (int idx) {
    return "";
}

SB_METHOD_0(End) () {
    std::exit(0);
    return SB_VOID;
}

SB_METHOD_1(Delay) (double milli) {
    std::this_thread::sleep_for(std::chrono::milliseconds((long long)milli));
    return SB_VOID;
}

SB_CLASS_END