function m40_xiaoguai_OnDie(MapID, InstanceID, CreatureID)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 11)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 11, xiaoguai_num)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 8)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1756)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1005408, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005409, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005410, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005411, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005412, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005413, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005414, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005415, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005416, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005417, 4, "m40_xiaoguai_OnDie")

aux.RegisterCreatureEvent(1005463, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005464, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005465, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005466, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005467, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005468, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005469, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005470, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005471, 4, "m40_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005472, 4, "m40_xiaoguai_OnDie")
