#include "stdafx.h"
#include "fgeMath.h"

namespace fge
{
	const real Math::POS_INFINITY = std::numeric_limits<real>::infinity();	//正无穷大
	const real Math::NEG_INFINITY = -std::numeric_limits<real>::infinity();	//负无穷大
	const real Math::PI = real( 4.0 * atan( 1.0 ) );
	const real Math::TWO_PI = real( 2.0 * PI );
	const real Math::HALF_PI = real( 0.5 * PI );
	const real Math::fDeg2Rad = PI / real(180.0);
	const real Math::fRad2Deg = real(180.0) / PI;

	float InvSqrt(float x)
	{
		union
		{
			int intPart;
			float floatPart;
		} convertor;

		convertor.floatPart = x;
		convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
		return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
	}

	real fgeVector::Angle(const fgeVector *v) const
	{
		if(v)
		{
			fgeVector s=*this, t=*v;

			s.Normalize(); t.Normalize();
			return acos(s.Dot(&t));
		}
		else return atan2(y, x);
	}

	fgeVector *fgeVector::Rotate(real a)
	{
		fgeVector v;

		v.x=x*cos(a) - y*sin(a);
		v.y=x*sin(a) + y*cos(a);

		x=v.x; y=v.y;

		return this;
	}



}