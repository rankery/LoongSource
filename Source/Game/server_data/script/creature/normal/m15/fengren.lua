--怪物死亡计数
function m15_fengren_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Fengren_Num = map.GetMapScriptData(MapID,InstanceID,1,3)
	map.SetMapScriptData(MapID,InstanceID,1,3,Fengren_Num+1)

	if Fengren_Num+1== 40 then
		map.MapCreateCreature(MapID, InstanceID, 1004426, 501, 7296, 356)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1707)	--风刃霸山已经出现于坐标[248, 563]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Fengren_Num+1< 40 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1708)	--您已击杀xx名风刃部众，击杀40名后风刃霸山将会出现！
		msg.AddMsgEvent(MsgID, 9, Fengren_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1004419, 4, "m15_fengren_OnDie")
aux.RegisterCreatureEvent(1004420, 4, "m15_fengren_OnDie")
aux.RegisterCreatureEvent(1004421, 4, "m15_fengren_OnDie")
aux.RegisterCreatureEvent(1004422, 4, "m15_fengren_OnDie")

