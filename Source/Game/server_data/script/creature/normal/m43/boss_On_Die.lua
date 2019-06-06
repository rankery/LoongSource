function m43_boss_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 9)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 10)

	if TargetTypeID == 1010201 then
		Boss_num = Boss_num + 1
	elseif TargetTypeID == 1010202 then
		Boss_num = Boss_num + 2
	elseif TargetTypeID == 1010252 then
		Boss_num = Boss_num + 4
	elseif TargetTypeID == 1010253 then
		Boss_num = Boss_num + 8
	elseif TargetTypeID == 1010302 then
		Boss_num = Boss_num + 16
	elseif TargetTypeID == 1010303 then
		Boss_num = Boss_num + 32
	end

	map.SetMapScriptData(MapID, InstanceID, 1, 10, Boss_num)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2
	local Boss5 = math.floor(Boss_num/16) % 2
	local Boss6 = math.floor(Boss_num/32) % 2

	local temp = math.max(Boss1+Boss2, Boss3+Boss4, Boss5+Boss6)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1782)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	if TargetTypeID == 1010304 then
		map.MapCreateCreature(MapID, InstanceID, 3075111, 399, 5968, 369)
		--玩家击杀无妄之壁后播放的服务器广播
		local MsgID = msg.BeginMsgEvent();					--注册广播消息句柄
		local n_RoleLv = role.GetRoleLevel(MapID, InstanceID, RoleID);		--获得击杀玩家的等级
		if ( n_RoleLv<101 ) then						--若击杀玩家的等级不大于100级时，则播放全服大字屏显广播
			msg.AddMsgEvent(MsgID,102,100020);
			msg.AddMsgEvent(MsgID,2,RoleID);
			msg.DispatchBroadcast(MsgID,-1,-1,-1);
		else
			msg.AddMsgEvent(MsgID,102,100021);
			msg.AddMsgEvent(MsgID,2,RoleID);

			local r = math.random(1,100);	--取随机数
			if ( r<16 ) then
				msg.DispatchBroadcast(MsgID,-1,-1,-1);			--击杀玩家等级大于100级时有15%几率播放全服广播，60%几率场景广播
			elseif ( r<61 ) then
				msg.DispatchBroadcast(MsgID,3017299407,-1,-1);--昆仑
				msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
			else
				msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
			end
		end
	end

	if TargetTypeID == 1010304 or TargetTypeID == 1010251 or TargetTypeID == 1010301 then

		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then
				local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
				if Target_MapID == MapID then
					role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 3305003, 2, -1, 8, 420)
				    if act.GetActIsStart(151) then
						HuoDeYinYuanJie(Role[i])
						HuoDeYinYuanJie(Role[i])
					end
				end
			end
		end

		local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 3075221, x, y, z)		--封魔使者
		map.MapCreateCreature(MapID, InstanceID, 1531108, x, y, z+4)	--祭魔石

	end

end

aux.RegisterCreatureEvent(1010201, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010202, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010301, 4, "m43_boss_OnDie")

aux.RegisterCreatureEvent(1010251, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010252, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010253, 4, "m43_boss_OnDie")

aux.RegisterCreatureEvent(1010302, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010303, 4, "m43_boss_OnDie")
aux.RegisterCreatureEvent(1010304, 4, "m43_boss_OnDie")
