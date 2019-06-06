
function a11_PlayerEnter(MapID, InstanceID, RoleID)
	local IsHave = role.IsRoleHaveBuff(MapID, InstanceID,RoleID,99996)
	local IsHaveSkill = role.IsLearnedSkill(MapID,InstanceID,RoleID,90019)
	role.AddSkill(RoleID, 9001901)--给玩家增加一个针对旗帜调查的技能
	if IsHave == true then
		unit.CancelBuff(MapID, InstanceID,RoleID,9999601)
	end
	unit.AddBuff(MapID, InstanceID, RoleID, 1121301, RoleID)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"], role.GetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"])+1)
	SendNormalInformation(MapID, InstanceID, RoleID) --同步信息

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 71, 3708)			--魔神激斗已开始，请选择英雄
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

	if Jidou.RoomInfo[InstanceID]~= nil and Jidou.RoomInfo[InstanceID].Role[RoleID]~= nil then
	    if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Red" then
		    role.SetForceInDota(MapID, InstanceID, RoleID, 0) --红色
		else
			role.SetForceInDota(MapID, InstanceID, RoleID, 1) --蓝色
		end
	end

end

--注册进入活动地图事件
aux.RegisterMapEvent("a11", 2, "a11_PlayerEnter")


--进入触发器获得BUFF
function a11_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
    if Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID] ~= nil and  Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreID ~= nil and  Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreID ~= 0 then
	    unit.AddBuff(MapID, InstanceID, RoleID, Jidou.CreBuff[Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreTypeID], RoleID)
        map.MapDeleteCreature(MapID, InstanceID, Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreID)
		Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreTypeID = 0
		Jidou.RoomInfo[InstanceID].BuffTrigger[ObjID].CreID = 0
		Jidou.RoomInfo[InstanceID].Role[RoleID].Level = Jidou.RoomInfo[InstanceID].Role[RoleID].Level + 1
		if Jidou.RoomInfo[InstanceID].Role[RoleID].Level <= 40 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 1)
			msg.AddMsgEvent(MsgID, 1, 12901)           --力量值增加为X/40点！每提升10点力量值将会提升变身后的技能等级。
			msg.AddMsgEvent(MsgID, 9, Jidou.RoomInfo[InstanceID].Role[RoleID].Level)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
        if Jidou.RoomInfo[InstanceID].Role[RoleID].Level ~= 0 and Jidou.RoomInfo[InstanceID].Role[RoleID].Level % 10 == 0 then
		    local k = Jidou.RoomInfo[InstanceID].Role[RoleID].Level/10 + 1
			if k <= 0 then
			    k = 2
			elseif k > 5 then
			    k = 5
			end
            for i=11231, 11238 do
				if role.IsRoleHaveBuff(MapID, InstanceID, RoleID, i) then
				    unit.AddBuff(MapID, InstanceID, RoleID, i*100+k, RoleID)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 12902)           --变身能力提升至XX级！
					msg.AddMsgEvent(MsgID, 9, k)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end
	end
end
aux.RegisterMapEvent("a11",6,"a11_OnEnterTrigger")


function a11_OnRoleDie(MapID, InstanceID, RoleID, KillerID)

	local ReviveTime={}
	ReviveTime[0]=5
	ReviveTime[1]=5
	ReviveTime[2]=8
	ReviveTime[3]=10
	ReviveTime[4]=12
	ReviveTime[5]=15
	ReviveTime[6]=15
	ReviveTime[7]=20
	ReviveTime[8]=20
	ReviveTime[9]=20
	ReviveTime[10]=20

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	role.DotaTryRevive(MapID, InstanceID, RoleID, ReviveTime[math.floor(Jidou.RoomInfo[InstanceID].Role[RoleID].Level/10)+1])

	Jidou.RoomInfo[InstanceID].Role[RoleID].Die = Jidou.RoomInfo[InstanceID].Role[RoleID].Die + 1
	SendNormalInformation(MapID, InstanceID, RoleID)

	if role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11401) then --蓝旗BUFF
	    unit.CancelBuff(MapID, InstanceID,RoleID, 1140101)
		Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902112, x, y, z)
		Jidou.RoomInfo[InstanceID].BlueFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].BlueFlag.Role = 0
		Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
	elseif role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11402) then  --红旗BUFF
	    unit.CancelBuff(MapID, InstanceID,RoleID, 1140201)
		Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113, x, y, z)
		Jidou.RoomInfo[InstanceID].RedFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].RedFlag.Role = 0
		Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
	end

	if unit.IsPlayer(KillerID) then
		local Target_MapID, Target_InstanceID = role.GetRoleMapID(KillerID)
		if Target_MapID ~= nil and Target_MapID == 2108961076 then
			Jidou.RoomInfo[InstanceID].Role[KillerID].Kill = Jidou.RoomInfo[InstanceID].Role[KillerID].Kill + 1
			SendNormalInformation(MapID, InstanceID, KillerID)
		end
	end
end

--死亡事件
aux.RegisterMapEvent("a11",  5, "a11_OnRoleDie")

function a11_LeaveMap(MapID, InstanceID, RoleID)

    if InstanceID ~= nil and InstanceID ~= 4294967295 then
		unit.UnsetState(MapID, InstanceID, RoleID, 13)
		unit.UnsetState(MapID, InstanceID, RoleID, 14)
    end

    if InstanceID ~= nil then
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		if TeamID ~= nil and TeamID ~= 4294967295 then
			role.LetRoleLeaveTeam(TeamID, RoleID)
		end
	end

	if InstanceID ~= nil and role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11401) then
	    unit.CancelBuff(MapID, InstanceID,RoleID, 1140101)
		local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
		Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902112, x, y, z)
		Jidou.RoomInfo[InstanceID].BlueFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].BlueFlag.Role = 0
		Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
	elseif InstanceID ~= nil and role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11402) then
	    unit.CancelBuff(MapID, InstanceID,RoleID, 1140201)
		local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
		Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113, x, y, z)
		Jidou.RoomInfo[InstanceID].RedFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].RedFlag.Role = 0
		Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
	end

	if  Jidou.RoomInfo[InstanceID] ~= nil and Jidou.RoomInfo[InstanceID].IsOver ~= 1 then
		Jidou.RoomInfo[InstanceID].Role[RoleID].GetFlag = 0
		Jidou.RoomInfo[InstanceID].Role[RoleID].MissFlag = 0
		Jidou.RoomInfo[InstanceID].Role[RoleID].Kill = 0
		Jidou.RoomInfo[InstanceID].Role[RoleID].Die = 0
		Jidou.RoomInfo[InstanceID].Role[RoleID].Leave = 1
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12903)			--XXX已经退出了比赛
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		MSJD_CheckIsOver(MapID, InstanceID)
	elseif Jidou.RoomInfo[InstanceID] ~= nil then
	    Jidou.RoomInfo[InstanceID].Role[RoleID].Leave = 1
	end
end
--离开地图
aux.RegisterMapEvent("a11", 3, "a11_LeaveMap")


function a11_On_Revive(MapID, InstanceID, RoleID, ReviveType, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID)

	--if ReviveType == 7 then
	if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Red" then
		unit.AddBuff(MapID, InstanceID, RoleID, 1140701, RoleID) --无敌，可对死亡目标添加
		unit.AddBuff(MapID, InstanceID, RoleID, 1140801, RoleID)  --可对死亡目标添加的越死越硬可叠加的BUFF，ID未有。建议每3秒回1%血，同时增加一定数量护甲。
		return ReviveHP, ReviveMP, 442,5502,105, 2108961076

	else
		unit.AddBuff(MapID, InstanceID, RoleID, 1140701, RoleID) --无敌，可对死亡目标添加
		unit.AddBuff(MapID, InstanceID, RoleID, 1140801, RoleID)  --可对死亡目标添加的越死越硬可叠加的BUFF，ID未有。建议每3秒回1%血，同时增加一定数量护甲。
		return ReviveHP, ReviveMP, 81,5555,426, 2108961076
	end


	--end
	--return ReviveHP, ReviveMP, fx, fy, fz, RebornMapID

end

--复活事件
aux.RegisterMapEvent("a11", 11, "a11_On_Revive")

-- 竞技场的敌我关系判断 -- 除同色以外全是敌人
function a11_FriendEnemy(MapID, InstanceID, SrcID, TargetID)

	-- 都是玩家
	if unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then
        if Jidou.RoomInfo[InstanceID] ~= nil and Jidou.RoomInfo[InstanceID].Role[SrcID] ~= nil and Jidou.RoomInfo[InstanceID].Role[TargetID] ~= nil then
			if Jidou.RoomInfo[InstanceID].Role[SrcID].Side == Jidou.RoomInfo[InstanceID].Role[TargetID].Side then
		        return true, false, false, true
			else
			    return false, true, false, true
			end
		end
    elseif unit.IsCreature(SrcID) and unit.IsPlayer(TargetID)then
	    local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, SrcID)
	    if cre.GetCreatureScriptData(MapID, InstanceID, SrcID, 1, 0) == 1 or TypeID == 4902117 then
		    if Jidou.RoomInfo[InstanceID].Role[TargetID].Side == "Red" then        --第0个位置是1的怪物和红方玩家是友方，和蓝方是敌方
			    return true, false, false, true
			else
			    return false, true, false, true
			end
		end
		if cre.GetCreatureScriptData(MapID, InstanceID, SrcID, 1, 0) == 2 or TypeID == 4902116 then
		    if Jidou.RoomInfo[InstanceID].Role[TargetID].Side == "Red" then        --第0个位置是2的怪物和蓝方玩家是友方，和红方是敌方
			    return false, true, false, true
			else
			    return true, false, false, true
			end
		end
	elseif unit.IsPlayer(SrcID) and unit.IsCreature(TargetID) then
	    local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, TargetID)
	    if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 2 or TypeID == 4902116 then
		    if Jidou.RoomInfo[InstanceID].Role[SrcID].Side == "Red" then        --第0个位置是2的怪物和蓝方玩家是友方，和红方是敌方
			    return false, true, false, true
			else
			    return true, false, false, true
			end
		end
		if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 1 or TypeID == 4902117 then
		    if Jidou.RoomInfo[InstanceID].Role[SrcID].Side == "Red" then        --第0个位置是1的怪物和红方玩家是友方，和蓝方是敌方
			    return true, false, false, true
			else
			    return false, true, false, true
			end
		end
	else
	    return false, false, false, false
	end

end

aux.RegisterMapEvent("a11", 16, "a11_FriendEnemy")

--找到最合适的副本实例
function a11_GetOnePerfectMap(MapID, InstanceID, RoleID)

	for k,v in pairs(Jidou.RoomInfo) do
	    if v.Role[RoleID] ~= nil and v.Role[RoleID].Leave ~= 1 then
		    return k
		end
	end
	return 0

end

aux.RegisterMapEvent("a11",  14, "a11_GetOnePerfectMap")

--玩家在活动中掉线后上线
function a11_CanTakeOverWhenOnline(MapID, RoleID)
	--修正玩家的上线地图和坐标todo
	return 3017298383, 880, 10000, 1987
end

aux.RegisterMapEvent("a11",  12, "a11_CanTakeOverWhenOnline")

-------------------------------------------------------------------------------------------------------
--找到地图的出口和坐标
function a11_GetExporMapAndCoord(MapID, InstanceID, RoleID)
	return 1, 3017298383, 880, 10000, 1987
end

aux.RegisterMapEvent("a11",  13, "a11_GetExporMapAndCoord")

--检测玩家是否能够进入活动场景
function a11_CanEnter(MapID, InstanceID, RoleID)

	return 1

end

aux.RegisterMapEvent("a11",  15, "a11_CanEnter")

--组队相关检测

function a11_CanInviteJoinTeam(MapID, InstanceID)

	return false

end

function a11_CanLeaveTeam(MapID, InstanceID)

	return false

end

function a11_CanChangeLeader(MapID, InstanceID)

	return false

end

function a11_CanKickMember(MapID, InstanceID)

	return false

end

aux.RegisterMapEvent("a11",  8, "a11_CanInviteJoinTeam")
aux.RegisterMapEvent("a11",  9, "a11_CanLeaveTeam")
aux.RegisterMapEvent("a11",  10, "a11_CanChangeLeader")
aux.RegisterMapEvent("a11",  17, "a11_CanKickMember")

function MSJD_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
    if Jidou.RoomInfo[InstanceID] ~= nil then
		if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)  ~= 0 and cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) ~= 40 then        --归位的计时器
			if Jidou.RoomInfo[InstanceID].IsOver ~= 1 then
				if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == Jidou.RoomInfo[InstanceID].RedFlag.FlagID then
					map.MapDeleteCreature(MapID, InstanceID, Jidou.RoomInfo[InstanceID].RedFlag.FlagID)
					Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902112,425,5483,109)--红方红旗
					Jidou.RoomInfo[InstanceID].RedFlag.State = "home"
					Jidou.RoomInfo[InstanceID].RedFlag.Role = 0
					Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
					map.MapCreateCreature(2108961076, Index, 4902114,64,5555,440)--蓝方空旗
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 12904)			--红方的旗帜已经回到了蓝方营地当中
					msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

				elseif cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == Jidou.RoomInfo[InstanceID].BlueFlag.FlagID then
					map.MapDeleteCreature(MapID, InstanceID, Jidou.RoomInfo[InstanceID].BlueFlag.FlagID)
					Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113,97,5556,406)--蓝方蓝旗
					Jidou.RoomInfo[InstanceID].BlueFlag.State = "home"
					Jidou.RoomInfo[InstanceID].BlueFlag.Role = 0
					Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
					map.MapCreateCreature(MapID, InstanceID,4902133 ,464,5517,96)  --红方空旗
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 12905)			--蓝方的旗帜已经回到了蓝方营地当中
					msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
				end
			end
		elseif Jidou.RoomInfo[InstanceID].Timer30  == 40 and cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 40 then        --传送的计时器
			map.DeleteInstance(MapID, InstanceID)
			Jidou.RoomInfo[InstanceID] = nil
		elseif Jidou.RoomInfo[InstanceID].IsOver ~= 1 then
			map.MapCreateCreature(2108961076, InstanceID, 4902134, 1, 1, 1)           --刷新一个30秒的透明计时器，该怪物未提需求
			Jidou.RoomInfo[InstanceID].Timer30 = Jidou.RoomInfo[InstanceID].Timer30 + 1
			MSJD_CheckIsOver(MapID, InstanceID)

			if Jidou.RoomInfo[InstanceID].Timer30 <= 39 and Jidou.RoomInfo[InstanceID].IsOver ~= 1 then

				--为每个没有怪物的触发器刷一个怪物
				for k,v in pairs(Jidou.RoomInfo[InstanceID].BuffTrigger) do
					if v.CreID == 0 then
						v.CreTypeID = math.random(4) + 4902118
						v.CreID = map.MapCreateColCreature(MapID, InstanceID, v.CreTypeID, v.x, v.y, v.z, 1)
					end
				end

				if Jidou.RoomInfo[InstanceID].BlueFlag.State == "outside" and Jidou.RoomInfo[InstanceID].BlueFlag.Role == 0 then
					Jidou.RoomInfo[InstanceID].BlueFlag.Time = Jidou.RoomInfo[InstanceID].BlueFlag.Time + 1
					if Jidou.RoomInfo[InstanceID].BlueFlag.Time >= 3 then
						map.MapDeleteCreature(MapID, InstanceID, Jidou.RoomInfo[InstanceID].BlueFlag.FlagID)
						Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113,97,5556,406)--蓝方蓝旗
						Jidou.RoomInfo[InstanceID].BlueFlag.State = "home"
						Jidou.RoomInfo[InstanceID].BlueFlag.Role = 0
						Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12906)			--蓝方的旗帜已经回到了蓝方营地当中
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
					end
				elseif Jidou.RoomInfo[InstanceID].RedFlag.State == "outside" and Jidou.RoomInfo[InstanceID].RedFlag.Role == 0 then
					Jidou.RoomInfo[InstanceID].RedFlag.Time = Jidou.RoomInfo[InstanceID].RedFlag.Time + 1
					if Jidou.RoomInfo[InstanceID].RedFlag.Time >= 3 then
						map.MapDeleteCreature(MapID, InstanceID, Jidou.RoomInfo[InstanceID].RedFlag.FlagID)
						Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113,425,5483,109)--红方红旗
						Jidou.RoomInfo[InstanceID].RedFlag.State = "home"
						Jidou.RoomInfo[InstanceID].RedFlag.Role = 0
						Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 12907)			--红方的旗帜已经回到了蓝方营地当中
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
					end
				end
			end
		end
	end
end

aux.RegisterCreatureEvent(4902134, 13, "MSJD_OnDisappear")--计时器消失事件注册，少计时器
