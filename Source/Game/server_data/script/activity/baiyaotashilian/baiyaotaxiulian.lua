--地图ScriptData字段意义说明。
--[[0:记录现在打到第几波第几轮。
    1：记录现在进行的是什么项目。0是普通流程，1个狼，2是小蛋，3个大蛋，4是答题，5是领奖。
	2:触发过几次跳关
	3：触发过几次特殊事件
	4-9:记录进入过束妖之间的玩家ID。
	10-15：记录开启过最终宝箱的玩家ID。
	16:记录现在杀了多少怪
	17:记录总共多少怪物
	18:第一头狼的ID
	19:第二头狼的ID
	20:记录现在已经进行了多少个30秒
	21:记录现在的活动还有几个人在参加
	22:记录轩辕的ID
	23:记录现在刷了多少次蛋
	24:记录现在进行了多少个15秒
	25：击杀蛋得的分数
	26：已经刷出的总分数
	]]

baiyaota_quest = {}
baiyaota_egg = {}

function baiyaota_OnTimerMin(actID)
    local curmin = tonumber(os.date("%M"))
	local curhour = tonumber(os.date("%H"))
	if curhour%2 == 0 and curmin % 31 == 0 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100139)  --百妖塔试炼（80级以上可参与）已经开放，各路英雄们请前往涿鹿的三塔驿迎接未知的挑战吧！
		msg.DispatchBroadcast(MsgID, -1, -1, -1)
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(156, 4, "baiyaota_OnTimerMin")


--与上官竹汐传送进副本

function x4902009_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	12301)		--id_msg.xml “上官竹汐：\n     百妖塔是以前轩辕黄帝关押妖魔凶灵的地方，如今涿鹿之主帝瑞将它开放，作为勇者的历练之所。百妖塔的魔物众多，需要<color=0xffff0000>4<color=0xfffff7e0>个<color=0xffff0000>80<color=0xfffff7e0>级以上的玩家组队才能进入。\n    每个玩家每天只能进入百妖塔3次，你想现在进去吗？”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	12302)			--“进入百妖塔”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		--判断玩家是否可以进入副本
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		if TeamID == nil or TeamID == 4294967295 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	2812)	--您没有组队，不能进入该副本
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			local Num = 0
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
				    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
				    local level = role.GetRoleLevel(Target_MapID, Target_InstanceID, Role[i])
					if level > 79 then
						Num = Num + 1
					end
				end
			end
			
			if rolelevel < 80 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif Num < 4 then
			local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	12303)	--您需要凑够4个80级以上的队友才能参加此活动
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local CurTime = tonumber(os.date("%j"))
				local UpdateTime = math.floor(role.GetRoleScriptData(RoleID, 1, RoleDataType["baiyaota_Update"])/10)
				if CurTime~=UpdateTime then
					role.SetRoleScriptData(RoleID, 1, RoleDataType["baiyaota_Update"], CurTime*10)
				end
				if  CurTime == UpdateTime and  role.GetRoleScriptData(RoleID, 1, RoleDataType["baiyaota_Update"]) % 10 > 2 then   --修改1
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 	12304)	--您今天进入百妖塔的次数已经超过了<p1>次，请明天再来吧。
					msg.AddMsgEvent(MsgID, 9, 	3)
					msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
					msg.AddMsgEvent(MsgID, 1, 	22)		--取消
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
				    role.SetRoleScriptData(RoleID, 1, RoleDataType["baiyaota_Update"],role.GetRoleScriptData(RoleID, 1, RoleDataType["baiyaota_Update"])+1)
				    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 118, 1, 324)
				end
			end
		end
	end
end
--注册传送NPC对话事件
aux.RegisterCreatureEvent(4902009, 7, "x4902009_OnTalk")

function x4902008_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local LeaderID = 0
	if TeamID ~= nil and TeamID ~= 4294967295 then
	    LeaderID = team.GetTeamLeaderID(TeamID)
	end
	--判断玩家是否可以进入副本
	if TalkIndex == -1 then
	    if map.GetMapScriptData(MapID, InstanceID, 1, 0) == 0 then
		    map.SetMapScriptData(MapID, InstanceID, 1, 0, 10)
		end
	    local Wave = math.floor(map.GetMapScriptData(MapID, InstanceID, 1, 0)/10)
		local part = map.GetMapScriptData(MapID, InstanceID, 1, 0)%10
		local Wave1 = Wave
		if Wave1 > 8 then
		    Wave1 = 8
		end
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 	12305)	--    只有队长才能开启新的试炼。\n    总共5轮试炼，当前进行到第X轮。
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 9, Wave1)
		
		if LeaderID == RoleID then
			if Wave < 9 and part == 0 then           --楼层限制，地图限制 if Wave < 9 and part == 0 then 
				msg.AddMsgEvent(MsgID, 21, 	4)		--确定按钮
				msg.AddMsgEvent(MsgID, 1, 12306)		--开启本轮试炼
			end
			
			msg.AddMsgEvent(MsgID, 21, 	4)		--确定按钮
			msg.AddMsgEvent(MsgID, 1, 12306)		--开启本轮试炼
				                                        --修改2
		end
		
		if Wave >= 2 then
			msg.AddMsgEvent(MsgID, 21, 	5)		--确定按钮
			msg.AddMsgEvent(MsgID, 1, 12307)		--进入束妖之间
		end
		msg.AddMsgEvent(MsgID, 21, 	6)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 12308)		--离开百妖塔
		msg.AddMsgEvent(MsgID, 21, 	7)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 	22)		--取消
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then     --计算应该刷出什么难度的怪物
	    if map.GetMapScriptData(MapID, InstanceID, 1, 0) % 10 == 0 and map.GetMapScriptData(MapID, InstanceID, 1, 0) < 90 then
			local award = true
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if u ~= nil then
					if BYT_IsInArea(MapID, InstanceID, v, 207) then
						award = false
					end
				end
			end
			if award == false then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12309)	--    请等待其他玩家从束妖之间出来之后再选择开启新一轮试炼。
				msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 21)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif map.GetMapScriptData(MapID, InstanceID, 1, 1) ~= 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12310)	--    特殊事件结束之后才能选择此选项。
				msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21, -1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 21)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				if math.random(100) > 80 and map.GetMapScriptData(MapID, InstanceID, 1, 3) == 0 then

-----------------------------------进入特殊事件-----------------------------------------
                    map.SetMapScriptData(MapID, InstanceID, 1, 3, 1)
					local k = math.random(4)
					if k == 1 then
              --------------------------狂狼--------------------------
					    local TransPoint = {}
						TransPoint[1] = {x=205,y=40,z=185}
						TransPoint[2] = {x=141,y=40,z=178}
						TransPoint[3] = {x=166,y=40,z=84}
						TransPoint[4] = {x=176,y=40,z=215}
						TransPoint[5] = {x=269,y=40,z=125}
						TransPoint[6] = {x=74,y=40,z=110}
						--TransPoint[7] = {x=343,y=40,z=106}
						--TransPoint[8] = {x=326,y=40,z=60}
						--TransPoint[9] = {x=379,y=40,z=80}

						local n = 0
                        for u,v in pairs(RoleTbl) do
							if u ~= nil then
							    n = n + 1
							    local point = math.random(6)
								role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, TransPoint[point].x, 1, TransPoint[point].z)
							    unit.AddBuff(MapID, InstanceID, v, 1130101, v)
							end
						end

						if math.random(5) == 3 then
							local player = math.random(n)
							local m = 0
							for u,v in pairs(RoleTbl) do
								m = m + 1
								if m == player then
								    unit.CancelBuff(MapID, InstanceID, v, 1130101)
									unit.AddBuff(MapID, InstanceID, v, 1130301, v)
								    break
								end
							end
						end

						map.SetMapScriptData(MapID, InstanceID, 1, 1, 1)
						map.SetMapScriptData(MapID, InstanceID, 1, 21, n)

						local monid = map.MapCreateCreature(MapID, InstanceID, 1535444, 229, 40, 155)
						unit.AddBuff(MapID, InstanceID, monid, 1130701, monid)
						map.SetMapScriptData(MapID, InstanceID, 1, 18, monid)
						monid = map.MapCreateCreature(MapID, InstanceID, 1535444, 112, 40, 144)
						unit.AddBuff(MapID, InstanceID, monid, 1130701, monid)
					    map.SetMapScriptData(MapID, InstanceID, 1, 19, monid)

						map.MapCreateCreature(MapID, InstanceID, 4902011, 1, 40, 1)

					    local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12311)	--    <color=0xffff0000>触发特殊事件：狼嘴逃生<color=0xfffff7e0>
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
					elseif k == 2 then
                        --------------------------答题--------------------------
						local n = 0
                        for u,v in pairs(RoleTbl) do
							if u ~= nil then
							    n = n + 1
								role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 344, 1, 119)
							    unit.AddBuff(MapID, InstanceID, v, 1130201, v)
							end
						end
						map.SetMapScriptData(MapID, InstanceID, 1, 1, 4)
						map.SetMapScriptData(MapID, InstanceID, 1, 21, n)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12312)	--15秒后开始提问。站在轩辕右手边的光圈代表“是”，站在左手边光圈代表“否”
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

						map.MapCreateCreature(MapID, InstanceID, 4902012, 1, 40, 1)
					elseif k == 3 then
                        --------------------------小蛋--------------------------
						for u,v in pairs(RoleTbl) do
							if u ~= nil then
								role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 299, 1, 319)
							end
						end
						map.MapCreateCreature(MapID, InstanceID, 4902013, 1, 40, 1)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12313)	--获得一半以上的分数将可以获得额外奖励
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

						map.SetMapScriptData(MapID, InstanceID, 1, 1, 2)
					elseif k == 4 then
                        --------------------------大蛋--------------------------
						local monster = map.MapCreateCreature(MapID, InstanceID, 1535435, 125, 40, 341)
						unit.AddBuff(MapID, InstanceID, monster, 1131101, monster)
						map.SetMapScriptData(MapID, InstanceID, 1, 1, 3)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12314)	--特殊事件：百妖塔中出现了一个奇怪的蛋
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

					end

---------------------------------------------------------------------------------------

				else
					if map.GetMapScriptData(MapID, InstanceID, 1, 2) < 2 and math.random(100) > 80 then
						map.SetMapScriptData(MapID, InstanceID, 1, 2, map.GetMapScriptData(MapID, InstanceID, 1, 2)+1)
						--进入特殊事件。
						local k = math.random(3)
						if k == 1 then
							map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)+2)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 101, 12315)	--    <color=0xffff0000>遭遇妖魔主力的袭击！<color=0xfffff7e0>
							msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
						elseif k == 2 then
							map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)+3)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 101, 12316)	--    <color=0xffff0000>遭遇妖魔精英的突然袭击！<color=0xfffff7e0>
							msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
						elseif k == 3 then
							map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)+4)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 101, 12317)	--    <color=0xffff0000>遭遇妖魔首领的突然袭击！<color=0xfffff7e0>
							msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
						end
					else
						map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)+1)
					end
					local Wave = math.floor(map.GetMapScriptData(MapID, InstanceID, 1, 0)/10)
					local part = map.GetMapScriptData(MapID, InstanceID, 1, 0)%10
					if part == 1 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12318)	--    第X轮的试炼开始
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
					end
					BaiYaoTa_CreatCreature(MapID, InstanceID, Wave, part)
				end
			end
		end
    elseif TalkIndex == 5 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 	12319)	--    每次挑战百妖塔试炼只有一次进入束妖之阁的机会。在束妖之阁中你可以选择一个妖族留下的宝箱来作为试炼的奖励。如果你在完成整个试炼之后再进入束妖之阁，那么你将会有几率获得更好的奖励。确定进入束妖阁吗？
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21, 	8)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 	21)		--取消
		msg.AddMsgEvent(MsgID, 21, 	7)		--取消按钮
		msg.AddMsgEvent(MsgID, 1, 	22)		--取消
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 6 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1568, 259, 1133)
	elseif TalkIndex == 8 then
	    local bool = true
		for i=4,9 do
		    if map.GetMapScriptData(MapID, InstanceID, 1, i) == 0 then
			    break
		    elseif map.GetMapScriptData(MapID, InstanceID, 1, i) == RoleID then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	12320)	--    您已经进入过束妖之阁了，本次试炼您无法再次领取奖励。
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21, 	-1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	21)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				bool = false
				break
			elseif map.GetMapScriptData(MapID, InstanceID, 1, i) ~= RoleID and i == 6 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	12321)	--    每次试炼只能有6个玩家进入束妖之阁领取奖励
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21, 	-1)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	21)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				bool = false
			end
	    end
		if bool then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 344, 1, 237)
	    end
	end
end
--注册
aux.RegisterCreatureEvent(4902008, 7, "x4902008_OnTalk")

function x4902018_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	12322)		--“    你将要开启的是妖族遗宝，每个人每次参与试炼只能开启一个妖族遗宝。\n    开启妖族遗宝将有可能碰到意想不到的情况，你确定要开启吗？”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	12323)			--“开启宝箱”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
	    local key = 0     --1是可以开，2是已经开过，3是已经6个人开过
		for i= 10,15 do
		    if RoleID == map.GetMapScriptData(MapID, InstanceID, 1, i) then
			    key = 2
				break
			elseif i == 15 and map.GetMapScriptData(MapID, InstanceID, 1, i) ~= 0 then
	            key = 3
			elseif map.GetMapScriptData(MapID, InstanceID, 1, i) == 0 then
			    map.SetMapScriptData(MapID, InstanceID, 1, i, RoleID)
				key = 1
				break
			end
		end
		if key == 2 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12324)		--“    你已经开过妖族遗宝了，本次试炼无法再次开启。”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	21)			--“取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif key == 3 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12325)		--“    每次试炼只能开启6个试炼宝箱。本次试炼已经有6个玩家开启过妖族遗宝，您无法再次开启。”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	21)			--“取消”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local index = 1
			local k = math.random(100)
		    if map.GetMapScriptData(MapID, InstanceID, 1, 0) >= 60 then
				if k > 95 then
				    index = 5
				elseif k >93 then
				    index = 4
				elseif k >10 then
				    index = 1
				elseif k >5 then
				    index = 2
				else
				    index = 3
				end
			elseif map.GetMapScriptData(MapID, InstanceID, 1, 0) >= 50 then
			    if k > 98 then
				    index = 5
				elseif k >96 then
				    index = 4
				elseif k >15 then
				    index = 1
				elseif k >8 then
				    index = 2
				else
				    index = 3
				end
			elseif map.GetMapScriptData(MapID, InstanceID, 1, 0) >= 40 then
			    if k > 98 then
				    index = 4
				elseif k >20 then
				    index = 1
				elseif k >10 then
				    index = 2
				else
				    index = 3
				end
			else
			    if k >30 then
				    index = 1
				elseif k >15 then
				    index = 2
				else
				    index = 3
				end
			end
			local base = math.floor(map.GetMapScriptData(MapID, InstanceID, 1, 0)/10)
			if base <= 0 then
			   base = 1
			elseif base > 6 then
			   base = 6
			end
			local exp = math.random(8,12)*base*10000
			local money = math.random(5,20)*base*5000
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			role.AddRoleSilver(MapID, InstanceID, RoleID, money, 102)

			if index == 2 then
			    local ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
				unit.ChangeHP(MapID, InstanceID, RoleID, -ReviveHP)
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 118, 1, 324)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	12326)		--触发陷阱，受到致命伤害
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif index == 3 then
			    local k = math.random(3)
				if k == 1 then
				    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1599, 360, 1096)
				elseif k == 2 then
				    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1703, 351, 1071)
				elseif k ==3 then
				    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1622, 351, 993)
				end
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	12327)		--触发陷阱，退出试炼
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif index == 5 then
			    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 326, 1, 60)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	12328)		--传送至魔王巢穴，挑战最终BOSS
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif index == 4 then
			    if role.GetBagFreeSize(RoleID) > 0 then
				    local item1 = {id={1000009,1000010,1000011,1000109,1000110,1000111},num={1,1,1,1,1,1}}
					local item2 = {id={6010803,6010803,6010803,6010804},num={1,1,1,1}}
					local item3 = {id={6010701,6010702,6010703,6010704,6010008,6010011,6010108,6010111},num={1,1,1,1,1,1,1,1}}
					local item = item1
					local k = math.random(100)
					if k == 100 then
					    item = item3
					elseif k >95 then
                        item = item2
					end
					local k = math.random(#item.id)
					role.AddRoleItem(MapID, InstanceID, RoleID, item.id[k], item.num[k], -1, 8, 420)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101,	12329)		--从妖族遗宝中发现额外奖励
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
			map.MapDeleteCreature(MapID, InstanceID, TargetID)
		end
	end
end
--注册传送NPC对话事件
aux.RegisterCreatureEvent(4902018, 7, "x4902018_OnTalk")

function x4902019_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	12330)		--“    你想要返回百妖塔吗？”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
	    if BYT_IsInArea(MapID, InstanceID, RoleID, 205) or BYT_IsInArea(MapID, InstanceID, RoleID, 207) then
		    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 118, 1, 324)
		end
    end
end
--注册传送NPC对话事件
aux.RegisterCreatureEvent(4902019, 7, "x4902019_OnTalk")

--杀掉怪物
function baiyaota_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Num = map.GetMapScriptData(MapID, InstanceID, 1, 16)
	local Num2 = map.GetMapScriptData(MapID, InstanceID, 1, 17)
	map.SetMapScriptData(MapID, InstanceID, 1, 16, Num - 1)
	Num = Num - 1

	if math.random(300) == 177 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12331)    --击杀3分钟存活时间的特殊怪物将有机会获得稀有道具奖励！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	    local monid = map.MapCreateCreature(MapID, InstanceID, 1535445, 125, 40, 341)
		unit.AddBuff(MapID, InstanceID, monid, 1131001, monid)
		local k = math.random(3)
		local q = math.random(3)
		if k == 2 then
		    unit.AddBuff(MapID, InstanceID, monid, 1132001, monid)
			unit.AddBuff(MapID, InstanceID, monid, 1136001, monid)
		elseif k == 3 then
		    unit.AddBuff(MapID, InstanceID, monid, 1133001, monid)
			unit.AddBuff(MapID, InstanceID, monid, 1136001, monid)
		end
		if q == 2 then
		    unit.AddBuff(MapID, InstanceID, monid, 1134001, monid)
		elseif q == 3 then
		    unit.AddBuff(MapID, InstanceID, monid, 1135001, monid)
		end
	end

	if Num > 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 12332)    --剩余怪物数量：XX/XX
		msg.AddMsgEvent(MsgID, 9, Num2-Num)
		msg.AddMsgEvent(MsgID, 9, Num2)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	else
	    local k = map.GetMapScriptData(MapID, InstanceID, 1, 0)
		Wave = math.floor(k/10)
		part = k % 10
		if part >= 4 then
		    map.SetMapScriptData(MapID, InstanceID, 1, 0, (Wave+1)*10)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12333)    --第XX轮试炼完成
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		else
		    map.SetMapScriptData(MapID, InstanceID, 1, 0, Wave*10+part+1)
			BaiYaoTa_CreatCreature(MapID, InstanceID, Wave, part+1)
		end
	end

end

aux.RegisterCreatureEvent(1535416, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535417, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535418, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535419, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535420, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535421, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535422, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535423, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535424, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535425, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535426, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535427, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535428, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535429, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535430, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535431, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535432, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535433, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535446, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535447, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535448, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535449, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535450, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535451, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535452, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535453, 4, "baiyaota_OnDie")
aux.RegisterCreatureEvent(1535454, 4, "baiyaota_OnDie")


--杀掉怪物
function C1535436_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if map.GetMapScriptData(MapID, InstanceID, 1, 1) == 2 then
		map.SetMapScriptData(MapID, InstanceID, 1, 25, map.GetMapScriptData(MapID, InstanceID, 1, 25)+1)
		local k = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
		if baiyaota_egg[InstanceID] ~= nil then
			table.insert(baiyaota_egg[InstanceID],k)
		end
		local point = map.GetMapScriptData(MapID, InstanceID, 1, 25)
		local point_total = map.GetMapScriptData(MapID, InstanceID, 1, 26)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 12334)    --当前获得分数 xx/xx
		msg.AddMsgEvent(MsgID, 9, point)
		msg.AddMsgEvent(MsgID, 9, point_total)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 12334)	--当前获得分数 xx/xx
		msg.AddMsgEvent(MsgID, 9, point)
		msg.AddMsgEvent(MsgID, 9, point_total)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1535436, 4, "C1535436_OnDie")

--杀掉怪物
function C1535437_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if map.GetMapScriptData(MapID, InstanceID, 1, 1) == 2 then
		map.SetMapScriptData(MapID, InstanceID, 1, 25, map.GetMapScriptData(MapID, InstanceID, 1, 25)+5)
		local k = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
		if baiyaota_egg[InstanceID] ~= nil then
			table.insert(baiyaota_egg[InstanceID],k)
		end
		local point = map.GetMapScriptData(MapID, InstanceID, 1, 25)
		local point_total = map.GetMapScriptData(MapID, InstanceID, 1, 26)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 12334)    --当前获得分数 xx/xx
		msg.AddMsgEvent(MsgID, 9, point)
		msg.AddMsgEvent(MsgID, 9, point_total)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 12334)	--当前获得分数 xx/xx
		msg.AddMsgEvent(MsgID, 9, point)
		msg.AddMsgEvent(MsgID, 9, point_total)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
    end
end

aux.RegisterCreatureEvent(1535437, 4, "C1535437_OnDie")

--杀掉怪物
function C1535438_6_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12335)	--XXX死亡，特殊挑战完成！
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
end

aux.RegisterCreatureEvent(1535438, 4, "C1535438_6_OnDie")
aux.RegisterCreatureEvent(1535439, 4, "C1535438_6_OnDie")
aux.RegisterCreatureEvent(1535440, 4, "C1535438_6_OnDie")
aux.RegisterCreatureEvent(1535441, 4, "C1535438_6_OnDie")
aux.RegisterCreatureEvent(1535442, 4, "C1535438_6_OnDie")
aux.RegisterCreatureEvent(1535443, 4, "C1535438_6_OnDie")

--杀掉怪物
function C1535435_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local mon = 1535437 + math.random(6)
	local monster = map.MapCreateCreature(MapID, InstanceID, mon, 125, 40, 341)
	unit.AddBuff(MapID, InstanceID, monster, 1131101, monster)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12336)	--特殊怪物BOSS出现！请在10分钟之内将其击杀
	msg.AddMsgEvent(MsgID, 5, mon)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

aux.RegisterCreatureEvent(1535435, 4, "C1535435_OnDie")


function c4902011_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
    map.SetMapScriptData(MapID, InstanceID, 1, 20,map.GetMapScriptData(MapID, InstanceID, 1, 20)+30)
	local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
	if map.GetMapScriptData(MapID, InstanceID, 1, 20) <= 210 then
	    if map.GetMapScriptData(MapID, InstanceID, 1, 1) == 1 then
			map.MapCreateCreature(MapID, InstanceID, 4902011, 1, 40, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12337)    --                特殊事件将在XX秒后结束
			msg.AddMsgEvent(MsgID, 9, 240-map.GetMapScriptData(MapID, InstanceID, 1, 20))
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12338)    --                <color=0xffff0000>梦魇狂狼对新的猎物产生了兴趣
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if BYT_IsInArea(MapID, InstanceID, v, 202) then
					local k = math.random(5)
					cre.AddEnmity(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 18), v, k*(map.GetMapScriptData(MapID, InstanceID, 1, 20)/30)^2)
					k = math.random(5)
					cre.AddEnmity(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 19), v, k*(map.GetMapScriptData(MapID, InstanceID, 1, 20)/30)^2)
				end
			end
		end
	else
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12339)    --                <color=0xffff0000>梦魇狂狼挑战完成！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	    map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
	    for u,v in pairs(RoleTbl) do
			if BYT_IsInArea(MapID, InstanceID, v, 202) then
			    unit.CancelBuff(MapID, InstanceID, v, 1130101)
				unit.CancelBuff(MapID, InstanceID, v, 1130301)
				if unit.IsDead(MapID, InstanceID, v) then
				    role.ForcedToRevive(MapID, InstanceID, v)
				end
				role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 118, 1, 324)
			end
		end
		map.MapCreateCreature(MapID, InstanceID, 4902017, 125, 40, 341)
	end
end

aux.RegisterCreatureEvent(4902011, 13, "c4902011_OnDisappear")

function c4902012_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
    local k = map.GetMapScriptData(MapID, InstanceID, 1, 24)
	k = k + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 24, k)
	if k >= 17 then
	    baiyaota_quest[InstanceID] = nil
		map.MapCreateCreature(MapID, InstanceID, 4902017, 125, 40, 341)
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for u,v in pairs(RoleTbl) do
			if BYT_IsInArea(MapID, InstanceID, v, 206) then
			    unit.CancelBuff(MapID, InstanceID, v, 1130201)
			    role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 118, 1, 324)
			end
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
	elseif map.GetMapScriptData(MapID, InstanceID, 1, 1) == 4 then
		local xuanyuan = map.GetMapScriptData(MapID, InstanceID, 1, 22)
		local stringtbl_quest = 12401
		local monstersay_quest = 30201
		local stringtbl_answer = 12501
		local monstersay_answer = 30301

		if k == 1 then
		    local baiyao_quest = {}
			for i = 1,68 do
				table.insert(baiyao_quest,i)
			end
			for i = 1,10 do
			    if baiyaota_quest[InstanceID] == nil then
				    baiyaota_quest[InstanceID] = {}
				end
			    baiyaota_quest[InstanceID][i] = baiyao_quest[math.random(69-i)]
				table.remove(baiyao_quest,baiyaota_quest[InstanceID][i])
			end
		end

		if k % 2 == 1 then
		    map.MapCreateCreature(MapID, InstanceID, 4902012, 1, 40, 1)
		    local index = (k+1)/2
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, stringtbl_quest+baiyaota_quest[InstanceID][index])
			msg.AddMsgEvent(MsgID, 9, index)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, stringtbl_quest+baiyaota_quest[InstanceID][index])
			msg.AddMsgEvent(MsgID, 9, index)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

			--cre.MonsterSay(MapID, InstanceID, xuanyuan, monstersay_quest+baiyaota_quest[InstanceID][index])

		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12340)	--    15秒后选错的玩家将被传送出抉择之间
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		else
		    local index = k/2
			local area = 218
			if baiyaota_quest[InstanceID][index] > 34 then
				area = 219
			end
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, stringtbl_answer+baiyaota_quest[InstanceID][index])
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

			cre.MonsterSay(MapID, InstanceID, xuanyuan, monstersay_answer+baiyaota_quest[InstanceID][index])

			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)

			local num = 0
			for u,v in pairs(RoleTbl) do
			    if BYT_IsInArea(MapID, InstanceID, v, area) then
				    num = num + 1
				elseif not BYT_IsInArea(MapID, InstanceID, v, 203) and not BYT_IsInArea(MapID, InstanceID, v, 205) then
				    unit.CancelBuff(MapID, InstanceID, v, 1130201)
				    role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 118, 1, 324)
				end
			end
			if num > 0 and k ~= 16 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 12341)	--15秒后开始提问。站在轩辕右手边的光圈代表“是”，站在左手边光圈代表“否”
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
				--[[local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 12341)	--15秒后开始提问。站在轩辕右手边的光圈代表“是”，站在左手边光圈代表“否”
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)]]
				map.MapCreateCreature(MapID, InstanceID, 4902012, 1, 40, 1)
			elseif num > 0 and k == 16 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 12342)	--特殊事件完成，15秒后将会在百妖塔刷出奖励宝箱
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
				map.MapCreateCreature(MapID, InstanceID, 4902012, 1, 40, 1)
			else
			    baiyaota_quest[InstanceID] = nil
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 12343)	--特殊事件失败，所有玩家被传送至百妖塔
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
				map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
			end
		end
	end

end

aux.RegisterCreatureEvent(4902012, 13, "c4902012_OnDisappear")

function c4902013_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)

	map.MapCreateCreature(MapID, InstanceID, 4902014, 1, 40, 1)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12344)	--5秒后开始刷新“蛋”
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

aux.RegisterCreatureEvent(4902013, 13, "c4902013_OnDisappear")

function c4902014_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
	local k = map.GetMapScriptData(MapID, InstanceID, 1, 23) + 1
	map.SetMapScriptData(MapID, InstanceID, 1, 23, k)

	if k >= 36 then
	    baiyaota_egg[InstanceID] = nil
	    map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
	    local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for u,v in pairs(RoleTbl) do
			if BYT_IsInArea(MapID, InstanceID, v, 204) then
				role.RoleGotoNewMap(MapID, InstanceID, v, 3067630287, 118, 1, 324)
			end
		end
		local point =  map.GetMapScriptData(MapID, InstanceID, 1, 25)
		local point_Total =  map.GetMapScriptData(MapID, InstanceID, 1, 26)
		if math.floor(point_Total/point) < 2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12345)	--特殊事件完成，奖励宝箱已刷新！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			map.MapCreateCreature(MapID, InstanceID, 4902017, 125, 40, 341)
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12346)	--特殊事件失败，未能获得奖励
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end
	else
	    map.MapCreateCreature(MapID, InstanceID, 4902014, 1, 40, 1)
		local EggPoint = {}
		EggPoint[1] = {x=220,y=40,z=415}
		EggPoint[2] = {x=243,y=40,z=415}
		EggPoint[3] = {x=266,y=40,z=415}
		EggPoint[4] = {x=289,y=40,z=415}
		EggPoint[5] = {x=312,y=40,z=415}
		EggPoint[6] = {x=335,y=40,z=415}
		EggPoint[7] = {x=358,y=40,z=415}
		EggPoint[8] = {x=220,y=40,z=392}
		EggPoint[9] = {x=243,y=40,z=392}
		EggPoint[10] = {x=266,y=40,z=392}
		EggPoint[11] = {x=289,y=40,z=392}
		EggPoint[12] = {x=312,y=40,z=392}
		EggPoint[13] = {x=335,y=40,z=392}
		EggPoint[14] = {x=358,y=40,z=392}
		EggPoint[15] = {x=220,y=40,z=369}
		EggPoint[16] = {x=243,y=40,z=369}
		EggPoint[17] = {x=266,y=40,z=369}
		EggPoint[18] = {x=289,y=40,z=369}
		EggPoint[19] = {x=312,y=40,z=369}
		EggPoint[20] = {x=335,y=40,z=369}
		EggPoint[21] = {x=358,y=40,z=369}
		EggPoint[22] = {x=220,y=40,z=346}
		EggPoint[23] = {x=243,y=40,z=346}
		EggPoint[24] = {x=266,y=40,z=346}
		EggPoint[25] = {x=289,y=40,z=346}
		EggPoint[26] = {x=312,y=40,z=346}
		EggPoint[27] = {x=335,y=40,z=346}
		EggPoint[28] = {x=358,y=40,z=346}
		EggPoint[29] = {x=220,y=40,z=323}
		EggPoint[30] = {x=243,y=40,z=323}
		EggPoint[31] = {x=266,y=40,z=323}
		EggPoint[32] = {x=289,y=40,z=323}
		EggPoint[33] = {x=312,y=40,z=323}
		EggPoint[34] = {x=335,y=40,z=323}
		EggPoint[35] = {x=358,y=40,z=323}
		EggPoint[36] = {x=220,y=40,z=300}
		EggPoint[37] = {x=243,y=40,z=300}
		EggPoint[38] = {x=266,y=40,z=300}
		EggPoint[39] = {x=289,y=40,z=300}
		EggPoint[40] = {x=312,y=40,z=300}
		EggPoint[41] = {x=335,y=40,z=300}
		EggPoint[42] = {x=358,y=40,z=300}
		EggPoint[43] = {x=220,y=40,z=277}
		EggPoint[44] = {x=243,y=40,z=277}
		EggPoint[45] = {x=266,y=40,z=277}
		EggPoint[46] = {x=289,y=40,z=277}
		EggPoint[47] = {x=312,y=40,z=277}
		EggPoint[48] = {x=335,y=40,z=277}
		EggPoint[49] = {x=358,y=40,z=277}

		if k == 1 then
		    baiyaota_egg[InstanceID] = {}
		    for i = 1,49 do
				baiyaota_egg[InstanceID][i] = i
            end
        end
		if baiyaota_egg[InstanceID] ~= nil then
		    for i = 1,5 do
			    local index = math.random(#baiyaota_egg[InstanceID])
				if math.random(100) > 85 then
					local mon = map.MapCreateCreature(MapID, InstanceID, 1535437, EggPoint[baiyaota_egg[InstanceID][index]].x, 40, EggPoint[baiyaota_egg[InstanceID][index]].z)
					map.SetMapScriptData(MapID, InstanceID, 1, 26, map.GetMapScriptData(MapID, InstanceID, 1, 26)+5)
		            cre.SetCreatureScriptData(MapID, InstanceID, mon, 1, 0, baiyaota_egg[InstanceID][index])
				else
				    local mon = map.MapCreateCreature(MapID, InstanceID, 1535436, EggPoint[baiyaota_egg[InstanceID][index]].x, 40, EggPoint[baiyaota_egg[InstanceID][index]].z)
				    map.SetMapScriptData(MapID, InstanceID, 1, 26, map.GetMapScriptData(MapID, InstanceID, 1, 26)+1)
				    cre.SetCreatureScriptData(MapID, InstanceID, mon, 1, 0, baiyaota_egg[InstanceID][index])
				end
				table.remove(baiyaota_egg[InstanceID],index)
			end
		end
		if k % 6 == 0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12347)	--距离特殊事件结束还有XXX秒
			msg.AddMsgEvent(MsgID, 9, (36-k)*5)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end
	end
end

aux.RegisterCreatureEvent(4902014, 13, "c4902014_OnDisappear")

--消失
function C1535435_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12348)	--XX消失，特殊挑战失败
	msg.AddMsgEvent(MsgID, 5, TargetTypeID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
end

aux.RegisterCreatureEvent(1535435, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535438, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535439, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535440, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535441, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535442, 13, "C1535435_OnDisappear")
aux.RegisterCreatureEvent(1535443, 13, "C1535435_OnDisappear")

--消失
function C1535436_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local k = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
	if baiyaota_egg[InstanceID] ~= nil then
		table.insert(baiyaota_egg[InstanceID],k)
	end

end

aux.RegisterCreatureEvent(1535436, 13, "C1535436_OnDisappear")

--消失
function C1535437_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local k = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
	if baiyaota_egg[InstanceID] ~= nil then
		table.insert(baiyaota_egg[InstanceID],k)
	end

end

aux.RegisterCreatureEvent(1535437, 13, "C1535437_OnDisappear")

--玩家m50死亡
function m50_PlayerDie(MapID, InstanceID, RoleID, KillerID)
    if BYT_IsInArea(MapID, InstanceID, RoleID, 202) then
	    --role.ForcedToRevive(MapID, InstanceID, RoleID)
		--role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 118, 1, 324)
	    local k = map.GetMapScriptData(MapID, InstanceID, 1, 21)
		k = k - 1
		if k < 0 then
		    k = 0
		end
	    map.SetMapScriptData(MapID, InstanceID, 1, 21, k)
		if k == 0 then
		    map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12349)    --                <color=0xffff0000>梦魇狂狼挑战失败！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		end
	end
end
aux.RegisterMapEvent("m50", 5, "m50_PlayerDie")

--玩家离开

function m50_LeaveMap(MapID, InstanceID, RoleID)
    if BYT_IsInArea(MapID, InstanceID, RoleID, 202) then
	    local k = map.GetMapScriptData(MapID, InstanceID, 1, 21)
		k = k - 1
		if k < 0 then
		    k = 0
		end
	    map.SetMapScriptData(MapID, InstanceID, 1, 21, k)
		if k == 0 then
		    map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12350)    --                <color=0xffff0000>梦魇狂狼挑战失败！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		end
	end
end
aux.RegisterMapEvent("m50", 3, "m50_LeaveMap")

--[[玩家进入
function m50_OnPlayerEnter(MapID, InstanceID, RoleID)
    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, 118, 1, 324)
end
aux.RegisterMapEvent("m50", 2, "m50_OnPlayerEnter")]]


--玩家进入区域
function m50_OnRoleEnterArea(MapID, InstanceID, RoleID, ObjAreaID)
    if ObjAreaID >= 208 and ObjAreaID <= 212 then
	    local TransPoint = {}
		TransPoint[1] = {x=205,y=40,z=185}
		TransPoint[2] = {x=141,y=40,z=178}
		TransPoint[3] = {x=166,y=40,z=84}
		TransPoint[4] = {x=176,y=40,z=215}
		TransPoint[5] = {x=269,y=40,z=125}
		TransPoint[6] = {x=74,y=40,z=110}
		--TransPoint[7] = {x=343,y=40,z=106}
		--TransPoint[8] = {x=326,y=40,z=60}
		--TransPoint[9] = {x=379,y=40,z=80}
		local point = math.random(6)
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067630287, TransPoint[point].x, 1, TransPoint[point].z)
	end
end

aux.RegisterMapEvent("m50", 7, "m50_OnRoleEnterArea")

--[[载入怪物
function C4902010_OnInit(MapID, InstanceID, TargetID)
    map.SetMapScriptData(MapID, InstanceID, 1, 22, TargetID)
end

aux.RegisterCreatureEvent(4902010, 4, "C4902010_OnInit")]]

--玩家在活动中复活
function m50_RoleRevive(MapID, InstanceID, RoleID, ReviveType, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID)
	--玩家选择回城复活
	if not BYT_IsInArea(MapID, InstanceID, RoleID, 203) then
		ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
		ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
		fx = 118
		fy = 1
		fz = 324
		RebornMapID = 3067630287
	end
	return ReviveHP, ReviveMP, fx, fy, fz, RebornMapID
end

aux.RegisterMapEvent("m50",  11, "m50_RoleRevive")

function BYT_IsInArea(MapID, InstanceID, RoleID, area)
    local x,y,z = unit.GetPosition(MapID, InstanceID, RoleID)
    if area == 202 then  --狼
		if x <= 303 and x >= 41 and z >= 36 and z <= 252 then
		    return true
		else
		    return false
		end
	elseif area == 206 then --抉择
	    if x <= 377 and x >= 310 and z >= 87 and z <= 156 then
		    return true
		else
		    return false
		end
	else
	    return map.IsInArea(MapID, InstanceID, RoleID, area)
	end
end
