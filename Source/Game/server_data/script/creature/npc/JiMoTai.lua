--冰封魔窟祭魔台脚本
function JiMoTai_OnTalk_m40(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	91)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	94)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	95)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		local CanSummon=1

		local Summoned_Num=map.GetMapScriptData(MapID, InstanceID, 1, 1)

		if Summoned_Num>=6 then
			CanSummon=0
		end

		for i=2, 7 do
			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
				CanSummon=0
				break
			end
		end

		if CanSummon==0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	99)	--每次创建副本，每位玩家只可进行一次魔王召唤，总共只可进行六次魔王召唤。
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200601, 1, 420) then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local monsterid = map.MapCreateColCreature(MapID, InstanceID, 1531104, x, y, z, 1)
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 1, Summoned_Num+1)
				map.SetMapScriptData(MapID, InstanceID, 1, Summoned_Num+2, RoleID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	96)	--需要祭魔石。
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(1531101, 7, "JiMoTai_OnTalk_m40")

--通冥鬼域祭魔台脚本
function JiMoTai_OnTalk_m42(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	92)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	94)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	95)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		local CanSummon=1

		local Summoned_Num=map.GetMapScriptData(MapID, InstanceID, 1, 3)

		if Summoned_Num>=6 then
			CanSummon=0
		end

		for i=4, 9 do
			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
				CanSummon=0
				break
			end
		end

		if CanSummon==0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	99)	--每次创建副本，每位玩家只可进行一次魔王召唤，总共只可进行六次魔王召唤。
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200602, 1, 420) then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local monsterid = map.MapCreateColCreature(MapID, InstanceID, 1531105, x, y, z, 1)
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 3, Summoned_Num+1)
				map.SetMapScriptData(MapID, InstanceID, 1, Summoned_Num+4, RoleID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	97)	--需要祭魔石。
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(1531102, 7, "JiMoTai_OnTalk_m42")

--桃园秘境祭魔台脚本

function JiMoTai_OnTalk_m41(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	93)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	94)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	95)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		local CanSummon=1

		local Summoned_Num=map.GetMapScriptData(MapID, InstanceID, 1, 3)

		if Summoned_Num>=6 then
			CanSummon=0
		end

		for i=4, 9 do
			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
				CanSummon=0
				break
			end
		end
		if CanSummon==0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	99)	--每次创建副本，每位玩家只可进行一次魔王召唤，总共只可进行六次魔王召唤。
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200603, 1, 420) then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local monsterid = map.MapCreateColCreature(MapID, InstanceID, 1531106, x, y, z, 1)
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 3, Summoned_Num+1)
				map.SetMapScriptData(MapID, InstanceID, 1, Summoned_Num+4, RoleID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	98)	--需要祭魔石。
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(1531103, 7, "JiMoTai_OnTalk_m41")

--青绝洞府祭魔台脚本

function JiMoTai_OnTalk_m45(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	476)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	94)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	95)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		local CanSummon=1

		local Summoned_Num=map.GetMapScriptData(MapID, InstanceID, 1, 7)

		if Summoned_Num>=6 then
			CanSummon=0
		end

		for i=8, 13 do
			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
				CanSummon=0
				break
			end
		end
		if CanSummon==0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	99)	--每次创建副本，每位玩家只可进行一次魔王召唤，总共只可进行六次魔王召唤。
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200638, 1, 420) then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local monsterid = map.MapCreateColCreature(MapID, InstanceID, 1531109, x, y, z, 1)
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 7, Summoned_Num+1)
				map.SetMapScriptData(MapID, InstanceID, 1, Summoned_Num+8, RoleID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	478)	--需要祭魔石。
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(1531107, 7, "JiMoTai_OnTalk_m45")

--玉虚峰祭魔台脚本

function JiMoTai_OnTalk_m43(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	477)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	94)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	95)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		local CanSummon=1

		local Summoned_Num=map.GetMapScriptData(MapID, InstanceID, 1, 25)

		if Summoned_Num>=6 then
			CanSummon=0
		end

		for i=26, 31 do
			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
				CanSummon=0
				break
			end
		end
		if CanSummon==0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	99)	--每次创建副本，每位玩家只可进行一次魔王召唤，总共只可进行六次魔王召唤。
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 3200639, 1, 420) then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local monsterid = map.MapCreateColCreature(MapID, InstanceID, 1531110, x, y, z, 1)
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 25, Summoned_Num+1)
				map.SetMapScriptData(MapID, InstanceID, 1, Summoned_Num+26, RoleID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	479)	--需要祭魔石。
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(1531108, 7, "JiMoTai_OnTalk_m43")

