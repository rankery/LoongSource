--·击杀 贪金近侍 1008078 刷出 贪金 1008074 坐标：x 2819 y 26387 z 684 ，场景内所有 鬼叉骠骑 1008073 消失
function c1008078_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008074, 2819, 26387, 684)
	--map.MapDeleteCreature(MapID, InstanceID, 1008073)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1766)	--贪金已出现于坐标[2819, 684]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008078, 4, "c1008078_OnDie")
