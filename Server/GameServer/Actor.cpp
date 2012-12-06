#include "stdafx.h"
#include "GameObject.h"

Actor::Actor()
{
	mBeginMoveTime = 0;
};
Actor::~Actor(){};

void Actor::MoveTo( uint x, uint y )
{
	mMovePath.PushBack(fge::Point((float)x,(float)y));
	mBeginMoveTime = 0;
}
void Actor::UpdatePos( uint dt )
{
	mBeginMoveTime += dt;
}