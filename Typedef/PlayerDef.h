#pragma once
#define ROLE_NAME_SIZE		20
#define TONG_NAME_SIZE		20
#define TITLE_NAME_SIZE		20

struct SRoleData
{
	DWORD	version;				// 版本号，用于二进制数据扩展！  
	DWORD	playerID;				// 玩家编号
	CHAR	name[ROLE_NAME_SIZE];	// 名字
	BYTE	worldID;				// 世界编号
	BYTE    heroID;					// 英雄ID
	WORD	level;					// 玩家的等级
	DWORD	exp;					// 玩家的当前经验值
	DWORD	leaveTime;				// 玩家离线时的时间

	DWORD	money;					// 玩家当前所携带的银两
	DWORD	tongID;					// 帮派ID

    DWORD	maxHP;					// 生命最大值
    WORD	maxMP;					// 内力最大值
    WORD	maxSP;					// 体力最大值
	DWORD	curHP;					// 当前生命
    WORD	curMP;					// 当前内力
    WORD	curSP;					// 当前体力

	// 地图相关D
    WORD	regionID;				// 当前所在场景的ID
    WORD	tileX;					// 当前所在地图的X坐标（Tile）
    WORD	tileY;					// 当前所在地图的Y坐标（Tile）
	WORD	serverId;				// 所在服务器编号

	CHAR	titleName[TITLE_NAME_SIZE];		// 自定的称号
	CHAR	mateName[ROLE_NAME_SIZE];		// 夫(妻)的名字
};
