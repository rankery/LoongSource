--固定活动二八星宿下凡


--活动中怪物死亡事件
function xingsu_Dead(MapID, InstanceID, CreatureID, TypeID, KillerID)
	--检测Killer是否有小队
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, KillerID)
			--增加进阶实力值
			Increase_shili(MapID, InstanceID, KillerID, 1, 20, "jinjieshili_A")
	--设置怪物为死亡状态
	act1_Creature[TypeID].IsDead = true

	--如果没有小队
	if 4294967295 == TeamID  then
		--计算活动任务奖励
		local BuffID = act1_Creature[TypeID].BuffID
		if -1 ~= BuffID	 then
			-- 给玩家一个特殊buff
			unit.AddBuff(MapID, InstanceID, KillerID, BuffID, KillerID)
		end

	else	-- 如果玩家有小队
		--获得小队玩家ID
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for k in pairs(Role) do
			if 4294967295 ~= Role[k] then
				--计算活动任务奖励
				local BuffID = act1_Creature[TypeID].BuffID
				if -1 ~= BuffID	 then
					-- 给玩家一个特殊buff
					unit.AddBuff(MapID, InstanceID, Role[k], BuffID, Role[k])
				end
			end
		end
	end

	--服务器广播“###成功挑战星宿###”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 1)
	msg.AddMsgEvent(MsgID, 2, KillerID)
	msg.AddMsgEvent(MsgID, 5, TypeID)
	msg.DispatchWorldMsgEvent(MsgID)

	--计算灵兽下凡概率
	Rate = math.random(1, 100)
	if Rate < 30 then
		--随机一个刷怪点
		local randPoint = math.random(500, 520)
		local x = map_list[MapID].map[randPoint].x
		local y = map_list[MapID].map[randPoint].y
		local z = map_list[MapID].map[randPoint].z

		--刷出灵兽
		map.MapCreateCreature(MapID, -1, 1500000, x, y, z)
		--全服广播“星宿下凡，天界灵兽逃脱，出现于###， 大家赶紧去抓捕”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3)
		msg.AddMsgEvent(MsgID, 6, MapID)
		msg.AddMsgEvent(MsgID, 9, x)
		msg.AddMsgEvent(MsgID, 9, z)
		msg.DispatchWorldMsgEvent(MsgID)
	end
end

--注册怪物死亡事件
aux.RegisterCreatureEvent(1500001, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500002, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500003, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500004, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500005, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500006, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500007, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500008, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500009, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500010, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500011, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500012, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500013, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500014, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500015, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500016, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500017, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500018, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500019, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500020, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500021, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500022, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500023, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500024, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500025, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500026, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500027, 4, "xingsu_Dead")
aux.RegisterCreatureEvent(1500028, 4, "xingsu_Dead")

