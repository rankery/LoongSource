
function moshenchuansong(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Jidou_Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"])
	local Jidou_Last = role.GetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Last"])

	local curdate = tonumber(os.date("%j"))

	if curdate ~= Jidou_Last then
		Jidou_Last = curdate
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Last"], Jidou_Last)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"], 0)
	end
-----------------------------------------------局部函数定义------------------------------------------------
	local function MSJD_JoinTeam(MapID, InstanceID, tbl)        --tbl必须是一个8个元素的玩家ID表
	    for k,v in pairs(tbl) do
			local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, v)
			if TeamID ~= nil and TeamID ~= 4294967295 then
				role.LetRoleLeaveTeam(TeamID, v)
			end
		end
        local TeamID1 = role.CreatANewTeam(tbl[1], tbl[2])
		role.LetRoleJoinTeam(TeamID1, tbl[3])
		role.LetRoleJoinTeam(TeamID1, tbl[4])
		local TeamID2 = role.CreatANewTeam(tbl[5], tbl[6])
		role.LetRoleJoinTeam(TeamID2, tbl[7])
		role.LetRoleJoinTeam(TeamID2, tbl[8])
	end

-----------------------------------------------默认值-------------------------------------------------
	local function MSJD_VALUE(InctancID,tbl)        --tbl必须是一个8个元素的玩家ID表
	    Jidou.RoomInfo[InctancID] = {IsOver = 0,BeginTime = os.time(),Timer30=0,Role = {},BlueFlag = {},RedFlag={},BuffTrigger = {}}      --记录了开始时间和消失了多少个30秒计时器
		Jidou.RoomInfo[InctancID].RedFlag = {State="home",Role=0,Time=0,FlagID=0,BianShen={1123501,1123501,1123601,1123601,1123401,1123401,1123801,1123801}}  --旗子有两个状态：home和outside。当旗子为outside时，如果在地上，则Role为0，否则标明在谁身上。当旗子在地上时，Time会每30秒加1，如果当它等于2还没有人去抢它的话，它会自己回到家中。
		Jidou.RoomInfo[InctancID].BlueFlag = {State="home",Role=0,Time=0,FlagID=0,BianShen={1123701,1123701,1123101,1123101,1123201,1123201,1123301,1123301}} --Bianshen记录的是所有可用的变身BUFF。如果玩家选择了其中一个，则删掉。
		Jidou.RoomInfo[InctancID].BuffTrigger = {}
		Jidou.RoomInfo[InctancID].BuffTrigger[380] ={CreID=0,CreTypeID=0,x=460,y=5573,z=107}--记录每个触发器上的怪物的ID和TYPEID，可以用来判断这个触发器上是否有BUFF可以吃
		Jidou.RoomInfo[InctancID].BuffTrigger[383] ={CreID=0,CreTypeID=0,x=79,y=5590,z=447}
		Jidou.RoomInfo[InctancID].BuffTrigger[388] ={CreID=0,CreTypeID=0,x=282,y=7302,z=284}
		Jidou.RoomInfo[InctancID].BuffTrigger[390] ={CreID=0,CreTypeID=0,x=195,y=5248,z=177}
		Jidou.RoomInfo[InctancID].BuffTrigger[396] ={CreID=0,CreTypeID=0,x=136,y=4448,z=102}
		for i=1,4 do
			Jidou.RoomInfo[InctancID].Role[tbl[i]] = {Name = tostring(role.GetRoleName(tbl[i])),Side = "Blue",Time = 1200,GetFlag = 0,MissFlag = 0, Kill = 0, Die = 0,Level = 0,Leave = 0} --level记录了吃掉了多少BUFF。吃掉10个升一级.Leave记录了玩家是否曾离开过该副本
		end
		for i=5,8 do
			Jidou.RoomInfo[InctancID].Role[tbl[i]] = {Name = tostring(role.GetRoleName(tbl[i])),Side = "Red",Time = 1200,GetFlag = 0,MissFlag = 0, Kill = 0, Die = 0,Level = 0,Leave = 0}
		end
	end
----------------------------------------------创建副本并刷怪-----------------------------------------------

    local function CreatNewMap(Index)

		map.CreateInstance(2108961076, Index)
		map.MapCreateCreature(2108961076,Index,4902135,1,1,1)

    end


------------------------------------------------传送及状态判断-----------------------------------------
    local function TransPlayerToMap(MapID, InstanceID, tbl)
		for k,v in pairs(tbl) do
			unit.CancelBuff(MapID, InstanceID,v, 1140901)        --取消    魔神激斗排队中
			if role.IsRoleInStatus(v, 1024) then        --取消摆摊
				role.CancelRoleStallState(MapID, InstanceID, v)
			end
			if unit.IsDead(MapID, InstanceID, v) then
				role.ForcedToRevive(MapID, InstanceID, v)
			end
			if k <= 4 then
				role.RoleGotoNewMap(MapID, InstanceID, v, 2108961076,81,112,426)--传送到蓝方阵地
			else
				role.RoleGotoNewMap(MapID, InstanceID, v, 2108961076, 442,111,105)--传送到红方阵地
			end
		end
	end
------------------------------------------------以下为对话---------------------------------------------
	if TalkIndex == -1 then
        if act.GetActIsStart(161) or act.GetActIsStart(162) then
		    local bool = 0
			for k,v in pairs(Jidou.Singlewaitinglist) do
				if v == RoleID then
					bool = 1
				end
			end
			for k,v in pairs(Jidou.Teamwaitinglist) do
				if v == RoleID then
					bool = 2
				end
			end
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	3701)		--请选择
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	3702)		--“单人报名”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	3703)		--“队伍报名”
			msg.AddMsgEvent(MsgID, 21,	6)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	12930)		    --领取奖励
			if bool > 0 then
				msg.AddMsgEvent(MsgID, 21,	7)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	12931)			--“取消报名”
			end
			msg.AddMsgEvent(MsgID, 21,	8)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12932)		--活动没有开始，不能报名
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	6)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	12930)		    --领取奖励
			msg.AddMsgEvent(MsgID, 21,	8)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end

	elseif TalkIndex == 4 then
        if act.GetActIsStart(161) or act.GetActIsStart(162) then
			--限制条件判断
			--是否已经报名
			local bool = true
			for k,v in pairs(Jidou.Singlewaitinglist) do
				if v == RoleID then
					bool = false
				end
			end
			for k,v in pairs(Jidou.Teamwaitinglist) do
				if v == RoleID then
					bool = false
				end
			end

			if not bool then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3704)		--不能重复报名
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif rolelevel<50 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3705)		--50级以下玩家不可参加魔神激斗
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			elseif Jidou_Times>=5 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3706)		--今日已参加过5次激斗
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else

				--将玩家加入等待列表

				table.insert(Jidou.Singlewaitinglist,RoleID)
				unit.AddBuff(MapID, InstanceID, RoleID, 1140901, RoleID)        --为玩家添加一个状态：魔神激斗排队中。改状态切换场景或者下线后消失

				if #Jidou.Singlewaitinglist == 8 then

					Jidou.RoomIndex = Jidou.RoomIndex + 1

					--创建副本
					CreatNewMap(Jidou.RoomIndex)

					--分配阵营
					local tbl = {}
					for i = 1,4 do
						tbl[i] = Jidou.Singlewaitinglist[i*2-1]
					end
					for i = 5,8 do
						tbl[i] = Jidou.Singlewaitinglist[(i-4)*2]
					end

					--为玩家组队
					MSJD_JoinTeam(MapID, InstanceID, tbl)

					--设定默认值
					MSJD_VALUE(Jidou.RoomIndex,tbl)

					--为每个没有怪物的触发器刷一个怪物
                    for k,v in pairs(Jidou.RoomInfo[Jidou.RoomIndex].BuffTrigger) do
					    if v.CreID == 0 then
						    v.CreTypeID = math.random(4) + 4902118
							v.CreID = map.MapCreateColCreature(MapID, InstanceID, v.CreTypeID, v.x, v.y, v.z, 1)
						end
					end

					--传送玩家进战场
					TransPlayerToMap(MapID, InstanceID, tbl)

					Jidou.Singlewaitinglist = {}
				else

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 71, 3707)			--您已成功报名魔神激斗………………
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

				end

			end

		end
    elseif TalkIndex == 5 then
        if act.GetActIsStart(161) or act.GetActIsStart(162) then
			--限制条件判断
			--是否已经报名

			local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			if TeamID == 4294967295 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	12934)		--必须组队才能报名
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    local LeaderID = team.GetTeamLeaderID(TeamID)
				if LeaderID ~= RoleID then
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	12935)		--只有队长才能报名
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local Role = {}
					Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
					local num = 0
					local bool = true
					local map_bool = true
					local level = true
					local times = true
					for i=1, 6 do
					    if Role[i] and Role[i] ~= 4294967295 then

							for k,v in pairs(Jidou.Singlewaitinglist) do
								if v == Role[i] then
									local MsgID = msg.BeginMsgEvent()
									msg.AddMsgEvent(MsgID, 20,	12936)		--队伍中已经有人报名
									msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
									msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
									msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
									msg.DispatchRoleMsgEvent(RoleID, MsgID)
									bool = false
								end
							end
							for k,v in pairs(Jidou.Teamwaitinglist) do
								if v == Role[i] then
									local MsgID = msg.BeginMsgEvent()
									msg.AddMsgEvent(MsgID, 20,	12936)		--队伍中已经有人报名
									msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
									msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
									msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
									msg.DispatchRoleMsgEvent(RoleID, MsgID)
									bool = false
								end
							end
							local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
							if Target_MapID ~= 3017298383 then
							    map_bool = false
							end
							if role.GetRoleLevel(Target_MapID, Target_InstanceID, Role[i])< 50 then
							    level = false
							end
							local Jidou_Times = role.GetRoleScriptData(Role[i], 1, RoleDataType["Jidou_Times"])
							local Jidou_Last = role.GetRoleScriptData(Role[i], 1, RoleDataType["Jidou_Last"])

							local curdate = tonumber(os.date("%j"))

							if curdate ~= Jidou_Last then
								Jidou_Last = curdate
								role.SetRoleScriptData(Role[i], 1, RoleDataType["Jidou_Last"], Jidou_Last)
								role.SetRoleScriptData(Role[i], 1, RoleDataType["Jidou_Times"], 0)
							end
							if Jidou_Times >= 5 then
							    times = false
							end

							num = num + 1
						end
					end
					if bool then
					    if num ~= 4 then
						    local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20,	12937)		--玩家的数量必须是4人
							msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
							msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
							msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						elseif not map_bool then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20,	12938)		--必须所有玩家都在凤翔才能报名
							msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
							msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
							msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						elseif not level then
						    local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20,	12939)		--等级必须都在50以上
							msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
							msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
							msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						elseif not times then
				            local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20,	12940)		--有人今天已经打了5次
							msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
							msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
							msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						else

							--将玩家加入等待列表
							local maxnum = 4
							for i = 1,6 do
							    if Role[i] and Role[i] ~= 4294967295 and maxnum > 0 then
								    maxnum = maxnum - 1
									table.insert(Jidou.Teamwaitinglist,Role[i])
									unit.AddBuff(MapID, InstanceID, Role[i], 1140901, Role[i])        --为玩家添加一个状态：魔神激斗排队中。改状态切换场景或者下线后消失
								end
							end

							if #Jidou.Teamwaitinglist == 8 then

								Jidou.RoomIndex = Jidou.RoomIndex + 1

								--创建副本
								CreatNewMap(Jidou.RoomIndex)

								--分配阵营
								local tbl = {}
								for i = 1,8 do
									tbl[i] = Jidou.Teamwaitinglist[i]
								end

								--为玩家组队
								MSJD_JoinTeam(MapID, InstanceID, tbl)

								--设定默认值
								MSJD_VALUE(Jidou.RoomIndex,tbl)

								--为每个没有怪物的触发器刷一个怪物
								for k,v in pairs(Jidou.RoomInfo[Jidou.RoomIndex].BuffTrigger) do
									if v.CreID == 0 then
										v.CreTypeID = math.random(4) + 4902118
										v.CreID = map.MapCreateColCreature(MapID, InstanceID, v.CreTypeID, v.x, v.y, v.z, 1)
									end
								end

								--传送玩家进战场
								TransPlayerToMap(MapID, InstanceID, tbl)

								Jidou.Teamwaitinglist = {}
							else
								for i = 1,6 do
									if Role[i] and Role[i] ~= 4294967295 then
										local MsgID = msg.BeginMsgEvent()
										msg.AddMsgEvent(MsgID, 71, 3707)			--您已成功报名魔神激斗………………
										msg.DispatchRoleMsgEvent(RoleID, MsgID)
									end
								end
							end
						end
					end
				end
			end
		end
	elseif TalkIndex == 6 then
		--代币最好做成9999个的叠加上限
        local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["MSJD_Award"])
		if k <= 0 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12941)		--您没有可以领取的奖励
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif role.GetBagFreeSize(RoleID) < 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12942)		--你有XX个可以领，请清空背包再来
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 9,	k)
			msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	12943)		--你已经领取了XX个
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 9,	k)
			msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		    role.AddRoleItem(MapID, InstanceID, RoleID, 6010840, k, -1, 8, 420)    --魔之徽章未有ID
			role.SetRoleScriptData(RoleID, 1, RoleDataType["MSJD_Award"],0)
		end
	elseif TalkIndex == 7 then
        local bool = 0
		local index = 0
		for k,v in pairs(Jidou.Singlewaitinglist) do
			if v == RoleID then
				bool = 1
				index = k
			end
		end
		for k,v in pairs(Jidou.Teamwaitinglist) do
			if v == RoleID then
				bool = 2
				index = k
			end
		end
		if bool == 1 then
		    unit.CancelBuff(MapID, InstanceID,RoleID, 1140901) --取消魔神排队状态
		    table.remove(Jidou.Singlewaitinglist,index)
            local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 12909)			--您已离开了魔神斗技场的排队序列
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif bool == 2 then
		    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			if TeamID ~= 4294967295 then
			    local LeaderID = team.GetTeamLeaderID(TeamID)
				if LeaderID ~= RoleID then
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	12945)		--只有队长才能取消
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					if index < 5 then
						for i = 1,4 do
						    local i = 5-i          --为了从后往前删掉玩家
						    local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 71, 12909)			--您已离开了魔神斗技场的排队序列
							msg.DispatchRoleMsgEvent(Jidou.Teamwaitinglist[i], MsgID)
							unit.CancelBuff(MapID, InstanceID,Jidou.Teamwaitinglist[i], 1140901)--取消魔神排队状态
							table.remove(Jidou.Teamwaitinglist,i)
						end
					elseif index < 9 then
			            for i = 5,8 do
                            local i = 13-i			 --为了从后往前删掉玩家
						    local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 71, 12909)			--您已离开了魔神斗技场的排队序列
							msg.DispatchRoleMsgEvent(Jidou.Teamwaitinglist[i], MsgID)
							unit.CancelBuff(MapID, InstanceID,Jidou.Teamwaitinglist[i],1140901 )--取消魔神排队状态
							table.remove(Jidou.Teamwaitinglist,i)
						end
					end
				end
			end
		end

	end
end




function moshenchuanchu(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local ChuanchuPos = {}
	ChuanchuPos[1] = {x=866,y=10000,z=1994}
	ChuanchuPos[2] = {x=719,y=10000,z=2098}
	ChuanchuPos[3] = {x=1080,y=10000,z=2130}
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	12947)		--NPC主界面对话文字
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)
		msg.AddMsgEvent(MsgID, 1,	12948)		--退出战场
		msg.AddMsgEvent(MsgID, 21,	5)
		msg.AddMsgEvent(MsgID, 1,	22)			--取消
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		local i = math.random(3)
		role.RoleGotoNewMap(MapID, InstanceID,RoleID,3017298383,ChuanchuPos[i].x ,ChuanchuPos[i].y ,ChuanchuPos[i].z)
	end
end
function CreatNPC_MSJD(MapID,InstanceID,CreTypeID,CreAi)

		local bianshen = {}
		bianshen[4902132] = {BuffID=1123701,x=62,y=5588,z=402}--蓝方3个变身NPC
		bianshen[4902109] = {BuffID=1123101,x=62,y=5582,z=407}--蓝方3个变身NPC
		bianshen[4902110] = {BuffID=1123201,x=58,y=5574,z=419}--蓝方3个变身NPC
		bianshen[4902111] = {BuffID=1123301,x=58,y=5566,z=425}--蓝方3个变身NPC
		bianshen[4902106] =	{BuffID=1123501,x=460,y=5550,z=129}--红方3个变身NPC
		bianshen[4902107] =	{BuffID=1123601,x=462,y=5537,z=120}--红方3个变身NPC
		bianshen[4902108] =	{BuffID=1123401,x=452,y=5376,z=80}--红方3个变身NPC
		bianshen[4902131] =	{BuffID=1123801,x=450,y=5348,z=75}--红方3个变身NPC

		for k,v in pairs(bianshen) do
			local creid = map.MapCreateCreature(MapID, InstanceID, k, v.x, v.y, v.z)--蓝方3个变身NPC
		    cre.SetCreatureScriptData(MapID, InstanceID, creid, 1, 0, v.BuffID)      --变身NPC的第0个位置记录它对应的什么BUFF
		end

		map.MapCreateCreature(MapID, InstanceID, 4902112,97,5556,406)--蓝方蓝旗
		map.MapCreateCreature(MapID, InstanceID, 4902114,64,5555,440)--蓝方空旗
		map.MapCreateCreature(MapID, InstanceID, 4902113,425,5483,109)--红方红旗
		map.MapCreateCreature(MapID, InstanceID, 4902133,464,5517,96)--红方空旗，两边的空旗不用同样的空旗杆，新做一个空旗杆ID

		map.MapCreateCreature(MapID, InstanceID, 4902134, 1, 1, 1)--刷新一个30秒的透明计时器，该怪物未提需求
end

aux.RegisterCreatureEvent(4902135, 13, "CreatNPC_MSJD")--计时器消失事件注册
aux.RegisterCreatureEvent(4902118, 7, "moshenchuanchu")
aux.RegisterCreatureEvent(4902101, 7, "moshenchuansong")
aux.RegisterCreatureEvent(4902102, 7, "moshenchuansong")
aux.RegisterCreatureEvent(4902103, 7, "moshenchuansong")



