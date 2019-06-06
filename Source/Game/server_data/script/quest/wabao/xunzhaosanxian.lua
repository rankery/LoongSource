
--寻找散仙活动
--3011066	姜天师
--3011067	蒋天师
--3011068	弥天师
--3011069	陆天师

--各个城市中随机坐标随机刷出NPC
sanxian_pos={}
--龙城
sanxian_pos[1]={MapID=3017298127,x=2033,y=4963,z=2136}
sanxian_pos[2]={MapID=3017298127,x=2314,y=6646,z=2247}
sanxian_pos[3]={MapID=3017298127,x=2022,y=6647,z=2442}
--凤翔
sanxian_pos[4]={MapID=3017298383,x=717,y=10000,z=2267}
sanxian_pos[5]={MapID=3017298383,x=846,y=10000,z=1996}
sanxian_pos[6]={MapID=3017298383,x=1090,y=10000,z=2172}
--涿鹿
sanxian_pos[7]={MapID=3017299663,x=2023,y=18338,z=2377}
sanxian_pos[8]={MapID=3017299663,x=2149,y=17512,z=2604}
sanxian_pos[9]={MapID=3017299663,x=2145,y=19209,z=2817}
--酆都
sanxian_pos[10]={MapID=3017299919,x=878,y=20140,z=1011}
sanxian_pos[11]={MapID=3017299919,x=1128,y=20140,z=831}
sanxian_pos[12]={MapID=3017299919,x=1394,y=20372,z=968}
--迦叶
sanxian_pos[13]={MapID=3017299151,x=2242,y=6744,z=1807}
sanxian_pos[14]={MapID=3017299151,x=2024,y=6365,z=1725}
--凌霄天宫
sanxian_pos[15]={MapID=3017299407,x=1826,y=14024,z=2358}
sanxian_pos[16]={MapID=3017299407,x=1912,y=14596,z=2567}
sanxian_pos[17]={MapID=3017299407,x=1841,y=15007,z=2778}

function xunzhaosanxian_OnStart(actID)
	local   k = math.random(1,17)
	--在各个场景城市中随机选择一个坐标点刷出散仙
	local n = math.random(1,100)
	if n >= 1 and n<=90 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 412)--散仙###（NPC名称）下界巡游至###（场景对应的城市名称），各位可在散仙处购买各种优异物品
		msg.AddMsgEvent(MsgID, 5, 3011066)
		msg.AddMsgEvent(MsgID, 6, sanxian_pos[k].MapID)
		msg.DispatchWorldMsgEvent(MsgID)
		map.MapCreateColCreature(sanxian_pos[k].MapID, -1, 3011066, sanxian_pos[k].x, sanxian_pos[k].y, sanxian_pos[k].z, 1)
	elseif n >= 91 and n<=95 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 412)--散仙###（NPC名称）下界巡游至###（场景对应的城市名称），各位可在散仙处购买各种优异物品
		msg.AddMsgEvent(MsgID, 5, 3011068)
		msg.AddMsgEvent(MsgID, 6, sanxian_pos[k].MapID)
		msg.DispatchWorldMsgEvent(MsgID)
		map.MapCreateColCreature(sanxian_pos[k].MapID, -1, 3011068, sanxian_pos[k].x, sanxian_pos[k].y, sanxian_pos[k].z, 1)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 412)--散仙###（NPC名称）下界巡游至###（场景对应的城市名称），各位可在散仙处购买各种优异物品
		msg.AddMsgEvent(MsgID, 5, 3011069)
		msg.AddMsgEvent(MsgID, 6, sanxian_pos[k].MapID)
		msg.DispatchWorldMsgEvent(MsgID)
		map.MapCreateColCreature(sanxian_pos[k].MapID, -1, 3011069, sanxian_pos[k].x, sanxian_pos[k].y, sanxian_pos[k].z, 1)
	end

end

--注册该活动的相关事件
aux.RegisterActEvent(81, 2, "xunzhaosanxian_OnStart")-----活动ID
aux.RegisterActEvent(82, 2, "xunzhaosanxian_OnStart")-----活动ID
aux.RegisterActEvent(83, 2, "xunzhaosanxian_OnStart")-----活动ID
aux.RegisterActEvent(84, 2, "xunzhaosanxian_OnStart")-----活动ID
aux.RegisterActEvent(85, 2, "xunzhaosanxian_OnStart")-----活动ID
aux.RegisterActEvent(91, 2, "xunzhaosanxian_OnStart")-----活动ID















