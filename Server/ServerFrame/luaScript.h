#pragma once
#include "msgSerialize.h"
#include "luaHelp.h"
/**
½Å±¾ÏµÍ³
*/
struct LuaScript{
	LuaScript( ){state = luaHelp::LuaState::Create(true);}
	~LuaScript( ){
		if(state) {
			state->Close();
			delete state;
		}
	}
	lua_State* GetLuaState( ){ return (lua_State*)state; }

	void	Register(const char *libname,const luaL_Reg *l){ 
		if(state) luaL_register(state->GetCState(),libname,l);
	}
public:
	luaHelp::LuaState *state;
};