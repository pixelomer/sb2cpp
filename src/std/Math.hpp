#include "Types.hpp"
#include <cmath>

namespace SmallBasic {
	class Math {
	public:
		static Number _GetPI() {
			return M_PI;
		}
		static Number Abs(Number number) { 
			return fabs(number);
		}
		static Number Ceiling(Number number) { 
			return ceill(number);
		}
		static Number Floor(Number number) { 
			return floorl(number);
		}
		static Number NaturalLog(Number number) { 
			return logl(number);
		}
		static Number Log(Number number) { 
			return log10l(number);
		}
		static Number Cos(Number number) { 
			return cosl(number);
		}
		static Number Sin(Number number) { 
			return sinl(number);
		}
		static Number Tan(Number number) { 
			return tanl(number);
		}
		static Number ArcSin(Number number) { 
			return asinl(number);
		}
		static Number ArcCos(Number number) { 
			return acosl(number);
		}
		static Number ArcTan(Number number) { 
			return atanl(number);
		}
		static Number GetDegrees(Number number) {
			return number * 180.0L / M_PI;
		}
		static Number GetRadians(Number number) {
			return number * M_PI / 180.0L;
		}
		static Number SquareRoot(Number number) { 
			return sqrtl(number);
		}
		static Number Power(Number a, Number b) {
			return powl(a, b);
		}
		static Number Round(Number number) { 
			return roundl(number);
		}
		static Number Max(Number a, Number b) {
			return (a > b) ? a : b;
		}
		static Number Min(Number a, Number b) {
			return (a > b) ? b : a;
		}
		static Number Remainder(Number a, Number b) {
			return remainderl(a, b);
		}
		static Number GetRandomNumber(Number max) {
			return (random() % (long)max) + 1;
		}
		static Number DoubleToDecimal(Number number) {
			return (long long)number;
		}
	};
}