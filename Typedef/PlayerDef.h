#pragma once
#define ROLE_NAME_SIZE		20
#define TONG_NAME_SIZE		20
#define TITLE_NAME_SIZE		20

struct SRoleData
{
	DWORD	version;				// �汾�ţ����ڶ�����������չ��  
	DWORD	playerID;				// ��ұ��
	CHAR	name[ROLE_NAME_SIZE];	// ����
	BYTE	worldID;				// ������
	BYTE    heroID;					// Ӣ��ID
	WORD	level;					// ��ҵĵȼ�
	DWORD	exp;					// ��ҵĵ�ǰ����ֵ
	DWORD	leaveTime;				// �������ʱ��ʱ��

	DWORD	money;					// ��ҵ�ǰ��Я��������
	DWORD	tongID;					// ����ID

    DWORD	maxHP;					// �������ֵ
    WORD	maxMP;					// �������ֵ
    WORD	maxSP;					// �������ֵ
	DWORD	curHP;					// ��ǰ����
    WORD	curMP;					// ��ǰ����
    WORD	curSP;					// ��ǰ����

	// ��ͼ���D
    WORD	regionID;				// ��ǰ���ڳ�����ID
    WORD	tileX;					// ��ǰ���ڵ�ͼ��X���꣨Tile��
    WORD	tileY;					// ��ǰ���ڵ�ͼ��Y���꣨Tile��
	WORD	serverId;				// ���ڷ��������

	CHAR	titleName[TITLE_NAME_SIZE];		// �Զ��ĳƺ�
	CHAR	mateName[ROLE_NAME_SIZE];		// ��(��)������
};
