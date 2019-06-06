TuNian_NianshouCreID1 = 0
TuNian_NianshouCreID2 = 0

NianShou_LongCheng_Pos = {}
NianShou_LongCheng_Pos[1]={x=2211 ,y=6646 ,z=2240}
NianShou_LongCheng_Pos[2]={x=2204 ,y=6646 ,z=2234}
NianShou_LongCheng_Pos[3]={x=2209 ,y=6646 ,z=2225}
NianShou_LongCheng_Pos[4]={x=2217 ,y=6646 ,z=2227}
NianShou_LongCheng_Pos[5]={x=2218 ,y=6646 ,z=2235}
NianShou_LongCheng_Pos[6]={x=2153 ,y=6646 ,z=2202}
NianShou_LongCheng_Pos[7]={x=2296 ,y=5733 ,z=2206}
NianShou_LongCheng_Pos[8]={x=2196 ,y=5738 ,z=2139}
NianShou_LongCheng_Pos[9]={x=2213 ,y=5738 ,z=2113}
NianShou_LongCheng_Pos[10]={x=2160 ,y=5738 ,z=2126}

NianShou_HuangDi_Pos = {}
NianShou_HuangDi_Pos[1]={x=2024 ,y=18345 ,z=2737}
NianShou_HuangDi_Pos[2]={x=2012 ,y=18345 ,z=2753}
NianShou_HuangDi_Pos[3]={x=2182 ,y=18345 ,z=2707}
NianShou_HuangDi_Pos[4]={x=2195 ,y=18345 ,z=2749}
NianShou_HuangDi_Pos[5]={x=2142 ,y=18345 ,z=2689}
NianShou_HuangDi_Pos[6]={x=2084 ,y=18345 ,z=2686}
NianShou_HuangDi_Pos[7]={x=2064 ,y=18345 ,z=2671}
NianShou_HuangDi_Pos[8]={x=2084 ,y=18345 ,z=2660}
NianShou_HuangDi_Pos[9]={x=2118 ,y=18345 ,z=2663}
NianShou_HuangDi_Pos[10]={x=2126 ,y=18345 ,z=2677}
--活动开启
function Chunjie_OnStart(actID)
	if Chunjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100501)	--新春活动现在正式开始！打年兽，得财宝，开开心心过大年！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		CreatureID1 = map.MapCreateCreature(3017298127, -1, 4900716, 2184, 5738, 2130)  -- 龙城新春NPC位置
		CreatureID2 = map.MapCreateCreature(3017299663, -1, 4900716, 2105, 18345, 2713)	-- 黄帝城新春NPC位置
	end
end

function Chunjie_OnTimerMin(actID)
	local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	if curhour == 0 and curmin ==0 then
		XinnianHongseLibao = 0 --重置每日限制礼包产出数量
		XinnianXuancaiLibao = 0--重置每日限制礼包产出数量
	end
	if curhour >= 12 and curmin == 0 and curhour %2 == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100502) --年兽已经出现于玄天龙城和黄帝城，请各路英雄尽快将其驱赶，让众人开心过大年！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		NianshouCreID1 = map.MapCreateCreature(3017298127, -1, 1550301, 2160, 6646, 2234)	-- 龙城刷巨型年兽
		NianshouCreID2 = map.MapCreateCreature(3017299663, -1, 1550301, 2103, 18345, 2670)	-- 黄帝城刷巨型年兽
		TuNian_NianshouCreID1 = 1
		TuNian_NianshouCreID2 = 1
	elseif curhour >= 13 and curmin == 0 and curhour %2 == 1 then
		local Baoxiang_longcheng_Pos = {}
		Baoxiang_longcheng_Pos[1]={x=1987,y=3742,z=2947}
		Baoxiang_longcheng_Pos[2]={x=1648,y=6129,z=2994}
		Baoxiang_longcheng_Pos[3]={x=1886,y=8106,z=3289}
		Baoxiang_longcheng_Pos[4]={x=2362,y=10321,z=3479}
		Baoxiang_longcheng_Pos[5]={x=2482,y=7151,z=3550}
		Baoxiang_longcheng_Pos[6]={x=2873,y=7726,z=3463}
		Baoxiang_longcheng_Pos[7]={x=3626,y=3863,z=3174}
		Baoxiang_longcheng_Pos[8]={x=3314,y=3597,z=2205}
		Baoxiang_longcheng_Pos[9]={x=2785,y=6434,z=2617}
		Baoxiang_longcheng_Pos[10]={x=3608,y=3925,z=1888}
		Baoxiang_longcheng_Pos[11]={x=3498,y=3654,z=771}
		Baoxiang_longcheng_Pos[12]={x=2477,y=4194,z=1009}
		Baoxiang_longcheng_Pos[13]={x=2269,y=4237,z=1083}
		Baoxiang_longcheng_Pos[14]={x=2469,y=6818,z=405}
		Baoxiang_longcheng_Pos[15]={x=1848,y=8509,z=375}
		Baoxiang_longcheng_Pos[16]={x=1563,y=3552,z=894}
		Baoxiang_longcheng_Pos[17]={x=1788,y=3545,z=1213}
		Baoxiang_longcheng_Pos[18]={x=996,y=4296,z=1704}
		Baoxiang_longcheng_Pos[19]={x=1706,y=4681,z=632}
		Baoxiang_longcheng_Pos[20]={x=1507,y=5179,z=739}
		Baoxiang_longcheng_Pos[21]={x=869,y=6620,z=375}
		Baoxiang_longcheng_Pos[22]={x=434,y=6581,z=463}
		Baoxiang_longcheng_Pos[23]={x=608,y=8730,z=1448}
		Baoxiang_longcheng_Pos[24]={x=2032,y=4052,z=2951}
		Baoxiang_longcheng_Pos[25]={x=539,y=10496,z=1522}
		Baoxiang_longcheng_Pos[26]={x=801,y=4700,z=1640}
		Baoxiang_longcheng_Pos[27]={x=1455,y=5189,z=2400}
		Baoxiang_longcheng_Pos[28]={x=1819,y=4695,z=1936}
		Baoxiang_longcheng_Pos[29]={x=1231,y=4999,z=2785}
		Baoxiang_longcheng_Pos[30]={x=1417,y=3595,z=2779}
		Baoxiang_longcheng_Pos[31]={x=884,y=8523,z=3697}
		Baoxiang_longcheng_Pos[32]={x=1241,y=10224,z=3665}
		Baoxiang_longcheng_Pos[33]={x=1970,y=4612,z=1424}
		Baoxiang_longcheng_Pos[34]={x=1948,y=4574,z=1766}
		Baoxiang_longcheng_Pos[35]={x=2276,y=6781,z=2480}
		Baoxiang_longcheng_Pos[36]={x=2142,y=7759,z=2317}
		Baoxiang_longcheng_Pos[37]={x=1703,y=4568,z=1479}
		Baoxiang_longcheng_Pos[38]={x=1897,y=3515,z=2262}
		Baoxiang_longcheng_Pos[39]={x=2558,y=3570,z=2531}
		Baoxiang_longcheng_Pos[40]={x=2884,y=3993,z=1948}
		Baoxiang_longcheng_Pos[41]={x=3227,y=5045,z=1491}
		Baoxiang_longcheng_Pos[42]={x=3339,y=5544,z=1498}
		Baoxiang_longcheng_Pos[43]={x=3525,y=4285,z=2063}
		Baoxiang_longcheng_Pos[44]={x=2685,y=4026,z=1509}
		Baoxiang_longcheng_Pos[45]={x=2818,y=4503,z=1972}
		Baoxiang_longcheng_Pos[46]={x=3295,y=4672,z=1032}
		Baoxiang_longcheng_Pos[47]={x=3184,y=4003,z=589}
		Baoxiang_longcheng_Pos[48]={x=1333,y=3626,z=2642}
		Baoxiang_longcheng_Pos[49]={x=1881,y=3530,z=2937}
		Baoxiang_longcheng_Pos[50]={x=2415,y=7486,z=3276}

		local Baoxiang_zhuolu_Pos={}
		Baoxiang_zhuolu_Pos[1]={x=1002,y=15100,z=1036}
		Baoxiang_zhuolu_Pos[2]={x=1006,y=13000,z=919}
		Baoxiang_zhuolu_Pos[3]={x=1047,y=13199,z=1656}
		Baoxiang_zhuolu_Pos[4]={x=1064,y=13455,z=1830}
		Baoxiang_zhuolu_Pos[5]={x=1121,y=13000,z=790}
		Baoxiang_zhuolu_Pos[6]={x=1136,y=14629,z=1323}
		Baoxiang_zhuolu_Pos[7]={x=1142,y=20938,z=2998}
		Baoxiang_zhuolu_Pos[8]={x=1182,y=13457,z=2105}
		Baoxiang_zhuolu_Pos[9]={x=1404,y=16857,z=2607}
		Baoxiang_zhuolu_Pos[10]={x=1445,y=12323,z=1531}
		Baoxiang_zhuolu_Pos[11]={x=1558,y=20667,z=3154}
		Baoxiang_zhuolu_Pos[12]={x=1576,y=15697,z=1502}
		Baoxiang_zhuolu_Pos[13]={x=1686,y=14250,z=614}
		Baoxiang_zhuolu_Pos[14]={x=1693,y=13677,z=992}
		Baoxiang_zhuolu_Pos[15]={x=1767,y=14980,z=2636}
		Baoxiang_zhuolu_Pos[16]={x=1770,y=12355,z=1720}
		Baoxiang_zhuolu_Pos[17]={x=1775,y=12733,z=1293}
		Baoxiang_zhuolu_Pos[18]={x=1836,y=12351,z=1879}
		Baoxiang_zhuolu_Pos[19]={x=1859,y=13657,z=2882}
		Baoxiang_zhuolu_Pos[20]={x=1954,y=13519,z=2011}
		Baoxiang_zhuolu_Pos[21]={x=1979,y=13895,z=2327}
		Baoxiang_zhuolu_Pos[22]={x=2014,y=12654,z=1470}
		Baoxiang_zhuolu_Pos[23]={x=2034,y=12409,z=1656}
		Baoxiang_zhuolu_Pos[24]={x=2062,y=13558,z=2579}
		Baoxiang_zhuolu_Pos[25]={x=2093,y=12334,z=932}
		Baoxiang_zhuolu_Pos[26]={x=2119,y=12749,z=1052}
		Baoxiang_zhuolu_Pos[27]={x=2154,y=14103,z=2290}
		Baoxiang_zhuolu_Pos[28]={x=2173,y=19978,z=2460}
		Baoxiang_zhuolu_Pos[29]={x=2186,y=12807,z=1536}
		Baoxiang_zhuolu_Pos[30]={x=2190,y=22793,z=2794}
		Baoxiang_zhuolu_Pos[31]={x=2213,y=17728,z=3375}
		Baoxiang_zhuolu_Pos[32]={x=2351,y=13028,z=1701}
		Baoxiang_zhuolu_Pos[33]={x=2362,y=13149,z=1160}
		Baoxiang_zhuolu_Pos[34]={x=2415,y=12699,z=2288}
		Baoxiang_zhuolu_Pos[35]={x=2453,y=13806,z=828}
		Baoxiang_zhuolu_Pos[36]={x=2481,y=12872,z=1032}
		Baoxiang_zhuolu_Pos[37]={x=2514,y=12684,z=2066}
		Baoxiang_zhuolu_Pos[38]={x=2565,y=13373,z=2031}
		Baoxiang_zhuolu_Pos[39]={x=2622,y=12336,z=991}
		Baoxiang_zhuolu_Pos[40]={x=2690,y=20787,z=3373}
		Baoxiang_zhuolu_Pos[41]={x=2844,y=13903,z=775}
		Baoxiang_zhuolu_Pos[42]={x=2863,y=12720,z=2968}
		Baoxiang_zhuolu_Pos[43]={x=2886,y=12273,z=1000}
		Baoxiang_zhuolu_Pos[44]={x=2938,y=12771,z=520}
		Baoxiang_zhuolu_Pos[45]={x=2956,y=23959,z=1959}
		Baoxiang_zhuolu_Pos[46]={x=3025,y=19259,z=3126}
		Baoxiang_zhuolu_Pos[47]={x=3083,y=23612,z=1412}
		Baoxiang_zhuolu_Pos[48]={x=3156,y=17205,z=633}
		Baoxiang_zhuolu_Pos[49]={x=3177,y=18227,z=573}
		Baoxiang_zhuolu_Pos[50]={x=3318,y=17886,z=1005}
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100504) --帝瑞看到众英雄合力抵挡住了年兽的侵袭，非常欣慰，并在涿鹿与玄天龙城放置了大量宝箱用来奖励众英雄，大家快去寻找宝贝吧！！！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		local seedA = math.randomseed(os.time())
		for i = 1, 20 do
			local k = math.random(51-i)
			if i <= 12 then
				map.MapCreateCreature(3017298127, -1, 4900719, Baoxiang_longcheng_Pos[k].x, Baoxiang_longcheng_Pos[k].y, Baoxiang_longcheng_Pos[k].z)	-- 龙城刷小宝箱
				map.MapCreateCreature(3017299663, -1, 4900719, Baoxiang_zhuolu_Pos[k].x, Baoxiang_zhuolu_Pos[k].y, Baoxiang_zhuolu_Pos[k].z)	-- 涿鹿刷小宝箱
			elseif i <= 18 and i >12 then
				map.MapCreateCreature(3017298127, -1, 4900718, Baoxiang_longcheng_Pos[k].x, Baoxiang_longcheng_Pos[k].y, Baoxiang_longcheng_Pos[k].z)	-- 龙城刷中宝箱
				map.MapCreateCreature(3017299663, -1, 4900718, Baoxiang_zhuolu_Pos[k].x, Baoxiang_zhuolu_Pos[k].y, Baoxiang_zhuolu_Pos[k].z)	-- 涿鹿刷中宝箱
			elseif i > 18 then
				map.MapCreateCreature(3017298127, -1, 4900717, Baoxiang_longcheng_Pos[k].x, Baoxiang_longcheng_Pos[k].y, Baoxiang_longcheng_Pos[k].z)	-- 龙城刷大宝箱
				map.MapCreateCreature(3017299663, -1, 4900717, Baoxiang_zhuolu_Pos[k].x, Baoxiang_zhuolu_Pos[k].y, Baoxiang_zhuolu_Pos[k].z)	-- 涿鹿刷大宝箱
			end
			table.remove(Baoxiang_longcheng_Pos,k)
			table.remove(Baoxiang_zhuolu_Pos,k)
		end
	end
end
--活动结束
function Chunjie_OnEnd(actID)
	if Chunjie == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100503)	--新春活动已结束，祝大家兔年快乐，万事如意！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
		if CreatureID1 ~= nil and CreatureID1 ~= 0 and CreatureID2 ~= nil and CreatureID2 ~= 0 then
			map.MapDeleteCreature(3017298127, -1, CreatureID1)
			map.MapDeleteCreature(3017299663, -1, CreatureID2)
		end
	end
end
--城中巨型年兽死亡后刷小年兽
function C1550301_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	for k = 1,10 do
		if MapID == 3017298127 then
			TuNian_NianshouCreID1 = 0
			map.MapCreateColCreature(3017298127, -1, 1534103, NianShou_LongCheng_Pos[k].x, NianShou_LongCheng_Pos[k].y, NianShou_LongCheng_Pos[k].z, 1)
		elseif MapID == 3017299663 then
			TuNian_NianshouCreID2 = 0
			map.MapCreateColCreature(3017299663, -1, 1534103, NianShou_HuangDi_Pos[k].x, NianShou_HuangDi_Pos[k].y, NianShou_HuangDi_Pos[k].z, 1)
		end
	end
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 1354)--位于(地名)的年兽已经被消灭掉，于城中发生小年兽暴动事件。
	msg.AddMsgEvent(MsgID, 6, MapID)
	msg.DispatchMapMsgEvent(MsgID, MapID, -1)
end
--年兽之王死后获得蓝色礼包
function C1550302_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if MapID == 3000522703 then
		if TeamID ~= nil and TeamID ~= 4294967295 then
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
					if Target_MapID == MapID then
						if role.GetBagFreeSize(Role[i]) > 0 then
							role.AddRoleItem(MapID, InstanceID, Role[i], 2616508, 1, -1, 8, 420)  --击杀饥饿的年兽之王后给该副本内的队伍玩家发道具新年蓝色礼包
						end
					end
				end
			end
		else
			role.AddRoleItem(MapID, InstanceID, RoleID, 2616508, 1, -1, 8, 420) --若是单人VIP进入副本成功击杀的话，则只给该角色自己添加道具！
		end
	end
end
--判断芝麻杆是否可用
function I2616506_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	if MapID ~= 3000522703 or x > 740 or x < 720 or z > 610 or z < 590 then
		bRet = 43					--若不在副本内或不在规定区域内无法使用该道具
	end
	return bRet, bIgnore
end
--芝麻杆使用召唤年兽之王
function I2616506_OnUse(MapID,InstanceID,TypeID,RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= nil and TeamID ~= 4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		local MAX_level  = 0
		local MAX_NUM = 0
		local ave_level = 0
		for i = 1,6 do
			if Role[i] and Role[i]~= 4294967295 then
				local level = role.GetRoleLevel(MapID, InstanceID, Role[i]) --玩家等级
				if level ~= nil then
					MAX_level = MAX_level + level
					MAX_NUM = MAX_NUM + 1
				end
			end
		end
		ave_level = math.floor(MAX_level/MAX_NUM)				--小队玩家平均等级
		if ave_level >= 100 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550302, 740, 7246, 590, 1)   -- 刷出100级饥饿的年兽之王
		elseif ave_level >= 90 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550303, 740, 7246, 590, 1)   -- 刷出90级饥饿的年兽之王
		elseif ave_level >= 80 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550304, 740, 7246, 590, 1)   -- 刷出80级饥饿的年兽之王
		elseif ave_level >= 70 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550305, 740, 7246, 590, 1)   -- 刷出70级饥饿的年兽之王
		elseif ave_level >= 60 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550306, 740, 7246, 590, 1)   -- 刷出60级饥饿的年兽之王
		elseif ave_level >= 50 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550307, 740, 7246, 590, 1)   -- 刷出50级饥饿的年兽之王
		else
			map.MapCreateColCreature(3000522703, InstanceID, 1550308, 740, 7246, 590, 1)   -- 刷出40级饥饿的年兽之王
		end
	elseif TeamID == nil or TeamID == 4294967295 then
		local singlelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if singlelevel >= 100 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550302, 740, 7246, 590, 1)   -- 刷出100级饥饿的年兽之王
		elseif singlelevel >= 90 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550303, 740, 7246, 590, 1)   -- 刷出90级饥饿的年兽之王
		elseif singlelevel >= 80 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550304, 740, 7246, 590, 1)   -- 刷出80级饥饿的年兽之王
		elseif singlelevel >= 70 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550305, 740, 7246, 590, 1)   -- 刷出70级饥饿的年兽之王
		elseif singlelevel >= 60 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550306, 740, 7246, 590, 1)   -- 刷出60级饥饿的年兽之王
		elseif singlelevel >= 50 then
			map.MapCreateColCreature(3000522703, InstanceID, 1550307, 740, 7246, 590, 1)   -- 刷出50级饥饿的年兽之王
		else
			map.MapCreateColCreature(3000522703, InstanceID, 1550308, 740, 7246, 590, 1)   -- 刷出40级饥饿的年兽之王
		end
	end
end

function x20336_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if TuNian_NianshouCreID1 > 0 or TuNian_NianshouCreID2 > 0 then
		return 1
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 12612)  --现在没有年兽存活您无须变身
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
end


aux.RegisterQuestEvent(20336, 4, "x20336_OnCheckAccept")
aux.RegisterActEvent(159, 2, "Chunjie_OnStart")
aux.RegisterActEvent(159, 3, "Chunjie_OnEnd")
aux.RegisterActEvent(159, 4, "Chunjie_OnTimerMin")
aux.RegisterCreatureEvent(1550301, 4, "C1550301_OnDie")
aux.RegisterItemEvent(2616506, 0, "I2616506_CanUse")
aux.RegisterItemEvent(2616506, 1, "I2616506_OnUse")
aux.RegisterCreatureEvent(1550302, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550303, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550304, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550305, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550306, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550307, 4, "C1550302_OnDie")
aux.RegisterCreatureEvent(1550308, 4, "C1550302_OnDie")

