function m45_xiaoguai_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	if TargetTypeID == 1010503 or TargetTypeID == 1010505 then

		local i = map.GetMapScriptData(MapID,InstanceID,1,5)
		map.SetMapScriptData(MapID,InstanceID,1,5,i+1)
		if i+1 == 18 then
			map.MapCreateColCreature(MapID, InstanceID, 1010527, 881, 7000, 155, 1, "")
		end

	elseif TargetTypeID == 1010510 then

		local kill = map.GetMapScriptData(MapID,InstanceID,1,6)
		map.SetMapScriptData(MapID,InstanceID,1,6,kill+1)
		if kill+1 == 10 then
			map.MapCreateColCreature(MapID, InstanceID, 1010528, 872, 4348, 340, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010510, 839, 4296, 321, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010510, 835, 4296, 334, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010509, 851, 4312, 319, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010509, 846, 4306, 341, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010507, 874, 4340, 333, 1, "")
			map.MapCreateColCreature(MapID, InstanceID, 1010507, 872, 4348, 340, 1, "")

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1778)	--红花鬼母已经出现于坐标[872, 340]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)     --发送消息
		end

	end

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 14)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 14, xiaoguai_num)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 4)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1777)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1010501, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010502, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010503, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010504, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010505, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010506, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010507, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010508, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010509, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010510, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010511, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010512, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010513, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010514, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010515, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010516, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010517, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010518, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010519, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010520, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010521, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010522, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010523, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010524, 4, "m45_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1010525, 4, "m45_xiaoguai_OnDie")
