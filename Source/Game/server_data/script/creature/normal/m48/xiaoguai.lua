function m48_xiaoguai_OnDie(MapID, InstanceID, CreatureID)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 0)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 0, xiaoguai_num)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 1)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2

	local temp = Boss1+Boss2+Boss3

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1822)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1011001, 4, "m48_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011002, 4, "m48_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011003, 4, "m48_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011004, 4, "m48_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011005, 4, "m48_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1011006, 4, "m48_xiaoguai_OnDie")
