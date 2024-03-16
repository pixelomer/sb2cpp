#pragma once

#include "macros.hpp"
#include <sys/time.h>

SB_CLASS(Clock)

SB_PRIVATE
struct timeval time() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp;
}

SB_PRIVATE
struct tm timestamp() {
    time_t curtime = time().tv_sec;
    struct tm t = *localtime(&curtime);
    return t;
}

SB_VALUE_GETTER(Time) () {
    int hr = _GetHour();
    int min = _GetMinute();
    int sec = _GetSecond();
    std::string tm =
        ((hr < 10) ? "0" : "") +
        std::to_string(hr) + ":" +
        ((min < 10) ? "0" : "") +
        std::to_string(min) + ":" +
        ((sec < 10) ? "0" : "") +
        std::to_string(sec);
    return tm;
}

SB_VALUE_GETTER(Date) () {
    int year = _GetYear();
    int month = _GetMonth();
    int day = _GetDay();
    std::string date =
        std::to_string(year) + "-" +
        ((month < 10) ? "0" : "") +
        std::to_string(month) + "-" +
        ((day < 10) ? "0" : "") +
        std::to_string(day);
    return date;
}

SB_VALUE_GETTER(Year) () {
    return 1900 + timestamp().tm_year;
}

SB_VALUE_GETTER(Month) () {
    return 1 + timestamp().tm_mon;
}

SB_VALUE_GETTER(Day) () {
    return timestamp().tm_mday;
}

SB_VALUE_GETTER(WeekDay) () {
    return 1 + timestamp().tm_wday;
}

SB_VALUE_GETTER(Hour) () {
    return timestamp().tm_hour;
}

SB_VALUE_GETTER(Minute) () {
    return timestamp().tm_min;
}

SB_VALUE_GETTER(Second) () {
    return timestamp().tm_sec;
}

SB_VALUE_GETTER(Millisecond) () {
    return (int)(time().tv_usec / 1000);
}

SB_VALUE_GETTER(ElapsedMilliseconds) () {
    auto tm = time();
    return (double)(tm.tv_sec * 1000 + tm.tv_usec / 1000);
}


SB_CLASS_END