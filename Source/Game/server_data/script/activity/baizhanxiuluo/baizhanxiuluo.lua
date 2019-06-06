--地图ScriptData字段意义说明。
--[[0:是否第一个人进来。
    1：记录困魔天球的ID。
	2:记录现在剩余的怪物数。
	3：记录现在的波数
	4:记录已经交任务的人的个数
	]]

BaiZhanXiuLuo_Point = {}
BaiZhanXiuLuo_Point[1] = {x=119, y=449, z=121}  --传送坐标
BaiZhanXiuLuo_Point[2] = {x=120, y=443, z=112}  --BOSS坐标
BaiZhanXiuLuo_Point[3] = {x=101, y=98, z=162}
BaiZhanXiuLuo_Point[4] = {x=121, y=98, z=182}
BaiZhanXiuLuo_Point[5] = {x=122, y=98, z=160}
BaiZhanXiuLuo_Point[6] = {x=90, y=98, z=171}
BaiZhanXiuLuo_Point[7] = {x=76, y=44, z=110}
BaiZhanXiuLuo_Point[8] = {x=71, y=50, z=95}
BaiZhanXiuLuo_Point[9] = {x=84, y=98, z=82}
BaiZhanXiuLuo_Point[10] = {x=88, y=98, z=92}
BaiZhanXiuLuo_Point[11] = {x=110, y=98, z=60}
BaiZhanXiuLuo_Point[12] = {x=102, y=98, z=81}
BaiZhanXiuLuo_Point[13] = {x=120, y=98, z=69}
BaiZhanXiuLuo_Point[14] = {x=134, y=108, z=82}
BaiZhanXiuLuo_Point[15] = {x=143, y=108, z=67}
BaiZhanXiuLuo_Point[16] = {x=161, y=108, z=91}
BaiZhanXiuLuo_Point[17] = {x=149, y=108, z=84}
BaiZhanXiuLuo_Point[18] = {x=142, y=108, z=77}
BaiZhanXiuLuo_Point[19] = {x=167, y=115, z=64}
BaiZhanXiuLuo_Point[20] = {x=181, y=108, z=68}
BaiZhanXiuLuo_Point[21] = {x=175, y=88, z=110}
BaiZhanXiuLuo_Point[22] = {x=176, y=88, z=121}
BaiZhanXiuLuo_Point[23] = {x=185, y=93, z=130}
BaiZhanXiuLuo_Point[24] = {x=167, y=88, z=129}
BaiZhanXiuLuo_Point[25] = {x=185, y=88, z=150}
BaiZhanXiuLuo_Point[26] = {x=175, y=40, z=177}
BaiZhanXiuLuo_Point[27] = {x=152, y=79, z=172}
BaiZhanXiuLuo_Point[28] = {x=110, y=98, z=169}
BaiZhanXiuLuo_Point[29] = {x=97, y=106, z=150}
BaiZhanXiuLuo_Point[30] = {x=79, y=44, z=103}
BaiZhanXiuLuo_Point[31] = {x=74, y=118, z=74}
BaiZhanXiuLuo_Point[32] = {x=101, y=98, z=65}
BaiZhanXiuLuo_Point[33] = {x=175, y=128, z=92}
BaiZhanXiuLuo_Point[34] = {x=172, y=108, z=76}
BaiZhanXiuLuo_Point[35] = {x=178, y=88, z=136}
BaiZhanXiuLuo_Point[36] = {x=168, y=88, z=142}
BaiZhanXiuLuo_Point[37] = {x=154, y=88, z=136}
BaiZhanXiuLuo_Point[38] = {x=165, y=88, z=106}
BaiZhanXiuLuo_Point[39] = {x=164, y=86, z=174}
BaiZhanXiuLuo_Point[40] = {x=86, y=98, z=67}
BaiZhanXiuLuo_Point[41] = {x=111, y=98, z=73}
BaiZhanXiuLuo_Point[42] = {x=94, y=98, z=75}
BaiZhanXiuLuo_Point[43] = {x=155, y=108, z=66}
BaiZhanXiuLuo_Point[44] = {x=182, y=108, z=81}
BaiZhanXiuLuo_Point[45] = {x=161, y=108, z=80}
BaiZhanXiuLuo_Point[46] = {x=162, y=88, z=116}
BaiZhanXiuLuo_Point[47] = {x=160, y=108, z=149}
BaiZhanXiuLuo_Point[48] = {x=136, y=118, z=176}
BaiZhanXiuLuo_Point[49] = {x=191, y=96, z=114}
BaiZhanXiuLuo_Point[50] = {x=179, y=40, z=165}
BaiZhanXiuLuo_Point[51] = {x=79, y=0, z=153}
BaiZhanXiuLuo_Point[52] = {x=179, y=40, z=165}


baizhan_TimerMin = 0
baizhanquest_complete = {}
baizhaninstance_times = {}
baizhan_creature = {}


function baizhan_OnStart(actID)

	--重置TimerMin
	baizhan_TimerMin = 1
    baizhanquest_complete = {}
	baizhaninstance_times = {}
	baizhan_creature = {}

	--服务器涿鹿全场景通知[屏幕顶部滚动文字提示]：“***”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100102)  --百斩修罗活动已经开始，只要顶住十波妖魔的猛烈攻击就可以获得大量的经验和金钱！大家快点到涿鹿的【2094,2710】找罗刹参与活动吧！
	msg.DispatchBroadcast(MsgID, -1, -1, -1)

	baizhan_creature[1] = map.MapCreateCreature(3017299663, -1, 3610207, 2094, 18339, 2710) --刷新罗刹
	baizhan_creature[2] = map.MapCreateCreature(3017299919, -1, 3610207, 1016, 20140, 975)
	baizhan_creature[3] = map.MapCreateCreature(3017299151, -1, 3610207, 2033, 6365, 1799)
	baizhan_creature[4] = map.MapCreateCreature(3017299407, -1, 3610207, 1817, 14024, 2280)

	msg.SendWorldSwitchMsg(1, 40, 55, 1, actID)
end


function baizhan_OnTimerMin(actID)
	baizhan_TimerMin = baizhan_TimerMin + 1
	if baizhan_TimerMin == 2 then
	    msg.SendWorldSwitchMsg(2, 56, 75, 1, actID)
	elseif baizhan_TimerMin == 3 then
	    msg.SendWorldSwitchMsg(3, 76, 90, 1, actID)
	elseif baizhan_TimerMin == 4 then
	    msg.SendWorldSwitchMsg(4, 91, 110, 1, actID)
	end
	if baizhan_TimerMin <= 10 and baizhan_TimerMin % 5 == 0 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100102)  --百斩修罗活动已经开始，只要顶住十波妖魔的猛烈攻击就可以获得大量的经验和金钱！大家快点到涿鹿的【2094,2710】找罗刹参与活动吧！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
	end

end

-- npc生存时间到，活动结束，在各城市中删除此npc
function baizhan_OnEnd(actID)

	map.MapDeleteCreature(3017299663, -1, baizhan_creature[1])
	map.MapDeleteCreature(3017299919, -1, baizhan_creature[2])
	map.MapDeleteCreature(3017299151, -1, baizhan_creature[3])
	map.MapDeleteCreature(3017299407, -1, baizhan_creature[4])

end

function baizhan_OnClick(MapID, InstanceID, RoleID, TOMsgID, ToMapID, x, y, z)
    if role.IsRoleInStatus(RoleID, 4096) then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3177) -- 您正处于牢狱之中，无法使用该传送功能
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
	else
		if baizhan_TimerMin >= 1 and baizhan_TimerMin <= 4 then
			if role.IsRoleInStatus(RoleID, 1024) then
				role.CancelRoleStallState(MapID, InstanceID, RoleID)
			end
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, ToMapID, x, y, z)      --传送
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 2779) -- 您点击确定的时间过晚，已经超出了1分钟的时间限制，请自行前往参加活动。
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(127, 8, "baizhan_OnClick")
aux.RegisterActEvent(128, 8, "baizhan_OnClick")
aux.RegisterActEvent(129, 8, "baizhan_OnClick")
aux.RegisterActEvent(130, 8, "baizhan_OnClick")
aux.RegisterActEvent(131, 8, "baizhan_OnClick")
aux.RegisterActEvent(132, 8, "baizhan_OnClick")
aux.RegisterActEvent(135, 8, "baizhan_OnClick")
aux.RegisterActEvent(136, 8, "baizhan_OnClick")
aux.RegisterActEvent(137, 8, "baizhan_OnClick")
aux.RegisterActEvent(138, 8, "baizhan_OnClick")

--注册该活动的相关事件
aux.RegisterActEvent(127, 2, "baizhan_OnStart")
aux.RegisterActEvent(128, 2, "baizhan_OnStart")
aux.RegisterActEvent(129, 2, "baizhan_OnStart")
aux.RegisterActEvent(130, 2, "baizhan_OnStart")
aux.RegisterActEvent(131, 2, "baizhan_OnStart")
aux.RegisterActEvent(132, 2, "baizhan_OnStart")
aux.RegisterActEvent(135, 2, "baizhan_OnStart")
aux.RegisterActEvent(136, 2, "baizhan_OnStart")
aux.RegisterActEvent(137, 2, "baizhan_OnStart")
aux.RegisterActEvent(138, 2, "baizhan_OnStart")

aux.RegisterActEvent(127, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(128, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(129, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(130, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(131, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(132, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(135, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(136, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(137, 4, "baizhan_OnTimerMin")
aux.RegisterActEvent(138, 4, "baizhan_OnTimerMin")

aux.RegisterActEvent(127, 3, "baizhan_OnEnd")
aux.RegisterActEvent(128, 3, "baizhan_OnEnd")
aux.RegisterActEvent(129, 3, "baizhan_OnEnd")
aux.RegisterActEvent(130, 3, "baizhan_OnEnd")
aux.RegisterActEvent(131, 3, "baizhan_OnEnd")
aux.RegisterActEvent(132, 3, "baizhan_OnEnd")
aux.RegisterActEvent(135, 3, "baizhan_OnEnd")
aux.RegisterActEvent(136, 3, "baizhan_OnEnd")
aux.RegisterActEvent(137, 3, "baizhan_OnEnd")
aux.RegisterActEvent(138, 3, "baizhan_OnEnd")

--与罗刹传送进副本

function x3610207_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    --获取队伍玩家等级
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	2762)		--“罗刹：\n     <color=0xffff0000>40<color=0xfffff7e0>级以上的玩家进入修罗界完成我交给你的挑战任务可以获得大量的经验。您想现在就进入修罗界完成挑战吗？”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	2766)			--“接取挑战任务”
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	2767)			--“进入修罗界”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 6 then
		if baizhanquest_complete[RoleID] == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2764)	--本次活动中您已经完成过一次挑战任务，请下次活动开始时再来。
			msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 22)		--取消
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local bool = false
			for i=20299,20310 do
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, i) == true then
					bool = true
				end
			end
			if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20264) == true then
				bool = true
			end
			if bool == true then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2765)	--您已经接取过任务了，不需要重复接取。
				msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 22)		--取消
				msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif rolelevel < 40 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local quest = 0
				if rolelevel >=110 then
				    quest = 20328
				elseif rolelevel >= 105 then
				    quest = 20327
				elseif rolelevel >= 100 then
					quest = 20310
				elseif rolelevel >= 95 and rolelevel <= 99 then
					quest = 20309
				elseif rolelevel >= 90 and rolelevel <= 94 then
					quest = 20308
				elseif rolelevel >= 85 and rolelevel <= 89 then
					quest = 20307
				elseif rolelevel >= 80 and rolelevel <= 84 then
					quest = 20306
				elseif rolelevel >= 75 and rolelevel <= 79 then
					quest = 20305
				elseif rolelevel >= 70 and rolelevel <= 74 then
					quest = 20304
				elseif rolelevel >= 65 and rolelevel <= 69 then
					quest = 20303
				elseif rolelevel >= 60 and rolelevel <= 64 then
					quest = 20302
				elseif rolelevel >= 55 and rolelevel <= 59 then
					quest = 20301
				elseif rolelevel >= 50 and rolelevel <= 54 then
					quest = 20300
				elseif rolelevel >= 45 and rolelevel <= 49 then
					quest = 20299
				elseif rolelevel >= 40 and rolelevel <= 44 then
					quest = 20264
				end
				if quest ~= 0 then
				    role.AddQuest(RoleID, quest)
				end
			end
		end
	elseif TalkIndex == 4 then
		--判断玩家是否可以进入副本
		if TeamID == 4294967295 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	2812)	--您没有组队，不能进入该副本
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			local Num = 0
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					Num = Num + 1
				end
			end
			if rolelevel < 40 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif Num < 4 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2761)	--您需要凑够4个队友才能参加此活动
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local bool = false
				for i=20299,20310 do
					if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, i) == true then
						bool = true
					end
				end
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20264) == true then
					bool = true
				end
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20327) == true then
					bool = true
				end
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20328) == true then
					bool = true
				end
				if bool == false then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 	2763)	--您需要接取了挑战任务之后才能进入修罗界
					msg.AddMsgEvent(MsgID, 21, 	-1)		--取消按钮
					msg.AddMsgEvent(MsgID, 1, 	22)		--取消
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif bool == true then
				    if baizhaninstance_times[RoleID] ~= nil and baizhaninstance_times[RoleID] > 9 then
					    local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 2774)	--每次活动您只能进入9次修罗界，您已经达到此上限，请下次活动再来。
						msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
						msg.AddMsgEvent(MsgID, 1, 22)		--取消
						msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
						if baizhaninstance_times[RoleID] == nil then
							baizhaninstance_times[RoleID] = 1
						else
							baizhaninstance_times[RoleID] = baizhaninstance_times[RoleID] + 1
						end
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092185396, BaiZhanXiuLuo_Point[1].x, math.floor(BaiZhanXiuLuo_Point[1].y / 50), BaiZhanXiuLuo_Point[1].z)
					end
				end
			end
		end
	end
end
--注册轩辕族哨兵对话事件
aux.RegisterCreatureEvent(3610207, 7, "x3610207_OnTalk")

function a07_OnPlayerEnter(MapID, InstanceID, RoleID)


	if map.GetMapScriptData(MapID, InstanceID, 1, 0) ~= 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 0, 1)--已经有人进入了副本
		local kunmotianqiuid = map.MapCreateCreature(MapID, InstanceID, 3610208,BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z) --刷新困魔天球

		map.SetMapScriptData(MapID, InstanceID, 1, 1, kunmotianqiuid)
    end

end

--注册
aux.RegisterMapEvent("a07", 2, "a07_OnPlayerEnter")

--与罗刹传送进副本

function x3610208_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	--判断玩家是否可以进入副本
	if TalkIndex == -1 then
	    local Wave = map.GetMapScriptData(MapID, InstanceID, 1, 3)
		--[[if Wave == 10 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	2768)	--您想要离开修罗界吗？
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21, 	6)		--离开按钮
			msg.AddMsgEvent(MsgID, 1, 	2769)		--离开这里
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else]]
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	2770)	--您想要施放困魔天球中囚禁的魔物吗？
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21, 	4)		--确定按钮
			msg.AddMsgEvent(MsgID, 1, 	2771)		--释放魔物
			--msg.AddMsgEvent(MsgID, 21, 	6)		--离开按钮
			--msg.AddMsgEvent(MsgID, 1, 	2769)		--离开这里
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		--end
	elseif TalkIndex == 4 then     --计算应该刷出什么难度的怪物
	    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		if TeamID ~= 4294967295 then
			--[[local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			local Num = 0
			local totallevel = 0
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
                    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
					local rolelevel = role.GetRoleLevel(Target_MapID, Target_InstanceID, Role[i])
					if rolelevel >=40 then
						Num = Num + 1
						totallevel = totallevel + rolelevel
					end
				end
			end
	 		local average_level = math.floor(totallevel/Num)
			local over_aver = 0
			local over_Num = 0
			local over = 0
            for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
					local rolelevel = role.GetRoleLevel(Target_MapID, Target_InstanceID, Role[i])
					if rolelevel - average_level >= 0 then
						over_Num = over_Num + 1
						over = over + rolelevel
					end
				end
			end
			over_aver = math.floor(over/over_Num)
			average_level = math.floor(((over_aver-average_level)/over_aver+1)*average_level)]]
			local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
            local Wave = map.GetMapScriptData(MapID, InstanceID, 1, 3) + 1
			map.SetMapScriptData(MapID, InstanceID, 1, 3, Wave)
		    baizhan_CreatTenCreature(MapID, InstanceID, rolelevel, Wave)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100103)    --第<color=0xffff0000>X<color=0xfffff7e0>波魔物出现
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		else
		    local average_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local Wave = map.GetMapScriptData(MapID, InstanceID, 1, 3) + 1
			map.SetMapScriptData(MapID, InstanceID, 1, 3, Wave)
		    baizhan_CreatTenCreature(MapID, InstanceID, average_level, Wave)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100103)    --第<color=0xffff0000>X<color=0xfffff7e0>波魔物出现
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		end

		map.MapDeleteCreature(MapID, InstanceID, TargetID)

	elseif TalkIndex == 6 then

		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 2094, 18339, 2701)
	end
end
--注册
aux.RegisterCreatureEvent(3610208, 7, "x3610208_OnTalk")

--清完小怪刷出天球
function baizhan_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Num = map.GetMapScriptData(MapID, InstanceID, 1, 2)
	Num = Num - 1
	map.SetMapScriptData(MapID, InstanceID, 1, 2, Num)
	local Wave = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	if Num == 0 and Wave ~= 10 then
	    local kunmotianqiuid = map.MapCreateCreature(MapID, InstanceID, 3610208, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z) --刷新困魔天球
		map.SetMapScriptData(MapID, InstanceID, 1, 1, kunmotianqiuid)
		local a = math.random(100)
	    if a <= 50 then
		    map.MapCreateCreature(MapID, InstanceID, 1535168, BaiZhanXiuLuo_Point[3].x, BaiZhanXiuLuo_Point[3].y, BaiZhanXiuLuo_Point[3].z)
		end
		baizhan_addexp(MapID, InstanceID,RoleID,Wave)
	elseif Num == 0 and Wave == 10 then
	    map.MapCreateCreature(MapID, InstanceID, 3610209, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z)
		--map.MapCreateCreature(MapID, InstanceID, 3610208, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z) --刷新困魔天球
		baizhan_addexp(MapID, InstanceID,RoleID,Wave)
	else
	    local Num2 = 0
		if Wave == 1 then
		    Num2 = 11
		elseif Wave == 2 then
		    Num2 = 16
		elseif Wave == 3 then
		    Num2 = 21
        elseif Wave == 4 then
		    Num2 = 21
        elseif Wave == 5 then
		    Num2 = 21
		elseif Wave == 6 then
		    Num2 = 21
		elseif Wave == 7 then
		    Num2 = 21
		elseif Wave == 8 then
		    Num2 = 21
		elseif Wave == 9 then
		    Num2 = 1
		elseif Wave == 10 then
		    Num2 = 51
		end
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2773)    --第X波剩余怪物数量：XX/XX
		msg.AddMsgEvent(MsgID, 9, Wave)
		msg.AddMsgEvent(MsgID, 9, Num)
		msg.AddMsgEvent(MsgID, 9, Num2)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	end

end

aux.RegisterCreatureEvent(1535000, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535001, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535002, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535003, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535004, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535005, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535006, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535007, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535008, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535009, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535010, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535011, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535012, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535013, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535014, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535015, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535016, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535017, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535018, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535019, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535020, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535021, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535022, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535023, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535024, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535025, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535026, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535027, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535028, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535029, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535030, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535031, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535032, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535033, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535034, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535035, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535036, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535037, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535038, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535039, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535040, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535041, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535042, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535043, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535044, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535045, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535046, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535047, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535048, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535049, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535050, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535051, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535052, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535053, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535054, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535055, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535056, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535057, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535058, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535059, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535060, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535061, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535062, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535063, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535064, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535065, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535066, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535067, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535068, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535069, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535070, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535071, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535072, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535073, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535074, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535075, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535076, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535077, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535078, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535079, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535080, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535081, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535082, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535083, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535084, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535085, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535086, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535087, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535088, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535089, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535090, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535091, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535092, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535093, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535094, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535095, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535096, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535097, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535098, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535099, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535100, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535101, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535102, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535103, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535104, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535105, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535106, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535107, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535108, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535109, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535110, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535111, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535112, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535113, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535114, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535115, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535116, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535117, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535118, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535119, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535120, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535121, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535122, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535123, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535124, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535125, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535126, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535127, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535128, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535129, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535130, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535131, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535132, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535133, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535134, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535135, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535136, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535137, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535138, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535139, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535140, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535141, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535142, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535143, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535144, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535145, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535146, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535147, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535148, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535149, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535150, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535151, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535152, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535153, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535154, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535155, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535156, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535157, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535158, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535159, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535160, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535161, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535162, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535163, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535164, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535165, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535166, 4, "baizhan_OnDie")
aux.RegisterCreatureEvent(1535167, 4, "baizhan_OnDie")





function x1535168_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= 4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then

				local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
				if Target_MapID == MapID then
					unit.AddBuff(Target_MapID, Target_InstanceID, Role[i], 9429401, Role[i])
				end
			end
		end
	else
	    unit.AddBuff(MapID, InstanceID, RoleID, 9429401, RoleID)
	end
end

aux.RegisterCreatureEvent(1535168, 4, "x1535168_OnDie")

--完成任务
function x20264_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	baizhanquest_complete[RoleID] = 1
    map.SetMapScriptData(MapID, InstanceID, 1, 4, map.GetMapScriptData(MapID, InstanceID, 1, 4) + 1)
	if guangbo_baizhan == 0 then
	    guangbo_baizhan = 1
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100104)  --【江湖传闻】XXX完成了百斩修罗的试炼，获得了XXX点经验！
		msg.AddMsgEvent(MsgID, 2, RoleID)
		if QuestID == 20264 then
		    msg.AddMsgEvent(MsgID, 9, 46786)
		elseif QuestID == 20299 then
		    msg.AddMsgEvent(MsgID, 9, 87946)
		elseif QuestID == 20300 then
		    msg.AddMsgEvent(MsgID, 9, 136769)
		elseif QuestID == 20301 then
		    msg.AddMsgEvent(MsgID, 9, 200918)
		elseif QuestID == 20302 then
		    msg.AddMsgEvent(MsgID, 9, 266011)
		elseif QuestID == 20303 then
		    msg.AddMsgEvent(MsgID, 9, 298201)
		elseif QuestID == 20304 then
		    msg.AddMsgEvent(MsgID, 9, 319728)
		elseif QuestID == 20305 then
		    msg.AddMsgEvent(MsgID, 9, 328322)
		elseif QuestID == 20306 then
		    msg.AddMsgEvent(MsgID, 9, 350559)
		elseif QuestID == 20307 then
		    msg.AddMsgEvent(MsgID, 9, 474676)
		elseif QuestID == 20308 then
		    msg.AddMsgEvent(MsgID, 9, 586095)
		elseif QuestID == 20309 then
		    msg.AddMsgEvent(MsgID, 9, 764874)
		elseif QuestID == 20310 then
		    msg.AddMsgEvent(MsgID, 9, 1000000)
		elseif QuestID == 20327 then
		    msg.AddMsgEvent(MsgID, 9, 1200000)
		elseif QuestID == 20328 then
		    msg.AddMsgEvent(MsgID, 9, 1450000)
		end
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
	else
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100104)  --【江湖传闻】XXX通过了百斩修罗的考验，获得了XXX点经验！
		msg.AddMsgEvent(MsgID, 2, RoleID)
		if QuestID == 20264 then
		    msg.AddMsgEvent(MsgID, 9, 46786)
		elseif QuestID == 20299 then
		    msg.AddMsgEvent(MsgID, 9, 87946)
		elseif QuestID == 20300 then
		    msg.AddMsgEvent(MsgID, 9, 136769)
		elseif QuestID == 20301 then
		    msg.AddMsgEvent(MsgID, 9, 200918)
		elseif QuestID == 20302 then
		    msg.AddMsgEvent(MsgID, 9, 266011)
		elseif QuestID == 20303 then
		    msg.AddMsgEvent(MsgID, 9, 298201)
		elseif QuestID == 20304 then
		    msg.AddMsgEvent(MsgID, 9, 319728)
		elseif QuestID == 20305 then
		    msg.AddMsgEvent(MsgID, 9, 328322)
		elseif QuestID == 20306 then
		    msg.AddMsgEvent(MsgID, 9, 350559)
		elseif QuestID == 20307 then
		    msg.AddMsgEvent(MsgID, 9, 474676)
		elseif QuestID == 20308 then
		    msg.AddMsgEvent(MsgID, 9, 586095)
		elseif QuestID == 20309 then
		    msg.AddMsgEvent(MsgID, 9, 764874)
		elseif QuestID == 20310 then
		    msg.AddMsgEvent(MsgID, 9, 1000000)
		elseif QuestID == 20327 then
		    msg.AddMsgEvent(MsgID, 9, 1200000)
		elseif QuestID == 20328 then
		    msg.AddMsgEvent(MsgID, 9, 1450000)
		end
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

end
aux.RegisterQuestEvent(20264, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20299, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20300, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20301, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20302, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20303, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20304, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20305, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20306, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20307, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20308, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20309, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20310, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20327, 1, "x20264_OnComplete")
aux.RegisterQuestEvent(20328, 1, "x20264_OnComplete")

--检测完成任务
function x20264_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

    if map.GetMapScriptData(MapID, InstanceID, 1, 4) < 6 then
	    return 1
	else
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2780)	--每个修罗界副本只有前6个玩家可以交付任务
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
        return 0
	end

end
aux.RegisterQuestEvent(20264, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20299, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20300, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20301, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20302, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20303, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20304, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20305, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20306, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20307, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20308, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20309, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20310, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20327, 5, "x20264_OnCheckComplete")
aux.RegisterQuestEvent(20328, 5, "x20264_OnCheckComplete")


function x3610211_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2814)	--您想要离开修罗界返回黄帝城吗？
		msg.AddMsgEvent(MsgID, 21, 7)		--确定按钮
		msg.AddMsgEvent(MsgID, 1, 2815)		--离开修罗界
		msg.AddMsgEvent(MsgID, 21, 8)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 22)		--取消
		msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 7 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 2094, 18339, 2701)
	end
end
--注册轩辕族哨兵对话事件
aux.RegisterCreatureEvent(3610211, 7, "x3610211_OnTalk")
