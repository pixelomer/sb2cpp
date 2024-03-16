#pragma once

#include "macros.hpp"
#include <math.h>
#include <cstdlib>
#include <random>

SB_CLASS(Math)

SB_VALUE_GETTER(Pi) () {
    return M_PI;
}

SB_METHOD_1(Abs) (double num) {
    return std::abs(num);
}

SB_METHOD_1(Ceiling) (double num) {
    return std::ceil(num);
}

SB_METHOD_1(Floor) (double num) {
    return std::floor(num);
}

SB_METHOD_1(NaturalLog) (double num) {
    return std::log(num);
}

SB_METHOD_1(Log) (double num) {
    return std::log10(num);
}

SB_METHOD_1(Cos) (double num) {
    return std::cos(num);
}

SB_METHOD_1(Sin) (double num) {
    return std::sin(num);
}

SB_METHOD_1(Tan) (double num) {
    return std::tan(num);
}

SB_METHOD_1(ArcSin) (double num) {
    return std::asin(num);
}

SB_METHOD_1(ArcCos) (double num) {
    return std::acos(num);
}

SB_METHOD_1(ArcTan) (double num) {
    return std::atan(num);
}

SB_METHOD_1(GetDegrees) (double rad) {
    return rad * 180.0 / M_PI;
}

SB_METHOD_1(GetRadians) (double deg) {
    return deg * M_PI / 180.0;
}

SB_METHOD_1(SquareRoot) (double num) {
    return std::sqrt(num);
}

SB_METHOD_2(Power) (double base, double exp) {
    return std::pow(base, exp);
}

SB_METHOD_1(Round) (double num) {
    return std::round(num);
}

SB_METHOD_2(Max) (double a, double b) {
    return (a > b) ? a : b;
}

SB_METHOD_2(Min) (double a, double b) {
    return (a > b) ? b : a;
}

SB_METHOD_2(Remainder) (double dividend, double divisor) {
    return std::remainder(dividend, divisor);
}

SB_METHOD_1(GetRandomNumber) (double max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, max);
    return (long)dist(rng);
}

SB_METHOD_1(DoubleToDecimal) (double num) {
    // not supported
    return num;
}

SB_CLASS_END