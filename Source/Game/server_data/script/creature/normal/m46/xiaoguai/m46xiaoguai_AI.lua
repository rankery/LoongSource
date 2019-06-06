--怪物死亡
function m46xioguai_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local Num = map.GetMapScriptData(MapID, InstanceID, 1, 13) + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 13, Num)
	if Num == 1100 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3351)	--通往合元天魔第二形态房间的大门已经打开！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	    map.OpenCloseDoor(MapID, InstanceID, 1524, 1)
	    map.OpenCloseDoor(MapID, InstanceID, 1525, 1)
	elseif Num < 1100 then
	    local a = math.floor(Num/1000)
		local b = Num % 1000
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3352)	--击杀喽啰XX/100,击杀合元天魔第一形态X/1
		msg.AddMsgEvent(MsgID, 9, b)
		msg.AddMsgEvent(MsgID, 9, a)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3353)	--条件完成后将会开启通往最终首领房间的大门！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end
aux.RegisterCreatureEvent(1010401, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010412, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010402, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010413, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010403, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010414, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010404, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010415, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010416, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010417, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010419, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010420, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010421, 4, "m46xioguai_OnDie")
aux.RegisterCreatureEvent(1010422, 4, "m46xioguai_OnDie")
