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
	fge::Point mCurPos;			//��ǰλ��
	fge::Vector2 mSpeed;		//�ٶ�
	fge::Vector2 mDir;			//����
	uint		mBeginMoveTime;	//��ʼ�ƶ�ʱ���
	fge::List<Point> mMovePath;	//�ƶ�·��
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
