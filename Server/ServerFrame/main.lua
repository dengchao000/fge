------------------
--预加载脚本
------------------
print( '预加载脚本' )
function OnLuaMsg(msgid,v)
	local f = _G["Msg_"..msgid];
	if f ~= nil then
		f(v)
	else
		print( msgid )
	end
end

function Msg_18792315( v )
	print( 'name = '..v.name..' password='..v.password );
end

function Msg_18792325( v )	
end

------------------
--Test
------------------
function Test()
	print('Test');	
		
	d={18792315,{'abc','def','dfw'},name="dc",password="cdd8679"}
	
	net.SendMsg(18792315,d);
	net.SendMsg(18792325,d);
end

