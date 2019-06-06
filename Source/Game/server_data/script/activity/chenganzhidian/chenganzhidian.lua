--固定活动“承安之殿“

--地图ScriptData字段意义说明。0:杀死小怪的数量。1：杀死boss的数量。
--击杀Boss次数足够后刷出大BOSS

XiaoWuMoBing_Point = {}
XiaoWuMoBing_Point[1] = {x=1223, y=13551, z=2116}
XiaoWuMoBing_Point[2] = {x=1223, y=13551, z=2106}
XiaoWuMoBing_Point[3] = {x=1223, y=13551, z=2096}
XiaoWuMoBing_Point[4] = {x=1223, y=13551, z=2089}
XiaoWuMoBing_Point[5] = {x=1223, y=13551, z=2079}
XiaoWuMoBing_Point[6] = {x=1223, y=13551, z=2069}

XiaoWuMoBing_Point[7] = {x=1217, y=13617, z=2053}
XiaoWuMoBing_Point[8] = {x=1207, y=13617, z=2053}
XiaoWuMoBing_Point[9] = {x=1197, y=13617, z=2053}
XiaoWuMoBing_Point[10] = {x=1187, y=13617, z=2053}
XiaoWuMoBing_Point[11] = {x=1176, y=13617, z=2053}

XiaoWuMoBing_Point[12] = {x=1217, y=13617, z=2130}
XiaoWuMoBing_Point[13] = {x=1207, y=13617, z=2130}
XiaoWuMoBing_Point[14] = {x=1197, y=13617, z=2130}
XiaoWuMoBing_Point[15] = {x=1187, y=13617, z=2130}
XiaoWuMoBing_Point[16] = {x=1176, y=13617, z=2130}

XiaoWuMoBing_Point[17] = {x=1165, y=13457, z=2127}
XiaoWuMoBing_Point[18] = {x=1152, y=13457, z=2123}

XiaoWuMoBing_Point[19] = {x=1144, y=13636, z=2116}
XiaoWuMoBing_Point[20] = {x=1144, y=13636, z=2106}
XiaoWuMoBing_Point[21] = {x=1144, y=13636, z=2096}
XiaoWuMoBing_Point[22] = {x=1144, y=13636, z=2089}
XiaoWuMoBing_Point[23] = {x=1144, y=13636, z=2079}
XiaoWuMoBing_Point[24] = {x=1144, y=13636, z=2069}

XiaoWuMoBing_Point[25] = {x=1165, y=13457, z=2059}
XiaoWuMoBing_Point[26] = {x=1152, y=13457, z=2065}

XiaoWuMoBing_Point[27] = {x=1206, y=13457, z=2071}
XiaoWuMoBing_Point[28] = {x=1206, y=13457, z=2082}
XiaoWuMoBing_Point[29] = {x=1206, y=13457, z=2093}
XiaoWuMoBing_Point[30] = {x=1206, y=13457, z=2104}
XiaoWuMoBing_Point[31] = {x=1206, y=13457, z=2115}
XiaoWuMoBing_Point[32] = {x=1197, y=13457, z=2115}
XiaoWuMoBing_Point[33] = {x=1188, y=13457, z=2115}
XiaoWuMoBing_Point[34] = {x=1179, y=13457, z=2115}
XiaoWuMoBing_Point[35] = {x=1170, y=13457, z=2115}
XiaoWuMoBing_Point[36] = {x=1170, y=13457, z=2104}
XiaoWuMoBing_Point[37] = {x=1170, y=13457, z=2093}
XiaoWuMoBing_Point[38] = {x=1170, y=13457, z=2082}
XiaoWuMoBing_Point[39] = {x=1170, y=13457, z=2071}
XiaoWuMoBing_Point[40] = {x=1179, y=13457, z=2071}
XiaoWuMoBing_Point[41] = {x=1188, y=13457, z=2071}
XiaoWuMoBing_Point[42] = {x=1197, y=13457, z=2071}

act22_TimerMin = 0
Cheng_An_Bossnum = 0
function act22_OnStart(actID)

	--重置TimerMin

	act22_TimerMin = 0
	Cheng_An_Bossnum = 0

	--服务器涿鹿全场景通知[屏幕顶部滚动文字提示]：“***”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 100)
	msg.DispatchMapMsgEvent(MsgID, 3017299663, -1)

	--随机刷出20个轩辕组哨兵
	for k=500, 520 do
		local x = map_list[3017299663].map[k].x
		local y = map_list[3017299663].map[k].y
		local z = map_list[3017299663].map[k].z

		--生成怪物
		map.MapCreateCreature(3017299663, -1, 3020705, x, y, z)
	end
end


function act22_OnTimerMin(actID)

	local broadmin={}
	broadmin[1] = 9
	broadmin[2] = 19
	broadmin[3] = 29
	broadmin[4] = 39
	broadmin[5] = 49

	local respawnmin={}
	respawnmin[1] = 10
	respawnmin[2] = 20
	respawnmin[3] = 30
	respawnmin[4] = 40
	respawnmin[5] = 50

	act22_TimerMin = act22_TimerMin+1

	for i=1,5 do
		if act22_TimerMin == broadmin[i] then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 106)
			msg.DispatchMapMsgEvent(MsgID, 3017299663, -1)
		end
	end

	for i=1,5 do
		if act22_TimerMin == respawnmin[i] then
			--随机刷出20个轩辕组哨兵
			for k=500, 520 do
				local x = map_list[3017299663].map[k].x
				local y = map_list[3017299663].map[k].y
				local z = map_list[3017299663].map[k].z

				--生成怪物
				map.MapCreateCreature(3017299663, -1, 3020705, x, y, z)
			end
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(24, 2, "act22_OnStart")
aux.RegisterActEvent(25, 2, "act22_OnStart")
aux.RegisterActEvent(26, 2, "act22_OnStart")
aux.RegisterActEvent(27, 2, "act22_OnStart")

aux.RegisterActEvent(24, 4, "act22_OnTimerMin")
aux.RegisterActEvent(25, 4, "act22_OnTimerMin")
aux.RegisterActEvent(26, 4, "act22_OnTimerMin")
aux.RegisterActEvent(27, 4, "act22_OnTimerMin")

--与轩辕哨兵对话传送进副本

function x3020705_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)

	--判断玩家是否可以进入副本
	if TeamID == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 	103)	--您没有组队，不能进入该副本
		msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 	22)		--取消
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	else
		--获取队伍玩家等级
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if rolelevel < 45 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	104)	--您等级不足，不能进入该副本
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if TalkIndex == -1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	105)		--“是否进入副本”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
				msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			elseif TalkIndex == 4 and rolelevel >= 45 then
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092183860, 491, 6781, 725)

				map.MapDeleteCreature(MapID, InstanceID, TargetID)
			end
		end
	end
end
--注册轩辕族哨兵对话事件
aux.RegisterCreatureEvent(3020705, 7, "x3020705_OnTalk")

--清完小怪刷出BOSS
function x1522001_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--获得副本已杀小怪数量
	local CreatureNum = map.GetMapScriptData(MapID, InstanceID, 1, 0) + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 0, CreatureNum)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 107)
	msg.AddMsgEvent(MsgID, 9, CreatureNum)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	if CreatureNum == 30 then
		local a = map.GetMapScriptData(MapID, InstanceID, 1, 2)
		map.MapCreateCreature(MapID, InstanceID, a, 731, 6902, 616)
	end
end


function x1522004_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 6, "jinjieshili_A")

	--刷新NPC轩辕族哨兵
	map.MapCreateCreature(MapID, InstanceID, 3020706, 746, 6982, 628)

	--获得副本已杀Boss数量
	Cheng_An_Bossnum = Cheng_An_Bossnum+1

	if Cheng_An_Bossnum == 30 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 101)
		msg.DispatchMapMsgEvent(MsgID, 3017299663, -1)

		--刷出枭吴魔头
		map.MapCreateCreature(3017299663, -1, 1522053, 1190, 13457, 2093)

		--刷出承安殿将军
		map.MapCreateCreature(3017299663, -1, 3020704, 1157, 13457, 2093 )

		--刷出枭吴魔兵
		for k=1, 42 do
			map.MapCreateCreature(3017299663, -1, 1522054, XiaoWuMoBing_Point[k].x,XiaoWuMoBing_Point[k].y, XiaoWuMoBing_Point[k].z)
		end

	end
end

aux.RegisterCreatureEvent(1522001, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522002, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522003, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522005, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522006, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522007, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522009, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522010, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522011, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522013, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522014, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522015, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522017, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522018, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522019, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522021, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522022, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522023, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522025, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522026, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522027, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522029, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522030, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522031, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522033, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522034, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522035, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522037, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522038, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522039, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522041, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522042, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522043, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522045, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522046, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522047, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522049, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522050, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522051, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522055, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522056, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522057, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522059, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522060, 4, "x1522001_OnDie")
aux.RegisterCreatureEvent(1522061, 4, "x1522001_OnDie")


aux.RegisterCreatureEvent(1522004, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522008, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522012, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522016, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522020, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522024, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522028, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522032, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522036, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522040, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522044, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522048, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522052, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522062, 4, "x1522004_OnDie")
aux.RegisterCreatureEvent(1522058, 4, "x1522004_OnDie")

function x1522053_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 102)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchMapMsgEvent(MsgID, 3017299663, -1)
	Increase_shili(MapID, InstanceID, RoleID, 1, 30, "jinjieshili_A")
end
aux.RegisterCreatureEvent(1522053, 4, "x1522053_OnDie")


function x1522054_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 1, "jinjieshili_A")
end
aux.RegisterCreatureEvent(1522054, 4, "x1522054_OnDie")




function a01_OnPlayerEnter(MapID, InstanceID, RoleID)
	if map.GetMapScriptData(MapID, InstanceID, 1, 1) ~= 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 1, 1)--已经有人进入了副本
		local montypeid = chengan_monid(MapID, InstanceID, RoleID)--随机出的怪物等级
        if montypeid > 105 then
		    map.SetMapScriptData(MapID, InstanceID, 1, 2, 1522062)--记录随机任务目标ID

			montypeid = 1522059
		elseif montypeid > 100 then
	        map.SetMapScriptData(MapID, InstanceID, 1, 2, 1522058)--记录随机任务目标ID

			montypeid = 1522055
		else
			map.SetMapScriptData(MapID, InstanceID, 1, 2, 1522004 + ((montypeid-40)/5 )*4)--记录随机任务目标ID

			montypeid = 1522001 + ((montypeid-40)/5 )*4
		end

		local chengan_pos = {}
		chengan_pos[1] = {x=439,y=5767,z=764}
		chengan_pos[2] = {x=425,y=5620,z=768}
		chengan_pos[3] = {x=406,y=5731,z=759}
		chengan_pos[4] = {x=411,y=5722,z=747}
		chengan_pos[5] = {x=375,y=5423,z=783}
		chengan_pos[6] = {x=374,y=5488,z=775}
		chengan_pos[7] = {x=333,y=5494,z=752}
		chengan_pos[8] = {x=299,y=5269,z=750}
		chengan_pos[9] = {x=291,y=5382,z=741}
		chengan_pos[10] = {x=321,y=6506,z=459}
		chengan_pos[11] = {x=323,y=5642,z=680}
		chengan_pos[12] = {x=351,y=5613,z=635}
		chengan_pos[13] = {x=387,y=4520,z=563}
		chengan_pos[14] = {x=396,y=4473,z=568}
		chengan_pos[15] = {x=266,y=5695,z=709}
		chengan_pos[16] = {x=255,y=6270,z=654}
		chengan_pos[17] = {x=246,y=6200,z=656}
		chengan_pos[18] = {x=243,y=6500,z=600}
		chengan_pos[19] = {x=232,y=6500,z=555}
		chengan_pos[20] = {x=251,y=6500,z=543}
		chengan_pos[21] = {x=344,y=6320,z=446}
		chengan_pos[22] = {x=344,y=5957,z=406}
		chengan_pos[23] = {x=378,y=6140,z=426}
		chengan_pos[24] = {x=370,y=5999,z=400}
		chengan_pos[25] = {x=441,y=6315,z=455}
		chengan_pos[26] = {x=532,y=5861,z=470}
		chengan_pos[27] = {x=621,y=5871,z=519}
		chengan_pos[28] = {x=685,y=6477,z=569}
		chengan_pos[29] = {x=682,y=6382,z=618}
		chengan_pos[30] = {x=714,y=6705,z=592}
		chengan_pos[31] = {x=710,y=6653,z=630}
		chengan_pos[32] = {x=668,y=6213,z=590}

		for k = 1,32 do
			local x = math.random(0,2)
			map.MapCreateColCreature(MapID, InstanceID, montypeid+x, chengan_pos[k].x, chengan_pos[k].y, chengan_pos[k].z, 1)
		end
	end
end

--注册
aux.RegisterMapEvent("a01", 2, "a01_OnPlayerEnter")

--接取任务
function x20037_OnQuestInit(MapID, InstanceID, QuestID, RoleID, NPCID)
	local a = map.GetMapScriptData(MapID, InstanceID, 1, 2)
	local Quest_20037 = {}
	Quest_20037[1]={TargetType=QuestTargetType["Kill"], TargetID1=a, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=22001}------差msgID
	--初始化任务的动态数据
	role.QuestInit(RoleID, QuestID, GetRandQuestTable(Quest_20037, 1))
	--同步客户端总任务环数和任务说明
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 11, QuestID)
	msg.AddMsgEvent(MsgID, 1, Quest_20037[1].MsgID)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

end
aux.RegisterQuestEvent(20037, 7, "x20037_OnQuestInit")


--完成任务
function x20037_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local award = {}
	award[45]={jingyan=13561,money=70000}
	award[50]={jingyan=21090,money=75000}
	award[55]={jingyan=30982,money=80000}
	award[60]={jingyan=41019,money=85000}
	award[65]={jingyan=45983,money=90000}
	award[70]={jingyan=49302,money=95000}
	award[75]={jingyan=50627,money=100000}
	award[80]={jingyan=54056,money=105000}
	award[85]={jingyan=73195,money=110000}
	award[90]={jingyan=90376,money=115000}
	award[95]={jingyan=117944,money=120000}
	award[100]={jingyan=154201,money=125000}
	award[105]={jingyan=204201,money=145000}
	award[110]={jingyan=264201,money=165000}

	local level = map.GetMapScriptData(MapID, InstanceID, 1, 2)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if level > rolelevel then --在人物等级和BOSS等级之间选取小的给经验
		level = rolelevel
	end

	local k = math.floor(level/5)*5
	role.AddRoleExp(MapID, InstanceID, RoleID, award[k].jingyan)
	role.AddRoleSilver(MapID, InstanceID, RoleID, award[k].money, 102)
	role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1255, 13457, 2042)

end
aux.RegisterQuestEvent(20037, 1, "x20037_OnComplete")

