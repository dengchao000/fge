/** 
*
** Author:	µË³¬
** QQ:		23427470
** Mail:    sen.night@hotmail.com
** Time:	2007-11-21
*
** Fly Game 2
** Copyright (C) 2005-2007, Fantasy Games
** Kernel functions
** 
** Description: ÑÕÉ«
*
*/
#pragma once

#include "fgeutil.h"
namespace fge
{
	inline void ColorClamp(float &x) { if(x<0.0f) x=0.0f; if(x>1.0f) x=1.0f; }

	class fgeColor
	{
	public:
		float		r,g,b,a;

		fgeColor(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }
		fgeColor(DWORD col) { SetHWColor(col); }
		fgeColor() { r=g=b=a=0; }

		fgeColor	operator- (const fgeColor &c) { return fgeColor(r-c.r, g-c.g, b-c.b, a-c.a); }
		fgeColor	operator+ (const fgeColor &c) { return fgeColor(r+c.r, g+c.g, b+c.b, a+c.a); }
		fgeColor	operator* (float scalar) { return fgeColor(r*scalar, g*scalar, b*scalar, a*scalar); }
		fgeColor&	operator-= (const fgeColor &c) { r-=c.r; g-=c.g; b-=c.b; a-=c.a; return *this; }
		fgeColor&	operator+= (const fgeColor &c) { r+=c.r; g+=c.g; b+=c.b; a+=c.a; return *this; }
		fgeColor&	operator*= (float scalar) { r*=scalar; g*=scalar; b*=scalar; a*=scalar; return *this; }
		bool		operator== (const fgeColor &c) { return (r==c.r && g==c.g && b==c.b && a==c.a); }
		bool		operator!= (const fgeColor &c) { return (r!=c.r || g!=c.g || b!=c.b || a!=c.a); }

		void		Clamp() { ColorClamp(r); ColorClamp(g); ColorClamp(b); ColorClamp(a); }
		void		SetHWColor(DWORD col) { a=(col>>24)/255.0f; r=((col>>16) & 0xFF)/255.0f; g=((col>>8) & 0xFF)/255.0f; b=(col & 0xFF)/255.0f; }
		DWORD		GetHWColor() const { return (DWORD(a*255.0f)<<24) + (DWORD(r*255.0f)<<16) + (DWORD(g*255.0f)<<8) + DWORD(b*255.0f); }
	};

	inline fgeColor operator* (const fgeColor &c, float s) { return fgeColor(s*c.r, s*c.g, s*c.b, s*c.a); }
	inline fgeColor operator* (float s, const fgeColor &c) { return fgeColor(s*c.r, s*c.g, s*c.b, s*c.a); }
}