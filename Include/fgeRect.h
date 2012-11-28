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
		//��վ��οռ�
		void    Clear() {bClean=true;}
		//�жϵ�ǰ���οռ��С�Ƿ�Ϊ��;
		bool    IsClean() const {return bClean;}
		//��ʼ�����οռ�
		void	Set(float _x1, float _y1, float _x2, float _y2) { x1=_x1; x2=_x2; y1=_y1; y2=_y2; bClean=false; }
		//��x,yΪ����,rΪ�뾶��ʼ�����οռ�
		void	SetRadius(float x, float y, float r) { x1=x-r; x2=x+r; y1=y-r; y2=y+r; bClean=false; }
		//�����οռ���չ��x,y
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
		//�ж�һ�����Ƿ��ھ�����
		bool	TestPoint(float x, float y) const
		{
			if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;
			return false;
		}
		//�ж����������Ƿ��ཻ
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