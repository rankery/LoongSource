function m43_xiaoguai_OnDie(MapID, InstanceID, CreatureID)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 9)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 9, xiaoguai_num)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 10)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2
	local Boss5 = math.floor(Boss_num/16) % 2
	local Boss6 = math.floor(Boss_num/32) % 2

	local temp = math.max(Boss1+Boss2, Boss3+Boss4, Boss5+Boss6)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1782)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1010203, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010204, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010205, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010206, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010207, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010208, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010209, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010210, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010211, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010212, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010213, 4, 'm43_xiaoguai_OnDie')

aux.RegisterCreatureEvent(1010254, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010257, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010255, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010256, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010258, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010259, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010260, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010261, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010262, 4, 'm43_xiaoguai_OnDie')

aux.RegisterCreatureEvent(1010305, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010306, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010307, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010308, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010309, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010310, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010311, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(1010312, 4, 'm43_xiaoguai_OnDie')
aux.RegisterCreatureEvent(3075105, 4, 'm43_xiaoguai_OnDie')
