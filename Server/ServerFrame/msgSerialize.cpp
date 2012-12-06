#include "stdafx.h"
#include <Windows.h>
#include "msgSerialize.h"
#include <fgeutil.h>
#include <fgeDataBuff.h>
//#include "./xfile/xFile.h"

//using namespace protocol_name;
//using namespace luaHelp;
using namespace lua;
namespace serialize{
	enum KTYPE{ K_STR,K_INT};
	enum VTYPE{
		V_BOOL,V_STR,V_FLOAT,V_TABLE,
		V_INT8,V_INT16,V_INT32,V_INT64,
		V_UINT8,V_UINT16,V_UINT32,V_UINT64,
		V_CHAR,V_DOUBLE,V_BLOB};
		struct VAL
		{
			VTYPE type;
			union {
				BYTE			b;
				CHAR			c;
				LPCTSTR			str;
				float			s_rale;	//单精度实数
				double			d_rale;	//双精度实数

				CHAR			n8;
				SHORT			n16;
				INT				n32;
				INT64			n64;

				BYTE			u8;
				WORD			u16;
				DWORD			u32;
				UINT64			u64;

				struct Blob{
					void*			pData;
					DWORD			len;
				}blob;
			};
		};
}
using namespace serialize;
//消息处理
int	packValue(lua_State* L,int index,fge::DataBuf& packvn);
const char* packTable( lua_State* L, int index,fge::DataBuf& pack);
int	packValue(lua_State* L,int index,fge::DataBuf& packvn)
{
	int vt=0;
	serialize::VAL v;
	switch( lua_type(L,index) )
	{
	case  LUA_TBOOLEAN:
		vt = serialize::V_BOOL;
		v.n8 = lua_toboolean(L,index);
		packvn.Write(&v.n8,1);
		break;
	case  LUA_TNUMBER:
		vt = serialize::V_FLOAT;
		v.s_rale = lua_tonumber(L,(index));
		if((v.s_rale-(int)v.s_rale)<0.000001)
		{
			v.n64 = lua_tointeger(L,index);				
			if(v.n64<=0xff)
			{
				vt=serialize::V_INT8;
				packvn.Write(&v.n8,1);
			}
			else if(v.n64<=0xffff)
			{
				vt=serialize::V_INT16;
				packvn.Write(&v.n16,2);
			}	
			else if(v.n64<=0xffffffff)
			{
				vt=serialize::V_INT32;
				packvn.Write(&v.n16,4);
			}	
			else{
				vt=serialize::V_INT64;				
				packvn.Write(&v.n64,8);
			}
		}
		else
		{
			packvn.Write(&v.s_rale,sizeof(float));
		}
		break;
	case  LUA_TSTRING:
		vt = serialize::V_STR;
		v.str = lua_tostring(L,index);
		packvn.Write(v.str,strlen(v.str)+1);
		break;
	case  LUA_TTABLE:
		vt = serialize::V_TABLE;
		packTable(L,index,packvn);
		break;
	case LUA_TNIL:
	default:
		return -1;
	}
	return vt;
}
const char* packTable( lua_State* L, int index, fge::DataBuf& pack)
{
	BYTE tableLen=0;	//参数个数
	fge::DataBuf pack_kt,pack_k,pack_vt,pack_v;

	WORD* pKTypeBuf = (WORD*)(pack_kt());
	WORD* pVTypeBuf = (WORD*)(pack_vt());
	int vt=0,kt=0;
	int nv=0,nk=0;

	lua_pushnil(L); /* 第一个 key */
	while (lua_next(L, index) != 0)
	{
		vt = -1;
		if(lua_type(L,-2) == LUA_TSTRING){
			kt = K_STR;	packValue(L,-2,pack_k);
		}
		else{
			kt = K_INT;
		}
		if(lua_istable(L,-1))
		{			
			packTable(L,lua_gettop(L),pack_v);
			vt = serialize::V_TABLE;
		}
		else
		{			
			vt = packValue(L,-1,pack_v);
		}
		lua_pop(L, 1);//pop the top element
		//写入类型

		*((DWORD*)pKTypeBuf) |= kt<<nk;	
		//类型使用3位长度表示
		nk+=3;
		if(nk >= 16){
			nk -= 16;
			pKTypeBuf++;
		}			

		*((DWORD*)pVTypeBuf) |= vt<<nv;	
		//类型使用3位长度表示
		nv+=3;
		if(nv >= 16){
			nv -= 16;
			pVTypeBuf++;
		}

		tableLen++;
	}
	//写表长度
	pack.Write(&tableLen,1);
	//计算类型长度, 类型使用3位长度表示
	int x = (int)ceil(tableLen*3/8.0f);
	pack.Write(pack_kt(),x);
	pack.Write(pack_vt(),x);
	pack.Write(pack_k(),pack_k.GetLen());	
	pack.Write(pack_v(),pack_v.GetLen());
	int len = pack.GetLen();
	return pack();
}
const char*	Serialize::Pack( lua_State* L,int index,int num, int& len )
{
	if( !L ) return 0;
	if(index<1) index = 1;

	fge::DataBuf packvt,packvn;
	assert(lua_gettop(L) >= index+num-1);

	BYTE vn=num;	//参数个数
	packvt.Write(&vn,1);

	WORD* pbuf = (WORD*)(packvt()+1);
	int vt=0;
	int n=0;
	for (int i=0; i<vn; i++)
	{
		vt = packValue(L,index+i,packvn);
		if(vt<0) continue;
		//写入类型
		*((DWORD*)pbuf) |= vt<<n;	
		//类型使用3位长度表示
		n+=3;
		if(n >= 16){
			n -= 16;
			pbuf++;
		}
	}

	static fge::DataBuf pack;
	pack.ReSet();
	//计算类型长度, 类型使用3位长度表示
	int x = (int)ceil(vn*3/8.0f);
	pack.Write(packvt(),x+1);	//x+1 : 参数类型长度+1个字节的参数个数.
	pack.Write(packvn(),packvn.GetLen());
	len = pack.GetLen();
	return pack();
}
const char*	pushParam( lua_State* L, VTYPE t, const char* pData )
{
	const char* pNext = pData;
	serialize::VAL v;
	switch( t )
	{
	case serialize::V_BOOL:
		v.n8 = *((unsigned char*)pNext);
		pNext+=1;
		lua_pushboolean(L,v.n8);
		break;
	case serialize::V_INT8:
		v.n8 = *((unsigned char*)pNext);		
		pNext+=1;
		lua_pushnumber(L,v.n8);
		break;
	case serialize::V_INT16:
		v.n16 = *((short*)pNext);	
		pNext += 2;
		lua_pushnumber(L,v.n16);
		break;
	case serialize::V_INT32:
		v.n32 = *((int*)pNext);
		pNext += 4;
		lua_pushnumber(L,v.n32);
		break;
	case serialize::V_INT64:
		v.n64 = *((__int64*)pNext);
		pNext += 8;
		lua_pushinteger(L,v.n64);
		break;
	case serialize::V_FLOAT:
		v.s_rale = *((float*)pNext);
		lua_pushnumber(L,v.s_rale);
		pNext+=sizeof(float);
		break;
	case serialize::V_STR:
		v.str = (char*)pNext;
		pNext += strlen(v.str)+1;
		lua_pushstring(L,v.str);
		break;
		//case serialize::V_TABLE:	
		//	v.n16 = *((unsigned char*)pNext); 
		//	pNext += 1;
		//	len = *(int*)pNext;
		//	lua_newtable(L);
		//	Serialize::UnPack(L,v.str,len );

		//	lua_settable(L, -3);
		//	return pNext;
	default:
		lua_pop(L,1);
		return 0;
	}	
	return pNext;
}

const char* pushTable( lua_State* L, const char* pstream, long len)
{
	fge::DataReader unpack(pstream,len);
	BYTE vn = 0;
	int kt=0,vt=0;
	//参数个数
	unpack.Read(&vn,sizeof(BYTE));
	if(vn==0)
		return pstream+1;
	int n=0;
	int x = (int)ceil(vn*3/8.0f);

	WORD* ptbuf = (WORD*)(unpack.GetCurrentStream());	
	const char* pkvbuf= (const char*)(unpack.GetCurrentStream()+x*2);	
	DWORD *t = (DWORD*)ptbuf;	
	std::vector<const char*> kstr;
	std::vector<int> ktBuff;
	// read k
	for (int i=0; i<vn; i++)
	{
		kt = ((*t)>>n) & 0x07;
		ktBuff.push_back(kt);
		if( kt==K_STR )
		{
			kstr.push_back(pkvbuf); pkvbuf += strlen(pkvbuf)+1;
		}
		n+=3;
		if(n >= 16){
			n-=16;	ptbuf++;
			t = (DWORD*)ptbuf;
		}
	}

	lua_newtable(L);	
	// read v
	ptbuf = (WORD*)(unpack.GetCurrentStream()+x);
	t = (DWORD*)ptbuf;	n=0;
	for (int i=0,_ks=0,_ki=1; i<vn; i++)
	{
		if(ktBuff[i] == K_STR)
			lua_pushstring(L,kstr[_ks++]);

		vt = ((*t)>>n) & 0x07;
		n+=3;
		if(n >= 16){
			n-=16;	ptbuf++;
			t = (DWORD*)ptbuf;
		}

		if(vt == V_TABLE)
			pkvbuf = pushTable(L,pkvbuf,(long)((pstream+len)-pkvbuf));
		else		// read value type:			
			pkvbuf = pushParam(L,(serialize::VTYPE)vt,pkvbuf);

		if(ktBuff[i] == K_STR)
			lua_settable(L, -3);
		else
			lua_rawseti(L, -2,_ki++);
	}
	return pkvbuf;
}
int	Serialize::UnPack( lua_State* L, const char* pstream, long len )
{
	fge::DataReader unpack(pstream,0xffff);
	BYTE vn = 0;
	int kt=0,vt=0;
	//参数个数
	unpack.Read(&vn,sizeof(BYTE));
	int x = (int)ceil(vn*3/8.0f);

	WORD* ptbuf = (WORD*)(unpack.GetCurrentStream());	//type buff
	const char* pvbuf = (char*)(unpack.GetCurrentStream()+x);	//value buff

	DWORD *t = (DWORD*)ptbuf;							//type
	t = (DWORD*)ptbuf;
	for (int i=0,n=0; i<vn; i++)
	{
		// read value type:
		vt = ((*t)>>n) & 0x07;
		n+=3;
		if(n >= 16){
			n-=16;	ptbuf++;
			t = (DWORD*)ptbuf;
		}

		if(vt == V_TABLE)
		{
			pvbuf = pushTable(L,pvbuf, (long)((pstream+len)-pvbuf));
		}
		else
		{
			pvbuf = pushParam(L,(serialize::VTYPE)vt,pvbuf);
		}
	}
	return vn;
}