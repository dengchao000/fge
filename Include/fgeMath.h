/**
** Author:	邓超
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	
*
** Fly Game 2 [ Sparrow ]
** Copyright (C) 2005-, Fantasy Games
** Kernel functions
*
*  版本	 :	1.0
*  描述  :  数学相关
**/

#pragma once
#include "fgeutil.h"
namespace fge
{
	class  Math
	{
	public:
		static const real POS_INFINITY;
		static const real NEG_INFINITY;
		static const real PI;
		static const real TWO_PI;
		static const real HALF_PI;
		static const real fDeg2Rad;
		static const real fRad2Deg;
	};
	/*
	** Fast 1.0/sqrt(real) routine
	*/
	float InvSqrt(float x);

	class  fgeVector
	{
	public:
		real	x,y;

		fgeVector(real _x, real _y)	{ x=_x; y=_y; }
		fgeVector()						{ x=0; y=0; }

		fgeVector	operator- (const fgeVector &v) { return fgeVector(x-v.x, y-v.y); }
		fgeVector	operator+ (const fgeVector &v) { return fgeVector(x+v.x, y+v.y); }
		fgeVector	operator* (real scalar) { return fgeVector(x*scalar, y*scalar); }
		fgeVector&	operator-= (const fgeVector &v) { x-=v.x; y-=v.y; return *this; }
		fgeVector&	operator+= (const fgeVector &v) { x+=v.x; y+=v.y; return *this; }
		fgeVector&	operator*= (real scalar) { x*=scalar; y*=scalar; return *this; }
		fgeVector	operator- () { return fgeVector(-x, -y); }
		bool		operator== (const fgeVector &v) { return (x==v.x && y==v.y); }
		bool		operator!= (const fgeVector &v) { return (x!=v.x || y!=v.y); }

		real		Dot(const fgeVector *v) const { return x*v->x + y*v->y; }
		fgeVector*	Normalize() { real rc=InvSqrt(Dot(this)); x*=rc; y*=rc; return this; }
		real		Length() const { return sqrt(Dot(this)); }
		real		Angle(const fgeVector *v = 0) const;
		fgeVector*	Rotate(real a);
	};

	inline fgeVector operator* (const fgeVector &v, real s) { return fgeVector(s*v.x, s*v.y); }
	inline fgeVector operator* (real s, const fgeVector &v) { return fgeVector(s*v.x, s*v.y); }
	inline real operator^ (const fgeVector &v, const fgeVector &u) { return v.Angle(&u); }
	inline real operator% (const fgeVector &v, const fgeVector &u) { return v.Dot(&u); }


	////Random
	//inline unsigned int Rand( )
	//{
	//	static unsigned int g_seed = clock( );
	//	return g_seed=214013*g_seed+2531011;
	//}
	//inline float Random_Float(float Min, float Max)
	//{
	//	unsigned int g_seed = Rand( );
	//	//return min+g_seed*(1.0f/4294967295.0f)*(max-min);
	//	return Min+(g_seed>>16)*(1.0f/65535.0f)*(Max-Min);
	//}
	//inline int Random_Int(int Min, int Max)
	//{
	//	unsigned int g_seed = Rand( );
	//	return Min+(g_seed ^ g_seed>>15)%(Max-Min+1);
	//}
};