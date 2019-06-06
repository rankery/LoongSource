function c1006507_OnDie(MapID, InstanceID, CreatureID)

	local Hanbing_Num = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	map.SetMapScriptData(MapID,InstanceID,1,3,Hanbing_Num+1)

	if Hanbing_Num+1<10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1744)	--您已击杀xx名寒冰营巫毒，击杀10名后寒冰鬼将将会出现！
		msg.AddMsgEvent(MsgID, 9, Hanbing_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Hanbing_Num+1==10 then
		map.MapCreateCreature(MapID, InstanceID, 1006556, 1541, 4346, 401)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1745)	--寒冰鬼将已经出现于坐标[1541, 401]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1006507, 4, "c1006507_OnDie")

