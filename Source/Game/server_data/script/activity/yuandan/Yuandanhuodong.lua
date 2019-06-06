

Yuan_Dan_NPC = {}
Yuan_Dan_NPC[1] = {mapcrc=3017298127, monsterid=1531201, x=2209, y=6646, z=2223, x2=2120, y2=7175, z2=2336}
Yuan_Dan_NPC[2] = {mapcrc=3017298383, monsterid=1531202, x=913, y=10000, z=1943, x2=1061, y2=10000, z2=2153}
Yuan_Dan_NPC[3] = {mapcrc=3017299663, monsterid=1531203, x=2071, y=18338, z=2405, x2=2106, y2=18338, z2=2683}
Yuan_Dan_NPC[4] = {mapcrc=3017299919, monsterid=1531204, x=1027, y=20140, z=977, x2=976, y2=20140, z2=954}
Yuan_Dan_NPC[5] = {mapcrc=3017299151, monsterid=1531205, x=2064, y=6365, z=1794, x2=2032, y2=6365, z2=1730}
Yuan_Dan_NPC[6] = {mapcrc=3017299407, monsterid=1531205, x=1865, y=14024, z=2290, x2=1744, y2=14024, z2=2309}


function YuanDan_OnStart(actID)
	if Yuandan == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100401)	--活动开始公告
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		Yuan_Dan_NPC[1].CreID = map.MapCreateCreature(Yuan_Dan_NPC[1].mapcrc, -1, 3610103, Yuan_Dan_NPC[1].x, Yuan_Dan_NPC[1].y, Yuan_Dan_NPC[1].z)
		Yuan_Dan_NPC[2].CreID = map.MapCreateCreature(Yuan_Dan_NPC[2].mapcrc, -1, 3610103, Yuan_Dan_NPC[2].x, Yuan_Dan_NPC[2].y, Yuan_Dan_NPC[2].z)
		Yuan_Dan_NPC[3].CreID = map.MapCreateCreature(Yuan_Dan_NPC[3].mapcrc, -1, 3610103, Yuan_Dan_NPC[3].x, Yuan_Dan_NPC[3].y, Yuan_Dan_NPC[3].z)
		Yuan_Dan_NPC[4].CreID = map.MapCreateCreature(Yuan_Dan_NPC[4].mapcrc, -1, 3610103, Yuan_Dan_NPC[4].x, Yuan_Dan_NPC[4].y, Yuan_Dan_NPC[4].z)
		Yuan_Dan_NPC[5].CreID = map.MapCreateCreature(Yuan_Dan_NPC[5].mapcrc, -1, 3610103, Yuan_Dan_NPC[5].x, Yuan_Dan_NPC[5].y, Yuan_Dan_NPC[5].z)
		Yuan_Dan_NPC[6].CreID = map.MapCreateCreature(Yuan_Dan_NPC[6].mapcrc, -1, 3610103, Yuan_Dan_NPC[6].x, Yuan_Dan_NPC[6].y, Yuan_Dan_NPC[6].z)
	end
end


function YuanDan_OnTimerMin(actID)

	local curhour = os.date("%H")
	local curmin = os.date("%M")

	if (curhour=="11" or curhour=="13" or curhour=="15" or curhour=="17" or curhour=="19" or curhour=="21") and curmin=="00" then		-- 整点刷堆雪人NPC
		for i=1,6 do
			map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610101, Yuan_Dan_NPC[i].x2, Yuan_Dan_NPC[i].y2, Yuan_Dan_NPC[i].z2)
			map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610101, Yuan_Dan_NPC[i].x2+20, Yuan_Dan_NPC[i].y2, Yuan_Dan_NPC[i].z2)
			map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610101, Yuan_Dan_NPC[i].x2, Yuan_Dan_NPC[i].y2, Yuan_Dan_NPC[i].z2+20)
		end
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100403)  --刷出雪人的公告
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
	end
end


function C3610101_OnInVest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)


	local monsterHP = cre.GetCreatureScriptData(MapID, InstanceID, CreatureID, 1, 0)
	local nRet = role.RemoveFromRole(MapID, InstanceID, RoleID, 1350401, 1, 1000)
	local a, b ,c = unit.GetPosition(MapID, InstanceID, CreatureID)

	if 0 == nRet then
		local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		role.AddRoleExp(MapID, InstanceID, RoleID, level*50)					-- 根据角色等级给角色增加经验
		local i = math.random(100)
		if i>70 then
			role.AddRoleSilver(MapID, InstanceID, RoleID, level*30, 102)		-- 有30%概率给角色加经验同时增加金钱
		end
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 355)
		msg.AddMsgEvent(MsgID, 9, level*50)	--经验值
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		monsterHP=monsterHP+1
		cre.SetCreatureScriptData(MapID, InstanceID, CreatureID, 1, 0, monsterHP)
		if monsterHP%50 == 0 then												-- 每100次堆雪人动作都会使雪人变大，并且燃放烟花
			unit.AddBuff(MapID, InstanceID, CreatureID, 5002801, CreatureID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 353)
			msg.DispatchMapMsgEvent(MsgID, MapID, -1)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a-5, b, c+5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2021601, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a, b, c+5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2021701, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a-5, b, c,1)
			unit.AddBuff(MapID, InstanceID, hua, 2021801, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a+5, b, c-5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2021901, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a, b, c-5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2022001, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a-5, b, c-5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2022101, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a+5, b, c+5,1)
			unit.AddBuff(MapID, InstanceID, hua, 2022201, hua)
			local hua = map.MapCreateColCreature(MapID, InstanceID, 4900602, a+5, b, c,1)
			unit.AddBuff(MapID, InstanceID, hua, 2022901, hua)
		end
	else
		unit.ChangeHP(MapID, InstanceID, CreatureID, 1)							-- 雪人血量加1
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 356)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end
function C3610101_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	local x, y ,z = unit.GetPosition(MapID, InstanceID, CreatureID)

	if MapID==3017298127 then													--在不同场景内刷出不同等级的雪人
		map.MapCreateColCreature(MapID, InstanceID, 1531212, x, y, z,1)
	elseif MapID==3017298383 then
		map.MapCreateColCreature(MapID, InstanceID, 1531213, x, y, z,1)
	elseif MapID==3017299663 then
		map.MapCreateColCreature(MapID, InstanceID, 1531214, x, y, z,1)
	elseif MapID==3017299919 then
		map.MapCreateColCreature(MapID, InstanceID, 1531215, x, y, z,1)
	elseif MapID==3017299151 then
		map.MapCreateColCreature(MapID, InstanceID, 1531216, x, y, z,1)
	elseif MapID==3017299407 then
		map.MapCreateColCreature(MapID, InstanceID, 1531217, x, y, z,1)
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 354)
	msg.DispatchMapMsgEvent(MsgID, MapID, -1)
end

function Yuandan_OnEnd(actID)
	if Yuandan == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100402)	--活动结束公告
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		for i = 1,6 do
			if Yuan_Dan_NPC[i].CreID ~= nil and Yuan_Dan_NPC[i].CreID ~= 0 then
				map.MapDeleteCreature(Yuan_Dan_NPC[i].mapcrc, -1, Yuan_Dan_NPC[i].CreID)
			end
		end
		
	end
end

aux.RegisterCreatureEvent(3610101, 4, "C3610101_OnDie")
aux.RegisterCreatureEvent(3610101, 6, "C3610101_OnInVest")
aux.RegisterActEvent(158, 4, "YuanDan_OnTimerMin")
aux.RegisterActEvent(158, 2, "YuanDan_OnStart")
aux.RegisterActEvent(158, 3, "Yuandan_OnEnd")
