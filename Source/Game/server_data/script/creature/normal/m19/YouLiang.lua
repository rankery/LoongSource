function c1006514_OnDie(MapID, InstanceID, CreatureID)

	local Qinbing_Num = map.GetMapScriptData(MapID, InstanceID, 1, 2)
	map.SetMapScriptData(MapID,InstanceID,1,2,Qinbing_Num+1)

	if Qinbing_Num+1<10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1742)	--您已击杀xx名幽量亲兵，击杀10名后幽量鬼将将会出现！
		msg.AddMsgEvent(MsgID, 9, Qinbing_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Qinbing_Num+1==10 then
		map.MapCreateCreature(MapID, InstanceID, 1006531, 581, 5923, 442)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1743)	--幽量鬼将已经出现于坐标[581, 442]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1006514, 4, "c1006514_OnDie")

