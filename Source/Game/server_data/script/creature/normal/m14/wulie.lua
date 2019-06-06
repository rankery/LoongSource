--怪物死亡计数
function m14_wulie_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Wulie_Num = map.GetMapScriptData(MapID,InstanceID,1,6)
	map.SetMapScriptData(MapID,InstanceID,1,6,Wulie_Num+1)

	if Wulie_Num+1== 30 then
		map.MapCreateCreature(MapID, InstanceID, 1004528, 158, 150, 482)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1729)	--枭吴队长吴裂已经出现于坐标[158, 482]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Wulie_Num+1< 30 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1730)	--您已击杀xx名吴裂部众，击杀30名后枭吴队长吴裂将会出现！
		msg.AddMsgEvent(MsgID, 9, Wulie_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1004502, 4, "m14_wulie_OnDie")
aux.RegisterCreatureEvent(1004504, 4, "m14_wulie_OnDie")
