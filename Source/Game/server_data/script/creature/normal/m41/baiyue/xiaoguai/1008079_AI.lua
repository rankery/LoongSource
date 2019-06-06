--·击杀 贪金近侍 1008079 刷出 贪金 1008074 坐标：x 2840 y 26387 z 316 ，场景内所有 鬼叉骠骑 1008073 消失
function c1008079_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008074, 2840, 26387, 316)
	--map.MapDeleteCreature(MapID, InstanceID, 1008073)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1767)	--贪金已出现于坐标[2840, 316]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008079, 4, "c1008079_OnDie")
