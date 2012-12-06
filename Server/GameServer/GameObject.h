#pragma once

class GameObject :public fge::SharedObject
{
public:
	GameObject( ){};
	virtual ~GameObject( ){};
};
class Region: public GameObject
{

};
class Actor: public GameObject
{
public:
	Actor( );
	virtual ~Actor( );

	void	MoveTo( uint x, uint y );
	void	UpdatePos( uint dt );
public:
	void	Attack( Actor* pobject );
private:
	fge::Point mCurPos;			//当前位置
	fge::Vector2 mSpeed;		//速度
	fge::Vector2 mDir;			//方向
	uint		mBeginMoveTime;	//开始移动时间戳
	fge::List<Point> mMovePath;	//移动路径
};
class Player: public Actor
{
public:
	SRoleData	playerData;	
};
class Monster: public Actor
{
public:
};
class Item: public GameObject
{
public:
};
class MoveManager :public fge::SharedObject
{
public:
	void	OnUpdate( ){};
};
