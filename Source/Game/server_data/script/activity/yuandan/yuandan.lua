--[[元旦节日活动

Yuan_Dan_NPC = {}
Yuan_Dan_NPC[1] = {mapcrc=3017298127, monsterid=1531201, x=2209, y=6646, z=2223, x2=2156, y2=6646, z2=2223}
Yuan_Dan_NPC[2] = {mapcrc=3017298383, monsterid=1531202, x=913, y=10000, z=1943, x2=863, y2=10000, z2=1963}
Yuan_Dan_NPC[3] = {mapcrc=3017299663, monsterid=1531203, x=2071, y=18338, z=2405, x2=2134, y2=18338, z2=2423}
Yuan_Dan_NPC[4] = {mapcrc=3017299919, monsterid=1531204, x=1027, y=20140, z=977, x2=976, y2=20140, z2=954}
Yuan_Dan_NPC[5] = {mapcrc=3017299151, monsterid=1531205, x=2064, y=6365, z=1794, x2=2032, y2=6365, z2=1730}
Yuan_Dan_NPC[6] = {mapcrc=3017299407, monsterid=1531205, x=1865, y=14024, z=2290, x2=1744, y2=14024, z2=2309}

Yuan_Dan_Exp = {}
Yuan_Dan_Exp[0] = 380
Yuan_Dan_Exp[1] = 780
Yuan_Dan_Exp[2] = 1280
Yuan_Dan_Exp[3] = 1890
Yuan_Dan_Exp[4] = 2550
Yuan_Dan_Exp[5] = 3550
Yuan_Dan_Exp[6] = 4680
Yuan_Dan_Exp[7] = 6380
Yuan_Dan_Exp[8] = 8160
Yuan_Dan_Exp[9] = 10340
Yuan_Dan_Exp[10] = 11160
Yuan_Dan_Exp[11] = 14160

YuanDan_TimerMin = 0

m02_yuandan_point={}
m02_yuandan_point[1]={mapcrc=3017298127, x=2296, y=5756, z=2204}
m02_yuandan_point[2]={mapcrc=3017298127, x=3265, y=4885, z=1597}
m02_yuandan_point[3]={mapcrc=3017298127, x=3184, y=8402, z=3356}
m02_yuandan_point[4]={mapcrc=3017298127, x=1925, y=8000, z=3624}

m03_yuandan_point={}
m03_yuandan_point[1]={mapcrc=3017298383, x=967, y=10000, z=2079}
m03_yuandan_point[2]={mapcrc=3017298383, x=537, y=8798, z=1223}
m03_yuandan_point[3]={mapcrc=3017298383, x=2231, y=10964, z=2495}
m03_yuandan_point[4]={mapcrc=3017298383, x=2520, y=13100, z=1616}

m04_yuandan_point={}
m04_yuandan_point[1]={mapcrc=3017299663, x=2074, y=18342, z=2425}
m04_yuandan_point[2]={mapcrc=3017299663, x=1099, y=13199, z=1683}
m04_yuandan_point[3]={mapcrc=3017299663, x=3053, y=23613, z=1462}
m04_yuandan_point[4]={mapcrc=3017299663, x=2745, y=13900, z=800}

m05_yuandan_point={}
m05_yuandan_point[1]={mapcrc=3017299919, x=1190, y=20140, z=873}
m05_yuandan_point[2]={mapcrc=3017299919, x=1475, y=16700, z=1748}
m05_yuandan_point[3]={mapcrc=3017299919, x=2426, y=16352, z=2530}
m05_yuandan_point[4]={mapcrc=3017299919, x=2361, y=17572, z=865}

m06_yuandan_point={}
m06_yuandan_point[1]={mapcrc=3017299151, x=2089, y=6365, z=1791}
m06_yuandan_point[2]={mapcrc=3017299151, x=1044, y=3785, z=1402}
m06_yuandan_point[3]={mapcrc=3017299151, x=1145, y=7900, z=2098}
--m06_yuandan_point[4]={mapcrc=3017299151, x=1460, y=8801, z=500}

m07_yuandan_point={}
m07_yuandan_point[1]={mapcrc=3017299407, x=1892, y=14024, z=2476}
m07_yuandan_point[2]={mapcrc=3017299407, x=2294, y=9860, z=1386}
m07_yuandan_point[3]={mapcrc=3017299407, x=969, y=8226, z=2309}
m07_yuandan_point[4]={mapcrc=3017299407, x=1138, y=10497, z=1041}

allmap_yuandan_point={}
allmap_yuandan_point[1]={maptable=m02_yuandan_point, pointnum=4}
allmap_yuandan_point[2]={maptable=m03_yuandan_point, pointnum=4}
allmap_yuandan_point[3]={maptable=m04_yuandan_point, pointnum=4}
allmap_yuandan_point[4]={maptable=m05_yuandan_point, pointnum=4}
allmap_yuandan_point[5]={maptable=m06_yuandan_point, pointnum=3}
allmap_yuandan_point[6]={maptable=m07_yuandan_point, pointnum=4}

function YuanDan_OnStart(actID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 350)
	msg.DispatchWorldMsgEvent(MsgID)

	for i=1,6 do
		map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610102, Yuan_Dan_NPC[i].x, Yuan_Dan_NPC[i].y, Yuan_Dan_NPC[i].z)
		map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610103, Yuan_Dan_NPC[i].x-4, Yuan_Dan_NPC[i].y, Yuan_Dan_NPC[i].z)
	end

	YuanDan_TimerMin = 0

end

function YuanDan_OnTimerMin(actID)

	local curhour = os.date("%H")
	local curmin = os.date("%M")

	if (curhour=="12" or curhour=="15" or curhour=="18" or curhour=="21" or curhour=="00") and curmin=="00" then
		for i=1,6 do
			map.MapCreateCreature(Yuan_Dan_NPC[i].mapcrc, -1, 3610101, Yuan_Dan_NPC[i].x2, Yuan_Dan_NPC[i].y2, Yuan_Dan_NPC[i].z2)
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 352)
		msg.DispatchWorldMsgEvent(MsgID)
	end

	local time5 = {}
	time5[1]="00"
	time5[2]="05"
	time5[3]="10"
	time5[4]="15"
	time5[5]="20"
	time5[6]="25"
	time5[7]="30"
	time5[8]="35"
	time5[9]="40"
	time5[10]="45"
	time5[11]="50"
	time5[12]="55"

	local TimeIs5=0

	for i=1,12 do
		if curmin==time5[i] then
			TimeIs5=1
		end
	end


	if (curhour=="20" or curhour=="21") and TimeIs5==1 then

		for i=1,6 do
			local temppoint=math.random(allmap_yuandan_point[i].pointnum)
			local mapcrc=allmap_yuandan_point[i].maptable[temppoint].mapcrc
			local x=allmap_yuandan_point[i].maptable[temppoint].x
			local y=allmap_yuandan_point[i].maptable[temppoint].y
			local z=allmap_yuandan_point[i].maptable[temppoint].z

			map.MapCreateColCreature(mapcrc, -1, 1531210+i, x, y, z,1)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 1200+i*10+temppoint)
			msg.DispatchMapMsgEvent(MsgID, mapcrc, -1)
		end
	end

	YuanDan_TimerMin = YuanDan_TimerMin + 1

	local randindex = math.random(6)

	local mapindex = Yuan_Dan_NPC[randindex].mapcrc
	local monster = Yuan_Dan_NPC[randindex].monsterid
	local k = math.random(500,520)

	map.MapCreateCreature(mapindex, -1, monster, map_list[mapindex].map[k].x, map_list[mapindex].map[k].y, map_list[mapindex].map[k].z)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 351)
	msg.AddMsgEvent(MsgID, 6, mapindex)
	msg.DispatchMapMsgEvent(MsgID, mapindex, -1)

end

--注册该活动的相关事件
aux.RegisterActEvent(56, 2, "YuanDan_OnStart")

aux.RegisterActEvent(56, 4, "YuanDan_OnTimerMin")


--雪人调查事件
function C3610101_OnInVest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	--检测玩家是否有五色石，如果有则直接扣除

	local monsterHP = cre.GetCreatureScriptData(MapID, InstanceID, CreatureID, 1, 0)

	local nRet = role.RemoveFromRole(MapID, InstanceID, RoleID, 1350401, 1, 1000)

	if 0 == nRet then
		local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		level = math.floor(level/10)
		role.AddRoleExp(MapID, InstanceID, RoleID, Yuan_Dan_Exp[level])

		local i = math.random(100)

		if i>15 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 1350410, 1, -1, 8, 102)
		else
			role.AddRoleItem(MapID, InstanceID, RoleID, 1350411, 1, -1, 8, 102)
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 355)
		msg.AddMsgEvent(MsgID, 9, Yuan_Dan_Exp[level])	--经验值
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

		monsterHP=monsterHP+1
		cre.SetCreatureScriptData(MapID, InstanceID, CreatureID, 1, 0, monsterHP)

		if monsterHP%10 == 0 then
			unit.AddBuff(MapID, InstanceID, CreatureID, 5002801, CreatureID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 353)
			msg.DispatchMapMsgEvent(MsgID, MapID, -1)
		end
	else
		unit.ChangeHP(MapID, InstanceID, CreatureID, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 356)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end
aux.RegisterCreatureEvent(3610101, 6, "C3610101_OnInVest")

function C3610101_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

	local x, y ,z = unit.GetPosition(MapID, InstanceID, CreatureID)

	if MapID==3017298127 then
		map.MapCreateColCreature(MapID, InstanceID, 1531206, x, y, z,1)
	elseif MapID==3017298383 then
		map.MapCreateColCreature(MapID, InstanceID, 1531207, x, y, z,1)
	elseif MapID==3017299663 then
		map.MapCreateColCreature(MapID, InstanceID, 1531208, x, y, z,1)
	elseif MapID==3017299919 then
		map.MapCreateColCreature(MapID, InstanceID, 1531209, x, y, z,1)
	elseif MapID==3017299151 then
		map.MapCreateColCreature(MapID, InstanceID, 1531210, x, y, z,1)
	elseif MapID==3017299407 then
		map.MapCreateColCreature(MapID, InstanceID, 1531210, x, y, z,1)
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 354)
	msg.DispatchMapMsgEvent(MsgID, MapID, -1)
end

aux.RegisterCreatureEvent(3610101, 4, "C3610101_OnDie")

]]