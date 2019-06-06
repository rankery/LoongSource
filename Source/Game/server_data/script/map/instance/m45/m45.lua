--刷怪触发器320
--青绝妖盗（1010506）
--X=278 Y=4202 Z=362
--X=300 Y=4216 Z=359
--X=275 Y=4215 Z=336
--X=297 Y=4200 Z=333
--刷怪触发器418
--赫连亲卫（）需求赫连亲卫4分半存在时间
--X=898 Y=7000 Z=146
--X=886 Y=7000 Z=170
--X=862 Y=7000 Z=146
--赫连亲卫（）
--X=899 Y=7000 Z=158
--X=865 Y=7000 Z=166
--X=872 Y=7000 Z=137
--上述怪需刷3次，每次间隔1.5分钟。全部击杀后，刷出赫连云龙（1010527）
--ScriptData第0位为判断是否触发过触发器320
--ScriptData第1位为判断是否触发过触发器418
--ScriptData第2位为判断是否触发过触发器743
--ScriptData第3位为判断正在刷第几波小怪
--ScriptData第4位为判断BOSS是否被杀。赫连云龙分值为1，红花鬼母分值为10.
--ScriptData第5位为判断杀掉赫连亲卫的数量。
--ScriptData第6位为判断杀掉花妖的数量。
--ScriptData第7-13位用于祭魔石。
--ScriptData第14为小怪总数
--ScriptData第15为领取奖励的人数纪录


function m45_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
	if ObjID == 320 then
		local BOOL_EnterTrigger320 = map.GetMapScriptData(MapID,InstanceID,1,0) --索引0数值为0则刷怪
		if BOOL_EnterTrigger320 == 0 then --判断是否被触发过
			map.SetMapScriptData(MapID,InstanceID,1,0,1)
		    map.MapCreateColCreature(MapID, InstanceID, 1010506, 278, 4202, 362, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010506, 300, 4216, 359, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010506, 275, 4215, 336, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010506, 397, 4200, 333, 1, "")
		end
	elseif ObjID == 418 then
		local BOOL_EnterTrigger418 = map.GetMapScriptData(MapID,InstanceID,1,1) --索引0数值为0则刷怪
		if BOOL_EnterTrigger418 == 0 then --判断是否被触发过
			map.SetMapScriptData(MapID,InstanceID,1,1,1)
			map.MapCreateColCreature(MapID, InstanceID, 1010503, 898, 7000, 146, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010503, 886, 7000, 170, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010503, 862, 7000, 146, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010505, 899, 7000, 158, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010505, 865, 7000, 166, 1, "")
		    map.MapCreateColCreature(MapID, InstanceID, 1010505, 872, 7000, 137, 1, "")
			map.SetMapScriptData(MapID,InstanceID,1,3,1)--设置该计数位为第一波
			map.MapCreateColCreature(MapID, InstanceID, 3075402, 154, 4320, 657, 1, "")--透明计时器1生存时间1分半
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1775)	--60秒后将刷新下一波赫连亲卫！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)     --发送消息
		end
	elseif ObjID == 743 then
		local BOOL_EnterTrigger743 = map.GetMapScriptData(MapID,InstanceID,1,2)
		if BOOL_EnterTrigger743 == 0 then --判断是否被触发过
			local i = role.GetRoleItemNum(RoleID, 2615801)
			local k = role.GetRoleItemNum(RoleID, 2615802)
			if i == 1 and k == 1 then
				map.SetMapScriptData(MapID,InstanceID,1,2,1)--记录已经触发过743
				map.OpenCloseDoor(MapID, InstanceID, 505, 1)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 2615801, 1, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 2615802, 1, 420)
			else
			    local MsgID = msg.BeginMsgEvent()
                msg.AddMsgEvent(MsgID, 26, 524)	-- "你必须同时拥有两把钥匙才能通过此门。"
                msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
		end
	end
end
aux.RegisterMapEvent("m45",6,"m45_OnEnterTrigger")

--下马
function m45_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m45", 2, "m45_OnPlayerEnter")



