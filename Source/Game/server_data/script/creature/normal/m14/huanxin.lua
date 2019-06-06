--怪物死亡计数
function m14_huanxin_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Huanxin_Num = map.GetMapScriptData(MapID,InstanceID,1,7)
	map.SetMapScriptData(MapID,InstanceID,1,7,Huanxin_Num+1)

	if Huanxin_Num+1== 30 then
		map.MapCreateCreature(MapID, InstanceID, 1004531, 223, 150, 747)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1731)	--妖邪祭祀幻辛已经出现于坐标[223, 747]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	elseif Huanxin_Num+1< 30 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1732)	--您已击杀xx名幻辛部众，击杀30名后妖邪祭祀幻辛将会出现！
		msg.AddMsgEvent(MsgID, 9, Huanxin_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1004503, 4, "m14_huanxin_OnDie")
aux.RegisterCreatureEvent(1004505, 4, "m14_huanxin_OnDie")
aux.RegisterCreatureEvent(1004521, 4, "m14_huanxin_OnDie")
