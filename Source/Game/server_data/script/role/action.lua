--玩家的某些事件脚本

-------------------------------------------------------------------------------------------------------
--玩家切换地图
function x_RoleEnterMap(mapid, instanceid, roleid)
	unit.AddBuff(mapid, instanceid, roleid, 9999801)    --加一个15秒钟无敌buff
end

--注册函数
aux.RegisterRoleEvent(4, "x_RoleEnterMap")

function Increase_shili(MapID, InstanceID, RoleID, ToTeam, Num, POS) --ToTeam:0-单人加， 1-全队加   Num:增加分值
--POS: "jinjieshili"，副本		"jinjieshili_A",活动		"jinjieshili_Q",每日和随机任务		"jinjieshili_K",击杀非白名玩家

	local shili = role.GetRoleScriptData(RoleID, 1, RoleDataType[POS])
	local e = role.GetConsumptiveStrength(RoleID)

	local d = 0
	if POS == "jinjieshili" then
		d = 12001
	elseif POS == "jinjieshili_A" then
		d = 12002
	elseif POS == "jinjieshili_Q" then
		d = 12003
	elseif POS == "jinjieshili_K" then
		d = 12004
	end

	if ToTeam == 0 then
		--增加进阶实力值和可消耗实力值
		role.SetRoleScriptData(RoleID, 1, RoleDataType[POS],shili+Num)
		role.SetConsumptiveStrength(RoleID, e+Num)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, d)	--您的XX进阶实力值增加了xx！
			msg.AddMsgEvent(MsgID, 9, Num)	--传入增加值
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif ToTeam == 1 then
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			--如果没有小队
		if 4294967295 == TeamID  then
			--增加进阶实力值
			role.SetRoleScriptData(RoleID, 1, RoleDataType[POS],shili+Num)
			role.SetConsumptiveStrength(RoleID, e+Num)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, d)	--您的XX进阶实力值增加了xx！
			msg.AddMsgEvent(MsgID, 9, Num)	--传入增加值
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			--获得小队玩家ID
			local Role = {}
			local Role_MapID = {}
			local Role_InstanceID = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for k in pairs(Role) do
				if 4294967295 ~= Role[k] then
					Role_MapID[k], Role_InstanceID[k] = role.GetRoleMapID(Role[k])
					if Role_MapID[k] == MapID or Role_InstanceID[k] == InstanceID then
						local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, RoleID)
						local x2, y2, z2 = unit.GetPosition(Role_MapID[k], Role_InstanceID[k], Role[k])
						if math.abs(x1-x2)<100 and math.abs(z1-z2)<100 then
							--增加进阶实力值和可消耗实力值
							local shili_team = role.GetRoleScriptData(Role[k], 1, RoleDataType[POS])
							local e_team = role.GetConsumptiveStrength(Role[k])
							role.SetRoleScriptData(Role[k], 1, RoleDataType[POS],shili_team + Num)
							role.SetConsumptiveStrength(Role[k], e_team + Num)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, d)	--您的XX进阶实力值增加了xx！
							msg.AddMsgEvent(MsgID, 9, Num)	--传入增加值
							msg.DispatchRoleMsgEvent(Role[k], MsgID)
						end
					end
				end
			end
		end
	end
end

-------------------------------------------------------------------------------------------------------
--玩家死亡后是否进行死亡惩罚
--[[function r_RoleIsDeadPenalty(MapID, InstanceID, RoleID)
	--pvp固定活动中不进行死亡惩罚
	if MapID == 2983744463 then
		return 0
	end

	if MapID == 2983743695 then
		return 0
	end


	local morality = role.GetRoleAttValue(MapID, InstanceID, RoleID, 74)
	-- 玩家道德值小于等于-30，且背包中有替身符时，免除死亡惩罚
	if morality <= -30 then
		local num = role.GetRoleItemNum(RoleID, 1320011)
		if num > 0 then
			role.RemoveFromRole(MapID, InstanceID, RoleID, 1320011, 1, 300)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 11001)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

			return 0
		end
	end

	return 1
end]]


----------------------------------------------------------------------------------
--[[玩家升级
function r_RoleLevelChange(MapID, InstanceID, RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)

	-- 玩家升到20级，提示钓鱼任务
	if level == 20 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 283)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end


end]]


--aux.RegisterRoleEvent(5, "r_RoleIsDeadPenalty")
--aux.RegisterRoleEvent(6, "r_RoleLevelChange")
