function m47_xiaoguai_OnDie(MapID, InstanceID, CreatureID, TargetTypeID, RoleID)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 0)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 0, xiaoguai_num)

	if xiaoguai_num<30 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1817)	--您已击杀xx名吴裂部众，击杀30名后枭吴队长吴裂将会出现！
		msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif xiaoguai_num==30 then
		map.MapCreateCreature(MapID, InstanceID, 1011017, 263, 12631, 245, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1818)	--枭吴队长吴裂已经出现于坐标[158, 482]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif xiaoguai_num<50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1819)	--您已击杀xx名吴裂部众，击杀30名后枭吴队长吴裂将会出现！
		msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif xiaoguai_num==50 then
		map.MapCreateCreature(MapID, InstanceID, 1011018, 207, 14883, 444,1 )
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1820)	--枭吴队长吴裂已经出现于坐标[158, 482]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

	local r = math.random(100)

	if r<=8 then

		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		--如果没有小队
		if TeamID == nil or TeamID == 4294967295 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 3304001, 1, -1, 8, 420)
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for k in pairs(Role) do
				role.AddRoleItem(MapID, InstanceID, Role[k], 3304001, 1, -1, 8, 420)
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1823)	--枭吴队长吴裂已经出现于坐标[158, 482]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	end

end

aux.RegisterCreatureEvent(1011011, 4, "m47_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011012, 4, "m47_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011013, 4, "m47_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011014, 4, "m47_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011015, 4, "m47_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011016, 4, "m47_xiaoguai_OnDie")
