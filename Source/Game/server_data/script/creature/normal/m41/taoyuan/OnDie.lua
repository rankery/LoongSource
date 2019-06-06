function taoyuan_xiaoguai_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	if TargetTypeID==1008003 then
		unit.AddBuff(MapID, InstanceID, RoleID, 2016601, RoleID)

	elseif TargetTypeID==1008020 then
		local temp = math.random(1,100)
		if temp>=1 and temp<=15 then
			--15%几率喊话
			cre.MonsterSay(MapID, InstanceID, TargetID, 40028)
		end

	elseif TargetTypeID==1008023 then
		local temp = math.random(1,100)
		if temp>=1 and temp<=15 then
			--15%几率喊话
			cre.MonsterSay(MapID, InstanceID, TargetID, 40031)
		end

	elseif TargetTypeID==1008026 then
		local temp = math.random(1,100)
		if temp>=1 and temp<=15 then
			--15%几率喊话
			cre.MonsterSay(MapID, InstanceID, TargetID, 40032)
		end

	elseif TargetTypeID==1008035 then
		local temp = math.random(1,100)
		if temp>=1 and temp<=15 then
			--15%几率喊话
			cre.MonsterSay(MapID, InstanceID, TargetID, 40037)
		end

	elseif TargetTypeID==1008091 then
		map.MapCreateCreature(MapID, InstanceID, 3060035, 1154, 14780, 523)

	end

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 12)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 12, xiaoguai_num)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 13)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1758)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1008003, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008004, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008005, 4, "taoyuan_xiaoguai_OnDie")

aux.RegisterCreatureEvent(1008013, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008014, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008015, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008016, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008017, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008018, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008019, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008020, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008021, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008022, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008023, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008026, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008027, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008028, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008029, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008030, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008031, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008032, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008033, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008034, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008035, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008036, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008037, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008040, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008041, 4, "taoyuan_xiaoguai_OnDie")

aux.RegisterCreatureEvent(1008048, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008049, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008050, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008051, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008052, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008053, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008054, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008055, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008056, 4, "taoyuan_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1008057, 4, "taoyuan_xiaoguai_OnDie")

aux.RegisterCreatureEvent(1008091, 4, "taoyuan_xiaoguai_OnDie")
