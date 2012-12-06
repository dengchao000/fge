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
	struct Point
	{ 
		Point(){ x =y = 0.0f; }
		Point(real _x, real _y ){ x=_x; y=_y; }
		real x,y;
	};
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

	class  Vector2
	{
	public:
		real	x,y;

		Vector2(real _x, real _y)	{ x=_x; y=_y; }
		Vector2()	{ x=0; y=0; }

		Vector2		operator- (const Vector2 &v) { return Vector2(x-v.x, y-v.y); }
		Vector2		operator+ (const Vector2 &v) { return Vector2(x+v.x, y+v.y); }
		Vector2		operator* (real scalar) { return Vector2(x*scalar, y*scalar); }
		Vector2&	operator-= (const Vector2 &v) { x-=v.x; y-=v.y; return *this; }
		Vector2&	operator+= (const Vector2 &v) { x+=v.x; y+=v.y; return *this; }
		Vector2&	operator*= (real scalar) { x*=scalar; y*=scalar; return *this; }
		Vector2		operator- () { return Vector2(-x, -y); }
		bool		operator== (const Vector2 &v) { return (x==v.x && y==v.y); }
		bool		operator!= (const Vector2 &v) { return (x!=v.x || y!=v.y); }

		real		Dot(const Vector2 *v) const { return x*v->x + y*v->y; }
		Vector2*	Normalize() { real rc=InvSqrt(Dot(this)); x*=rc; y*=rc; return this; }
		real		Length() const { return sqrt(Dot(this)); }
		real		Angle(const Vector2 *v = 0) const;
		Vector2*	Rotate(real a);
	};

	inline Vector2 operator* (const Vector2 &v, real s) { return Vector2(s*v.x, s*v.y); }
	inline Vector2 operator* (real s, const Vector2 &v) { return Vector2(s*v.x, s*v.y); }
	inline real operator^ (const Vector2 &v, const Vector2 &u) { return v.Angle(&u); }
	inline real operator% (const Vector2 &v, const Vector2 &u) { return v.Dot(&u); }


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