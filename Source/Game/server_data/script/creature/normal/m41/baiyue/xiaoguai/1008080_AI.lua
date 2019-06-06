--·击杀 贪金近侍 1008080 刷出 贪金 1008074 坐标：x 2437 y 26387 z 731，场景内所有 鬼叉骠骑 1008073 消失
function c1008080_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008074, 2437, 26387, 731)
	--map.MapDeleteCreature(MapID, InstanceID, 1008073)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1768)	--贪金已出现于坐标[2437, 731]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008080, 4, "c1008080_OnDie")
