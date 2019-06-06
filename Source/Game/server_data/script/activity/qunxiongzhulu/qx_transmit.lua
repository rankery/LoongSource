--[[根据玩等级得到副本ID
local function RoleLevel2InstanceID(RoleLevel)
	if RoleLevel == nil then
		return 0
	end

	if RoleLevel < 40 then
		return 0
	elseif RoleLevel < 60 then
		return 1
	elseif RoleLevel < 80 then
		return 2
	elseif RoleLevel < 100 then
		return 3
	else
		return 4
	end
end

----判断玩家队伍是否合法
local function TestRoleTeam(MapID, InstanceID, RoleID, TeamRoleData, InstanceIndex)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])
	if TeamID ~= 4294967295 then
	    --获得小队玩家ID
	    local Role = {}
	    Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	    for i = 1, 6 do]]
--			if Role[i]~=4294967295 and (not TeamRoleData[TeamKey][Role[i]]) then
--[[			return false
			end
		end

		--判断报名时的小队成员是否在同一队伍
	    for k in pairs(TeamRoleData[TeamKey]) do
		    local ID = role.IsRoleHaveTeam(2983743695, InstanceIndex, k)
		    if ID and ID ~= TeamID then
			    return false
		    end
	    end

	    return true
	else
		return true
	end
end


--判断玩家是否为死亡后离开活动
local function IsDeadLeave(MapID, InstanceID, RoleID, RoleLevel)
	local RoleExpire = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"])
	local TeamKey = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"])

	--if RoleLevel < 60 then
		if RoleExpire ~= act.GetActScriptData(18, 1, 0) then
			return false, 0
		elseif Act18_TeamRoleData[TeamKey] and Act18_TeamRoleData[TeamKey][RoleID] then
			return true, 2000
		end
	--elseif RoleLevel < 80 then
		if RoleExpire ~= act.GetActScriptData(19, 1, 0) then
			return false, 0
		elseif Act19_TeamRoleData[TeamKey] and Act19_TeamRoleData[TeamKey][RoleID] then
			return true, 5000
		end
	--elseif RoleLevel < 100 then
		if RoleExpire ~= act.GetActScriptData(20, 1, 0) then
			return false, 0
		elseif Act20_TeamRoleData[TeamKey] and Act20_TeamRoleData[TeamKey][RoleID] then
			return true, 10000
		end
	--else
		if RoleExpire ~= act.GetActScriptData(21, 1, 0) then
			return false, 0
		elseif Act21_TeamRoleData[TeamKey] and Act21_TeamRoleData[TeamKey][RoleID] then
			return true, 20000
		else
		    return false, 0
		end
	--end
end

--清空活动数据
function ClearAct18Data(TeamRoleData, TeamData)
	local Empty = {}
	TeamRoleData = Empty
	TeamData = Empty
end

--检测玩家是否参加过活动
function TestRoleExpire(MapID, InstanceID, RoleID)
    local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
    local InstanceIndex = RoleLevel2InstanceID(RoleLevel)
    local RoleExpire = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"])

    if InstanceIndex == 1 then
        if RoleExpire == Act18_Expire then
            return true
        end
    end

    if InstanceIndex == 2 then
        if RoleExpire == Act19_Expire then
            return true
        end
    end

    if InstanceIndex == 3 then
        if RoleExpire == Act20_Expire then
            return true
        end
    end

    if InstanceIndex == 4 then
        if RoleExpire == Act21_Expire then
            return true
        end
    end

    return false
end

--NPC传送进入场景todo
function C4500201_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	--判断活动是否已经开始
	local bStart = (act.GetActIsStart(18) or act.GetActIsStart(103))

	if -1 == TalkIndex then
	    if PVPactivity_trigger == 1 then
			--活动已经开始
			if bStart then
				local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				if RoleLevel < 40 then
					--提示玩家等级不够，无法进入活动
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	157)
					msg.AddMsgEvent(MsgID, 21,	5)			    --取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					return
				end

				--如果玩家是死亡后离开，提示玩家再次进入需要花费金钱
				local bDeadLeave, Money = IsDeadLeave(MapID, InstanceID, RoleID, RoleLevel)
				if bDeadLeave then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 151)
					msg.AddMsgEvent(MsgID, 9, Money)
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	4)			--确定
					msg.AddMsgEvent(MsgID, 1,	21)
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif TestRoleExpire(MapID, InstanceID, RoleID) then  --如果玩家是掉线后离开的活动
					--提示玩家是否进入活动场景
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	160)		--您是否要进入英雄乱武活动？
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	4)			--确定
					msg.AddMsgEvent(MsgID, 1,	21)
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					--提示玩家没有报名，无法进入活动
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	162)		--群雄逐鹿活动报名时间还没有开始
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else --活动还没有开始
				--检测是否为活动的报名时间
				local Hour = tonumber(os.date("%H"))
				local Min  = tonumber(os.date("%M"))
				local Week = tonumber(os.date("%w"))
				if (Week == 0 or Week == 3) and Hour == 19 and Min >=50 then
					--判断玩家是否已经报过名
					if TestRoleExpire(MapID, InstanceID, RoleID) then  --如果玩家是掉线后离开的活动
						--提示玩家是否进入活动场景
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20,	160)		--您是否要进入英雄乱武活动？
						msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
						msg.AddMsgEvent(MsgID, 21,	4)			--确定
						msg.AddMsgEvent(MsgID, 1,	21)
						msg.AddMsgEvent(MsgID, 21,	5)			--取消
						msg.AddMsgEvent(MsgID, 1,	22)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						return
					end

					--玩家还没有报名
					local Success = true
					local LeaderID = 4294967295

					--判断报名的是否是队长
					local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
					if TeamID == -1 then
						Success = false
					end

					if Success then
						LeaderID = team.GetTeamLeaderID(TeamID)
						if LeaderID ~= RoleID then
							Success = false
						end
					end

					--判断小队成员个数和等级
					if Success then
						--获得小队玩家ID
						local Role = {}
						Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
						local InstanceIndex = RoleLevel2InstanceID(role.GetRoleLevel(MapID, InstanceID, RoleID))
						for k in pairs(Role) do
							if 4294967295 ~= Role[k] then
								local RoleLevel = role.GetRoleLevel(MapID, InstanceID, Role[k])
								if InstanceIndex ~= RoleLevel2InstanceID(RoleLevel) then
									Success = false
									break
								end
							else
								Success = false
								break
							end
						end
					end

					if Success then
						--提示玩家是否进入活动场景
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20,	160)		--您是否要进入英雄乱武活动？
						msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
						msg.AddMsgEvent(MsgID, 21,	4)			--确定
						msg.AddMsgEvent(MsgID, 1,	21)
						msg.AddMsgEvent(MsgID, 21,	5)			--取消
						msg.AddMsgEvent(MsgID, 1,	22)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)

					else
						--提示玩家提示：“您不符合报名条件。参加群雄逐鹿活动必须以组队方式报名，且队伍必须组满6人，报名人必须为队长，队员等级必须在同一组别内。”
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20,	161)
						msg.AddMsgEvent(MsgID, 21,	5)			--取消
						msg.AddMsgEvent(MsgID, 1,	22)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				else
					--提示玩家没有到活动报名时间
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	162)		--群雄逐鹿活动报名时间还没有开始
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		else
		    --提示玩家活动没有开始
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2863)			--群雄逐鹿活动暂不开放，敬请谅解。
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
			msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end

	--如果玩家选择确定
    if TalkIndex == 4 then
	    local InstanceIndex = RoleLevel2InstanceID(role.GetRoleLevel(MapID, InstanceID, RoleID))
	    --活动已经开始
        if bStart then
	        local RoleExpire = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"])
	        --侠客组
	        if InstanceIndex == 1 then
	            --得到玩家上次参加活动时的届数, 如果和当前活动届数不相等，不允许玩家进入
                if RoleExpire ~= act.GetActScriptData(18, 1, 0) then
	                return
	            else --判断玩家所在队伍是否是报名时的队伍
					local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act18_TeamRoleData, InstanceIndex)
					if false == bRet then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 2864)		--群雄逐鹿活动报名时间已过
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						return
					end
			    end
	        end

	        --豪杰组
	        if InstanceIndex == 2 then
                if RoleExpire ~= act.GetActScriptData(19, 1, 0) then
	                return
	            else --判断玩家所在队伍是否是报名时的队伍
	                local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act19_TeamRoleData, InstanceIndex)
					if false == bRet then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 2864)		--群雄逐鹿活动报名时间已过
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						return
					end
				end
	        end

	        --英雄组
	        if InstanceIndex == 3 then
                if RoleExpire ~= act.GetActScriptData(20, 1, 0) then
	                return
	            else --判断玩家所在队伍是否是报名时的队伍
	                local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act20_TeamRoleData, InstanceIndex)
					if false == bRet then
					    local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 2864)		--群雄逐鹿活动报名时间已过
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						return
					end
	            end
	        end

	        --天下组
	        if InstanceIndex == 4 then
                if RoleExpire ~= act.GetActScriptData(21, 1, 0) then
	                return
	            else --判断玩家所在队伍是否是报名时的队伍
	                local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act21_TeamRoleData, InstanceIndex)
					if false == bRet then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 2864)		--群雄逐鹿活动报名时间已过
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						return
					end
	            end
	        end

			--传送玩家到活动场景
			local Index = math.random(1, 8)
			role.RoleGotoNewMap(MapID, InstanceIndex, RoleID, 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
        else    --活动没有开始
			--得到玩家上次参加活动时的届数, 如果和当前活动届数不相等，检测玩家小队成员的等级和数量
			local RoleExpire = role.GetRoleScriptData(RoleID, 1, RoleDataType["Act18_Expire"])
	        if InstanceIndex == 1 then
	            --如果没有创建活动副本
                if IsCreateAct18Instance == 0 then
	                --初始化活动中玩家数据
				    ClearAct18Data(Act18_TeamRoleData, Act18_TeamData)
	                Act18_TeamIndex = 1

                    Act18_Expire = act.GetActScriptData(18, 1, 0)
	                if Act18_Expire == 0 then
					    --如果是第一届活动，初始化当前届数为10001（方便领取奖励时的判断）
					    Act18_Expire = 10000
	                end

	                --初始化活动当前届数
	                Act18_Expire = Act18_Expire + 1
	                act.SetActScriptData(18, 1, 0, Act18_Expire)
	                act.SaveActScriptData(18)

                    --创建活动副本
	                map.CreateInstance(2983743695, 1)
                    IsCreateAct18Instance = 1
	            end

                if RoleExpire ~= Act18_Expire then
					--判断小队成员个数和等级
		            local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
                    local Success = true

                    --获得小队玩家ID
		            local Role = {}
		            Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
        		    for k in pairs(Role) do
	    	    	    if Role[k] and 4294967295 ~= Role[k] then
							local RoleLevel = role.GetRoleLevel(MapID, InstanceID, Role[k])
			    	        if InstanceIndex ~= RoleLevel2InstanceID(RoleLevel) then
				    	        Success = false
					            break
			    	        end
			            else
				            Success = false
				            break
			            end
		            end

                    if Success then
--                         table.insert(Act18_TeamRoleData, Act18_TeamIndex, {})
                        --传送玩家到活动场景
						local Index = math.random(1, 8)
		                for i=1, 6 do
		                    role.SetRoleScriptData(Role[i], 1, RoleDataType["Act18_TeamKey"], Act18_TeamIndex)
		                    role.RoleGotoNewMap(MapID, InstanceID, Role[i], 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
		                end

		                --活动小队编号加一
                        Act18_TeamIndex = Act18_TeamIndex + 1
                    end
	            else	--玩家已经报名
					local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act18_TeamRoleData, 1)
					if bRet then
						--传送玩家到活动场景
						local Index = math.random(1, 8)
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
					end
	            end
	        end

	        if InstanceIndex == 2 then
	            --如果没有创建活动副本
                if IsCreateAct19Instance == 0 then
					--初始化活动中玩家数据
	                ClearAct18Data(Act19_TeamRoleData, Act19_TeamData)
	                Act19_TeamIndex = 1

                    Act19_Expire = act.GetActScriptData(19, 1, 0)
	                if Act19_Expire == 0 then
	                    --如果是第一届活动，初始化当前届数为20001（方便领取奖励时的判断）
	                    Act19_Expire = 20001
	                end

	                --初始化活动当前届数
	                Act19_Expire = Act19_Expire + 1
	                act.SetActScriptData(19, 1, 0, Act19_Expire)
	                act.SaveActScriptData(19)

                    --创建活动副本
	                map.CreateInstance(2983743695, 2)
                    IsCreateAct19Instance = 1
	            end

                if RoleExpire ~= Act19_Expire then
	                --判断小队成员个数和等级
		            local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
                    local Success = true

                    --获得小队玩家ID
		            local Role = {}
		            Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
					for k in pairs(Role) do
	    	    	    if Role[k] and 4294967295 ~= Role[k] then
							local RoleLevel = role.GetRoleLevel(MapID, InstanceID, Role[k])
			    	        if InstanceIndex ~= RoleLevel2InstanceID(RoleLevel) then
				        	    Success = false
					            break
			      	        end
						else
				            Success = false
				            break
			            end
		            end

                    if Success then
--                        table.insert(Act19_TeamRoleData, Act19_TeamIndex, {})
--                        Act19_TeamRoleData[Act19_TeamIndex] = {}

             			--传送玩家到活动场景
						local Index = math.random(1, 8)
						for i=1, 6 do
						    role.SetRoleScriptData(Role[i], 1, RoleDataType["Act18_TeamKey"], Act19_TeamIndex)
		                    role.RoleGotoNewMap(MapID, InstanceID, Role[i], 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
		                end

		                --活动小队编号加一
                        Act19_TeamIndex = Act19_TeamIndex + 1
                    end
	            else  --玩家已经报名
					local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act19_TeamRoleData, 2)
					if bRet then
						--传送玩家到活动场景
						local Index = math.random(1, 8)
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
					end
	            end
	        end

	        if InstanceIndex == 3 then
	            --如果没有创建活动副本
                if IsCreateAct20Instance == 0 then
					--初始化活动中玩家数据
	                ClearAct18Data(Act20_TeamRoleData, Act20_TeamData)
	                Act20_TeamIndex = 1
                    Act20_Expire = act.GetActScriptData(20, 1, 0)

	                if Act20_Expire == 0 then
	                    --如果是第一届活动，初始化当前届数为30001（方便领取奖励时的判断）
	                     Act20_Expire = 30001
	                end

	                --初始化活动当前届数
	                Act20_Expire = Act20_Expire + 1
	                act.SetActScriptData(20, 1, 0, Act20_Expire)
	                act.SaveActScriptData(20)

                    --创建活动副本
	                map.CreateInstance(2983743695, 3)
                    IsCreateAct20Instance = 1
	            end

                if RoleExpire ~= Act20_Expire then
	     	        --判断小队成员个数和等级
		            local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
                    local Success = true

                    --获得小队玩家ID
		            local Role = {}
		            Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
					for k in pairs(Role) do
	    	    	    if Role[k] and 4294967295 ~= Role[k] then
							local RoleLevel = role.GetRoleLevel(MapID, InstanceID, Role[k])
			    	        if InstanceIndex ~= RoleLevel2InstanceID(RoleLevel) then
				    	        Success = false
					            break
			    	        end
			            else
				            Success = false
				            break
			            end
		            end

                    if Success then
--                        table.insert(Act20_TeamRoleData, Act20_TeamIndex, {})
--                        Act20_TeamRoleData[Act20_TeamIndex] = {}

                        --传送玩家到活动场景
		                local Index = math.random(1, 8)
		                for i=1, 6 do
		                    role.SetRoleScriptData(Role[i], 1, RoleDataType["Act18_TeamKey"], Act20_TeamIndex)
		                    role.RoleGotoNewMap(MapID, InstanceID, Role[i], 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
		                end

		                --活动小队编号加一
                        Act20_TeamIndex = Act20_TeamIndex + 1
                    end
                else   --玩家已经报名
		            local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act20_TeamRoleData, 3)
					if bRet then
						--传送玩家到活动场景
						local Index = math.random(1, 8)
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
					end
                end
	        end

	        if InstanceIndex == 4 then
	            --如果没有创建活动副本
                if IsCreateAct21Instance == 0 then
					--初始化活动中玩家数据
	                ClearAct18Data(Act21_TeamRoleData, Act21_TeamData)
	                Act21_TeamIndex = 1

                    Act21_Expire = act.GetActScriptData(21, 1, 0)
					if Act21_Expire == 1 then
						--如果是第一届活动，初始化当前届数为40001（方便领取奖励时的判断）
	                    Act21_Expire = 40001
	                end

	                --初始化活动当前届数
	                Act21_Expire = Act21_Expire + 1
	                act.SetActScriptData(21, 1, 0, Act21_Expire)
	                act.SaveActScriptData(21)

                    --创建活动副本
	                map.CreateInstance(2983743695, 4)
                    IsCreateAct21Instance = 1
	            end

                if RoleExpire ~= Act21_Expire then
					--判断小队成员个数和等级
		            local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
                    local Success = true

                    --获得小队玩家ID
		            local Role = {}
		            Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
					for k in pairs(Role) do
	    	    	    if Role[k] and 4294967295 ~= Role[k] then
							local RoleLevel = role.GetRoleLevel(MapID, InstanceID, Role[k])
			    	        if InstanceIndex ~= RoleLevel2InstanceID(RoleLevel) then
				    	        Success = false
					            break
			    	        end
			            else
							Success = false
				            break
			            end
		            end

                    if Success then
--                        table.insert(Act21_TeamRoleData, Act21_TeamIndex, {})
--                        Act21_TeamRoleData[Act21_TeamIndex] = {}

                        --传送玩家到活动场景
		                local Index = math.random(1, 8)
		                for i=1, 6 do
		                    role.SetRoleScriptData(Role[i], 1, RoleDataType["Act18_TeamKey"], Act21_TeamIndex)
		                    role.RoleGotoNewMap(MapID, InstanceID, Role[i], 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
		                end

		                --活动小队编号加一
                        Act21_TeamIndex = Act21_TeamIndex + 1
                    end
	            else --玩家已经报名
					local bRet = TestRoleTeam(MapID, InstanceID, RoleID, Act21_TeamRoleData, 4)
					if bRet then
						--传送玩家到活动场景
						local Index = math.random(1, 8)
						role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983743695, m22_BornPos[Index].x, m22_BornPos[Index].y, m22_BornPos[Index].z)
					end
	            end
            end
        end
    end
end

--注册玩家进入活动事件
aux.RegisterCreatureEvent(4500201, 7, "C4500201_OnTalk")]]
