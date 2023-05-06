#ifndef _SMALLBASIC_MATH_H
#define _SMALLBASIC_MATH_H

#include "../common/Mixed.hpp"
#include <cmath>

namespace SmallBasic {
	namespace Std {
		class Math {
		public:
			static Mixed _GetPI() {
				return M_PI;
			}
			static Mixed Abs(Number number) { 
				return fabs(number);
			}
			static Mixed Ceiling(Number number) { 
				return ceill(number);
			}
			static Mixed Floor(Number number) { 
				return floorl(number);
			}
			static Mixed NaturalLog(Number number) { 
				return logl(number);
			}
			static Mixed Log(Number number) { 
				return log10l(number);
			}
			static Mixed Cos(Number number) { 
				return cosl(number);
			}
			static Mixed Sin(Number number) { 
				return sinl(number);
			}
			static Mixed Tan(Number number) { 
				return tanl(number);
			}
			static Mixed ArcSin(Number number) { 
				return asinl(number);
			}
			static Mixed ArcCos(Number number) { 
				return acosl(number);
			}
			static Mixed ArcTan(Number number) { 
				return atanl(number);
			}
			static Mixed GetDegrees(Number number) {
				return number * 180.0L / M_PI;
			}
			static Mixed GetRadians(Number number) {
				return number * M_PI / 180.0L;
			}
			static Mixed SquareRoot(Number number) { 
				return sqrtl(number);
			}
			static Mixed Power(Number a, Number b) {
				return powl(a, b);
			}
			static Mixed Round(Number number) { 
				return roundl(number);
			}
			static Mixed Max(Number a, Number b) {
				return (a > b) ? a : b;
			}
			static Mixed Min(Number a, Number b) {
				return (a > b) ? b : a;
			}
			static Mixed Remainder(Number a, Number b) {
				return remainderl(a, b);
			}
			static Mixed GetRandomNumber(Number max) {
				return (random() % (long)max) + 1;
			}
			static Mixed DoubleToDecimal(Number number) {
				return (long long)number;
			}
		};
	}
}

#endif