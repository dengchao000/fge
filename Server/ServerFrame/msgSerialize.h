/**
** Author:	�˳�
** QQ:		23427470
** Mail:	aron_d@yeah.net
** Time:	2010-12-15

** �汾	 :	1.0
** ����  :  ��Ϣ
**/

#pragma once
extern "C"{    
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
}

namespace lua{
	struct Serialize{
		static const char*	Pack( lua_State* L,int index,int num, int& len );
		static int			UnPack( lua_State* L, const char* pstream, long len );
	};
}