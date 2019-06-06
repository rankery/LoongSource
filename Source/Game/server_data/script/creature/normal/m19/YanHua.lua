function c3040539_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local truebox = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)

	if truebox==1 then
		map.MapCreateCreature(MapID, InstanceID, 1006530, 609, 8773, 691)
		map.MapCreateCreature(MapID, InstanceID, 4900605, x, y, z)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1740)	--无间军头已经出现于坐标[609, 691]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	else
		map.MapCreateCreature(MapID, InstanceID, 1006514, x, y, z)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1741)	--箱子是空的！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end
aux.RegisterCreatureEvent(3040539, 6, "c3040539_OnInvest")
