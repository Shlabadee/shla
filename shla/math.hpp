#ifndef __SHLA_MATH_INCLUDED
#define __SHLA_MATH_INCLUDED

#include <cmath>

namespace shla
{
	const double PI = 3.14159265359;
	const double PI2 = PI * 2.0;
	const double HPI = PI / 2;
	const double QPI = PI / 4;
	const double PI15 = PI * 1.5;
	const double PIU2 = 2 / PI;
	
	double fmod(double a, double b)
	{
		return a - b * floor(a / b);
	}
}

#endif