#pragma once

#include <math.h>
namespace fge
{

	class Rect
	{
	public:
		float	x1, y1, x2, y2;

		Rect(float _x1, float _y1, float _x2, float _y2) {x1=_x1; y1=_y1; x2=_x2; y2=_y2; bClean=false; }
		Rect() {bClean=true;}
		//清空矩形空间
		void    Clear() {bClean=true;}
		//判断当前矩形空间大小是否为空;
		bool    IsClean() const {return bClean;}
		//初始化矩形空间
		void	Set(float _x1, float _y1, float _x2, float _y2) { x1=_x1; x2=_x2; y1=_y1; y2=_y2; bClean=false; }
		//以x,y为中心,r为半径初始化矩形空间
		void	SetRadius(float x, float y, float r) { x1=x-r; x2=x+r; y1=y-r; y2=y+r; bClean=false; }
		//将矩形空间扩展至x,y
		void	Encapsulate(float x, float y)
		{
			if(bClean) {
				x1=x2=x;
				y1=y2=y;
				bClean=false;
			} else {
				if(x<x1) x1=x;
				if(x>x2) x2=x;
				if(y<y1) y1=y;
				if(y>y2) y2=y;
			}
		}
		//判断一个点是否在矩形中
		bool	TestPoint(float x, float y) const
		{
			if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;
			return false;
		}
		//判断两个矩形是否相交
		bool	Intersect(const Rect *rect) const
		{
			if(fabs(x1+x2 - rect->x1-rect->x2) < (x2-x1 + rect->x2-rect->x1)) {
				if(fabs(y1+y2 - rect->y1-rect->y2) < (y2-y1 + rect->y2-rect->y1)) return true;
			}
			return false;
		}

	private:
		bool	bClean;
	};
}