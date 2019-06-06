

--地图ScriptData字段意义说明。
--[[0:是否第一个人进来。
	1:记录现在剩余的怪物数。
	2:记录现在进来的人的总级数。
	3:记录现在进来的人数。
	]]

ZhongZhiChengCheng_Point = {}
ZhongZhiChengCheng_Point[1] = {x=150, y=46, z=108}--入口
ZhongZhiChengCheng_Point[2] = {x=137, y=46, z=121}--NPC
ZhongZhiChengCheng_Point[3] = {x=117, y=40, z=59}
ZhongZhiChengCheng_Point[4] = {x=103, y=40, z=74}
ZhongZhiChengCheng_Point[5] = {x=88, y=40, z=76}
ZhongZhiChengCheng_Point[6] = {x=92, y=40, z=92}
ZhongZhiChengCheng_Point[7] = {x=69, y=40, z=79}
ZhongZhiChengCheng_Point[8] = {x=77, y=40, z=95}
ZhongZhiChengCheng_Point[9] = {x=58, y=40, z=96}
ZhongZhiChengCheng_Point[10] = {x=60, y=40, z=149}
ZhongZhiChengCheng_Point[11] = {x=64, y=40, z=163}
ZhongZhiChengCheng_Point[12] = {x=79, y=40, z=152}
ZhongZhiChengCheng_Point[13] = {x=73, y=40, z=170}
ZhongZhiChengCheng_Point[14] = {x=92, y=40, z=164}
ZhongZhiChengCheng_Point[15] = {x=82, y=40, z=189}
ZhongZhiChengCheng_Point[16] = {x=98, y=40, z=181}
ZhongZhiChengCheng_Point[17] = {x=108, y=40, z=193}
ZhongZhiChengCheng_Point[18] = {x=148, y=40, z=180}
ZhongZhiChengCheng_Point[19] = {x=161, y=40, z=193}
ZhongZhiChengCheng_Point[20] = {x=161, y=40, z=179}
ZhongZhiChengCheng_Point[21] = {x=171, y=40, z=173}
ZhongZhiChengCheng_Point[22] = {x=187, y=40, z=174}
ZhongZhiChengCheng_Point[23] = {x=172, y=40, z=159}
ZhongZhiChengCheng_Point[24] = {x=192, y=40, z=162}
ZhongZhiChengCheng_Point[25] = {x=194, y=40, z=146}
ZhongZhiChengCheng_Point[26] = {x=183, y=40, z=99}
ZhongZhiChengCheng_Point[27] = {x=194, y=40, z=94}
ZhongZhiChengCheng_Point[28] = {x=181, y=40, z=89}
ZhongZhiChengCheng_Point[29] = {x=163, y=40, z=89}
ZhongZhiChengCheng_Point[30] = {x=181, y=40, z=70}
ZhongZhiChengCheng_Point[31] = {x=159, y=40, z=66}
ZhongZhiChengCheng_Point[32] = {x=158, y=40, z=57}


zhongzhi_TimerMin = 0
zhongzhiquest_complete = {}
zhongzhi_guild = {}

function zhongzhi_OnStart(actID)

	--重置TimerMin
	zhongzhi_TimerMin = 0
    zhongzhiquest_complete = {}

	--服务器涿鹿全场景通知[屏幕顶部滚动文字提示]：“***”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100095)       --兄弟齐心，其利断金！大家招呼好帮派中的弟兄，一起来涿鹿的【2094,2710】找众志成城发布人参与众志成城活动吧！
	msg.DispatchBroadcast(MsgID, -1, -1, -1)

	zhongzhi_creature = map.MapCreateCreature(3017299663, -1, 3610210, 2094, 18339, 2710) --刷新众志成城发布人

end


function zhongzhi_OnTimerMin(actID)

    zhongzhi_TimerMin = zhongzhi_TimerMin + 1
	if zhongzhi_TimerMin <= 10 and zhongzhi_TimerMin % 5 == 0 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100095)  --兄弟齐心，其利断金！大家招呼好帮派中的弟兄，一起来涿鹿的【2094,2710】找众志成城发布人参与众志成城活动吧！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
	end

end

-- npc生存时间到，活动结束，在各城市中删除此npc
function zhongzhi_OnEnd(actID)

	map.MapDeleteCreature(3017299663, -1, zhongzhi_creature)

end

--注册该活动的相关事件
aux.RegisterActEvent(119, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(120, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(121, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(122, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(123, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(124, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(125, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(126, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(139, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(140, 2, "zhongzhi_OnStart")
aux.RegisterActEvent(141, 2, "zhongzhi_OnStart")

aux.RegisterActEvent(119, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(120, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(121, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(122, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(123, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(124, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(125, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(126, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(139, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(140, 4, "zhongzhi_OnTimerMin")
aux.RegisterActEvent(141, 4, "zhongzhi_OnTimerMin")

aux.RegisterActEvent(119, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(120, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(121, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(122, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(123, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(124, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(125, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(126, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(139, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(140, 3, "zhongzhi_OnEnd")
aux.RegisterActEvent(141, 3, "zhongzhi_OnEnd")
--与众志成城发布人对话传送进副本

function x3610210_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if MapID == 3017299663 then
	    --获取队伍玩家等级
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
        if TalkIndex == -1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2802)		--“众志成城发布人：\n     <color=0xffff0000>20<color=0xfffff7e0>级以上的玩家与帮中的兄弟一起进入同心殿完成挑战可以获得大量的经验。您想现在就进入同心殿完成挑战吗？”
			msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
			msg.AddMsgEvent(MsgID, 1, 2803)			--“接取挑战任务”
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1, 2804)			--“进入同心殿”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1, 22)			--“取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif TalkIndex == 6 then
			local bool = false
			for i=0,15 do
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20265+i) == true then
					bool = true
					break
				end
			end
			if bool == true then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2806)	--您已经接取过任务了，不需要重复接取。
				msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 22)		--取消
				msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif rolelevel < 20 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local quest = 0
				if rolelevel >= 95 then
					quest = 20279
				elseif rolelevel >= 90 and rolelevel <= 94 then
					quest = 20278
				elseif rolelevel >= 85 and rolelevel <= 89 then
					quest = 20280
				elseif rolelevel >= 80 and rolelevel <= 84 then
					quest = 20277
				elseif rolelevel >= 75 and rolelevel <= 79 then
					quest = 20276
				elseif rolelevel >= 70 and rolelevel <= 74 then
					quest = 20275
				elseif rolelevel >= 65 and rolelevel <= 69 then
					quest = 20274
				elseif rolelevel >= 60 and rolelevel <= 64 then
					quest = 20273
				elseif rolelevel >= 55 and rolelevel <= 59 then
					quest = 20272
				elseif rolelevel >= 50 and rolelevel <= 54 then
					quest = 20271
				elseif rolelevel >= 45 and rolelevel <= 49 then
					quest = 20270
				elseif rolelevel >= 40 and rolelevel <= 44 then
					quest = 20269
				elseif rolelevel >= 35 and rolelevel <= 39 then
					quest = 20268
				elseif rolelevel >= 30 and rolelevel <= 34 then
					quest = 20267
				elseif rolelevel >= 25 and rolelevel <= 29 then
					quest = 20266
				elseif rolelevel >= 20 and rolelevel <= 24 then
					quest = 20265
				end
				if quest ~= 0 then
				    role.AddQuest(RoleID, quest)
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
				local playerguild = guild.GetRoleGuildID(RoleID)
				local same_guild = true
				if playerguild == 4294967295 or playerguild == nil then
					same_guild = false
				end
				for i=1, 6 do
					if Role[i] and Role[i]~= 4294967295 then
						Num = Num + 1
						local k = guild.GetRoleGuildID(Role[i])
						if k == 4294967295 or k == nil or k ~= playerguild then
							same_guild = false
						end
					end
				end


				if rolelevel < 20 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
					msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
					msg.AddMsgEvent(MsgID, 1, 	22)		--取消
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif Num < 4 or same_guild == false then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2801)	--您必须和3个同帮派的玩家组成队伍才能参加此活动
					msg.AddMsgEvent(MsgID, 21, 5)		--取消按钮
					msg.AddMsgEvent(MsgID, 1, 22)		--取消
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local bool = false
					for i=0,15 do
						if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20265+i) == true then
							bool = true
							break
						end
					end
					if bool ~= true then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 	2805)	--您需要接取了挑战任务之后才能进入同心殿
						msg.AddMsgEvent(MsgID, 21, 	-1)		--取消按钮
						msg.AddMsgEvent(MsgID, 1, 	22)		--取消
						msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
						local roleguild = guild.GetRoleGuildID(RoleID)
						if roleguild ~= 4294967295 and roleguild ~= nil then
							if zhongzhi_guild[roleguild] == nil then
								zhongzhi_guild[roleguild] = {RoleID}
							else
								local a = true
								for k,v in pairs(zhongzhi_guild[roleguild]) do
									if v == RoleID then
										a = false
										break
									end
								end
								if a == true then
									zhongzhi_guild[roleguild][#zhongzhi_guild[roleguild] + 1] = RoleID
								end
							end
						end
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092185652, ZhongZhiChengCheng_Point[1].x, ZhongZhiChengCheng_Point[1].y, ZhongZhiChengCheng_Point[1].z)
					end
				end
			end
		end
	else
	    local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	    if TalkIndex == -1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2807)	--众志成城发布人：\n     我有什么可以帮助您的？
			msg.AddMsgEvent(MsgID, 21, 9)		--确定按钮
			msg.AddMsgEvent(MsgID, 1, 2808)		--接取任务
			msg.AddMsgEvent(MsgID, 21, 7)		--确定按钮
			msg.AddMsgEvent(MsgID, 1, 2809)		--离开同心殿
			msg.AddMsgEvent(MsgID, 21, 8)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 22)		--取消
			msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif TalkIndex == 7 then

		    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 2094, 18339, 2701)
		elseif TalkIndex == 9 then
			local bool = false
			for i=0,15 do
				if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20265+i) == true then
					bool = true
					break
				end
			end
			if bool == true then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2806)	--您已经接取过任务了，不需要重复接取。
				msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 22)		--取消
				msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local quest = 0
				if rolelevel >= 95 then
					quest = 20279
				elseif rolelevel >= 90 and rolelevel <= 94 then
					quest = 20278
				elseif rolelevel >= 85 and rolelevel <= 89 then
					quest = 20280
				elseif rolelevel >= 80 and rolelevel <= 84 then
					quest = 20277
				elseif rolelevel >= 75 and rolelevel <= 79 then
					quest = 20276
				elseif rolelevel >= 70 and rolelevel <= 74 then
					quest = 20275
				elseif rolelevel >= 65 and rolelevel <= 69 then
					quest = 20274
				elseif rolelevel >= 60 and rolelevel <= 64 then
					quest = 20273
				elseif rolelevel >= 55 and rolelevel <= 59 then
					quest = 20272
				elseif rolelevel >= 50 and rolelevel <= 54 then
					quest = 20271
				elseif rolelevel >= 45 and rolelevel <= 49 then
					quest = 20270
				elseif rolelevel >= 40 and rolelevel <= 44 then
					quest = 20269
				elseif rolelevel >= 35 and rolelevel <= 39 then
					quest = 20268
				elseif rolelevel >= 30 and rolelevel <= 34 then
					quest = 20267
				elseif rolelevel >= 25 and rolelevel <= 29 then
					quest = 20266
				elseif rolelevel >= 20 and rolelevel <= 24 then
					quest = 20265
				end
				role.AddQuest(RoleID, quest)
			end
		end
	end
end
--注册轩辕族哨兵对话事件
aux.RegisterCreatureEvent(3610210, 7, "x3610210_OnTalk")

function a08_OnPlayerEnter(MapID, InstanceID, RoleID)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local temp1 = map.GetMapScriptData(MapID, InstanceID, 1, 2)
    map.SetMapScriptData(MapID, InstanceID, 1, 2, temp1 + rolelevel)
	local temp2 = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	map.SetMapScriptData(MapID, InstanceID, 1, 3, temp2 + 1)
	if map.GetMapScriptData(MapID, InstanceID, 1, 0) ~= 1 then

		map.SetMapScriptData(MapID, InstanceID, 1, 0, 1)--已经有人进入了副本
		map.MapCreateCreature(MapID, InstanceID, 1535216, 1, 30, 1) --刷新同心殿灵
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100096)    --<color=0xffff0000>30<color=0xfffff7e0>秒后怪物将会出现
		msg.DispatchBroadcast(MsgID, MapID, InstanceID, -1)

    end

end

--注册
aux.RegisterMapEvent("a08", 2, "a08_OnPlayerEnter")

--玩家离开竞技场

function a08_LeaveMap(MapID, InstanceID, RoleID)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local temp1 = map.GetMapScriptData(MapID, InstanceID, 1, 2)
    map.SetMapScriptData(MapID, InstanceID, 1, 2, temp1 - rolelevel)
	local temp2 = map.GetMapScriptData(MapID, InstanceID, 1, 3)
	map.SetMapScriptData(MapID, InstanceID, 1, 3, temp2 - 1)

end

aux.RegisterMapEvent("a08", 3, "a08_LeaveMap")

--用30秒怪物控制时间，当计时器使用
function c1535216_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 2810)    --新一波的怪物已经出现
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
    --[[if TeamID ~= 4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)]]
		local Num = map.GetMapScriptData(MapID, InstanceID, 1, 3)
		local totallevel = map.GetMapScriptData(MapID, InstanceID, 1, 2)
		if Num <= 0 or totallevel <= 0 then
		    Num = 1
		    totallevel = 20
		end
		--[[for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then

				local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				if rolelevel >=20 then
					Num = Num + 1
					totallevel = totallevel + rolelevel
				end
			end
		end]]
		local average_level = math.floor(totallevel/Num/10)
		if average_level < 2 then
			average_level = 2
		elseif average_level > 9 then
			average_level = 9
		end
		local monsterid1 = 1535198 + average_level*2 - 2
		local monsterid2 = 1535199 + average_level*2 - 2
		if totallevel/Num > 105 then
		    monsterid1 = 1535227
			monsterid2 = 1535228
		elseif totallevel/Num > 100 then
		    monsterid1 = 1535225
			monsterid2 = 1535226
		end
		for i=3,32 do
			if i % 2== 0 then
				map.MapCreateCreature(MapID, InstanceID, monsterid1, ZhongZhiChengCheng_Point[i].x, ZhongZhiChengCheng_Point[i].y, ZhongZhiChengCheng_Point[i].z)
			else
				map.MapCreateCreature(MapID, InstanceID, monsterid2, ZhongZhiChengCheng_Point[i].x, ZhongZhiChengCheng_Point[i].y, ZhongZhiChengCheng_Point[i].z)
			end
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 1, 30)
	--end
end

aux.RegisterCreatureEvent(1535216, 13, "c1535216_OnDisappear")

--清完小怪刷出同心殿灵
function zhongzhi_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local a = math.random(100)
	if a == 1 then
	    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		if TeamID ~= 4294967295 then
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			local Num = 0
			local totallevel = 0
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then

					local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
					if rolelevel >=20 then
						Num = Num + 1
						totallevel = totallevel + rolelevel
					end
				end
			end
			local average_level = math.floor(totallevel/Num/10)
			if average_level < 2 then
				average_level = 2
			elseif average_level > 9 then
				average_level = 9
			end
			local d= math.random(3,32)
			map.MapCreateCreature(MapID, InstanceID, 1535215 + average_level, ZhongZhiChengCheng_Point[d].x, ZhongZhiChengCheng_Point[d].y, ZhongZhiChengCheng_Point[d].z)
        end
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= 4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then

				--local k = math.random(2)
				--if k == 1 then
					local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
					if Target_MapID == MapID then
						if role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20265) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616329, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20266) == true then
						    role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616330, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20267) == true then
						    role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616331, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20268) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616332, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20269) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616333, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20270) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616334, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20271) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616335, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20272) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616336, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20273) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616337, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20274) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616338, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20275) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616339, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20276) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616340, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20277) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616341, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20280) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616342, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20278) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616343, 1, 0, 102)
						elseif role.IsRoleHaveQuest(MapID, InstanceID, Role[i], 20279) == true then
							role.QuestAddRoleItem(MapID, InstanceID, Role[i], 20265, 2616344, 1, 0, 102)
						end
					end
				--end
			end
		end
	end

	local Num = map.GetMapScriptData(MapID, InstanceID, 1, 1)
	Num = Num - 1
	map.SetMapScriptData(MapID, InstanceID, 1, 1, Num)
	if Num == 0 then
	    map.MapCreateCreature(MapID, InstanceID, 1535216, 1, 30, 1) --刷新同心殿灵
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100096)    --<color=0xffff0000>30<color=0xfffff7e0>秒后怪物将会出现
		msg.DispatchBroadcast(MsgID, MapID, InstanceID, -1)
	end
end

aux.RegisterCreatureEvent(1535200, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535201, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535202, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535203, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535204, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535205, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535206, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535207, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535208, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535209, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535210, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535211, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535212, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535213, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535214, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535215, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535225, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535226, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535227, 4, "zhongzhi_OnDie")
aux.RegisterCreatureEvent(1535228, 4, "zhongzhi_OnDie")

--完成任务
function zhongzhi_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

    if math.random(1,10) == 7 then
		if QuestID <= 20272 then
			local k = math.random(1,5)
			role.AddRoleItem(MapID, InstanceID, RoleID, 3110000 + k, 1, -1, 8, 420)
		else
			local k = math.random(1,5)
			role.AddRoleItem(MapID, InstanceID, RoleID, 3110005 + k, 1, -1, 8, 420)
		end
	end

	local a = QuestID - 20265           --计算玩家一共获得的同心印数量
	if QuestID == 20280 then
	    a = 13
	elseif QuestID == 20278 then
	    a = 14
	elseif QuestID == 20279 then
	    a = 16
	end
	local b = 40 + a*5
	local playerguild = guild.GetRoleGuildID(RoleID)
	if playerguild ~= 4294967295 and playerguild ~= nil then
	    if zhongzhiquest_complete[playerguild] == nil then
	        zhongzhiquest_complete[playerguild] = 0
		end

		local k = zhongzhiquest_complete[playerguild]
		zhongzhiquest_complete[playerguild] = zhongzhiquest_complete[playerguild] + b
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 2811)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 9, b)
		msg.AddMsgEvent(MsgID, 9, zhongzhiquest_complete[playerguild])
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		if zhongzhiquest_complete[playerguild] >= 1000 and k < 1000 then
			ZhongZhi_ExpOnComplete(playerguild, 0.5)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100097)       --【江湖传闻】XXX所在帮派上交的同心印达到1000，所有处于涿鹿场景的帮派成员都获得了额外经验奖励！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.DispatchBroadcast(MsgID, -1, -1, -1)
		elseif zhongzhiquest_complete[playerguild] >= 2000 and k < 2000 then
			ZhongZhi_ExpOnComplete(playerguild, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100098)       --【江湖传闻】XXX所在帮派上交的同心印达到2000，所有处于涿鹿场景的帮派成员都获得了额外经验奖励！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.DispatchBroadcast(MsgID, -1, -1, -1)
		elseif zhongzhiquest_complete[playerguild] >= 4000 and k < 4000 then
			ZhongZhi_ExpOnComplete(playerguild, 1.5)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100099)       --【江湖传闻】XXX所在帮派上交的同心印达到4000，所有处于涿鹿场景的帮派成员都获得了额外经验奖励！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.DispatchBroadcast(MsgID, -1, -1, -1)
		elseif zhongzhiquest_complete[playerguild] >= 7000 and k < 7000 then
			ZhongZhi_ExpOnComplete(playerguild, 2)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100100)       --【江湖传闻】XXX所在帮派上交的同心印达到7000，所有处于涿鹿场景的帮派成员都获得了额外经验奖励！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.DispatchBroadcast(MsgID, -1, -1, -1)
		elseif zhongzhiquest_complete[playerguild] >= 10000 and k < 10000 then
			guild.ModifyGuildFund(playerguild, RoleID, 150000,104)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100101)       --【江湖传闻】同心印达到10000！XXX所在的帮派获得帮派资金150000！
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.DispatchBroadcast(MsgID, -1, -1, -1)
		end
	end
end
aux.RegisterQuestEvent(20265, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20266, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20267, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20268, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20269, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20270, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20271, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20272, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20273, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20274, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20275, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20276, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20277, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20278, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20279, 1, "zhongzhi_OnComplete")
aux.RegisterQuestEvent(20280, 1, "zhongzhi_OnComplete")

function ZhongZhi_ExpOnComplete(roleguild, kk)
    local RoleTbl = map.GetMapAllRoleID(3017299663, -1)
	for k,v in pairs(RoleTbl) do
		if guild.GetRoleGuildID(v) == roleguild then
			local rolelevel = role.GetRoleLevel(3017299663, -1, v)
			local exp = math.floor(activity_newexp[rolelevel]/5*1.3*kk)
			role.AddRoleExp(3017299663, -1, v, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2816)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(v, MsgID)
		end
	end
	if zhongzhi_guild[roleguild] ~= nil then
	    for k,v in pairs(zhongzhi_guild[roleguild]) do
		    local Target_MapID, Target_InstanceID = role.GetRoleMapID(v)
			if Target_MapID and Target_InstanceID then
			    local rolelevel = role.GetRoleLevel(Target_MapID, Target_InstanceID, v)
				local exp = math.floor(activity_newexp[rolelevel]/5*1.3*kk)
				role.AddRoleExp(Target_MapID, Target_InstanceID, v, exp)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 2816)
				msg.AddMsgEvent(MsgID, 9, exp)
				msg.DispatchRoleMsgEvent(v, MsgID)
			end
		end
	end
end
