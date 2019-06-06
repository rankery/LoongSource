--怪物死亡计数
function m14_kuangzhua_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Kuangzhua_Num = map.GetMapScriptData(MapID,InstanceID,1,9)
	map.SetMapScriptData(MapID,InstanceID,1,9,Kuangzhua_Num+1)

	if Kuangzhua_Num+1== 40 then
		map.MapCreateCreature(MapID, InstanceID, 1004530, 897, 276, 918)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1735)	--枭吴教头狂爪已经出现于坐标[897, 918]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Kuangzhua_Num+1< 40 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1736)	--您已击杀xx名狂爪部众，击杀40名后枭吴教头狂爪将会出现！
		msg.AddMsgEvent(MsgID, 9, Kuangzhua_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1004507, 4, "m14_kuangzhua_OnDie")
aux.RegisterCreatureEvent(1004509, 4, "m14_kuangzhua_OnDie")
aux.RegisterCreatureEvent(1004511, 4, "m14_kuangzhua_OnDie")
