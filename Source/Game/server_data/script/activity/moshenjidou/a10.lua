
function a10_PlayerEnter(MapID, InstanceID, RoleID)

	unit.AddBuff(MapID, InstanceID, RoleID, 1121301, RoleID)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"], role.GetRoleScriptData(RoleID, 1, RoleDataType["Jidou_Times"])+1)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 71, 3708)			--魔神激斗已开始，请选择英雄
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

	if Dota_GetPos(InstanceID, RoleID)%2 == 0 then
		unit.SetState(MapID, InstanceID, RoleID, 14)
	else
		unit.SetState(MapID, InstanceID, RoleID, 13)
	end

end

--注册进入活动地图事件
aux.RegisterMapEvent("a10", 2, "a10_PlayerEnter")

function a10_OnRoleDie(MapID, InstanceID, RoleID, KillerID)

	local ReviveTime={}
	ReviveTime[0]=5
	ReviveTime[1]=5
	ReviveTime[2]=6
	ReviveTime[3]=8
	ReviveTime[4]=10
	ReviveTime[5]=12
	ReviveTime[6]=15
	ReviveTime[7]=20
	ReviveTime[8]=25
	ReviveTime[9]=30
	ReviveTime[10]=36

	role.DotaTryRevive(MapID, InstanceID, RoleID, ReviveTime[Dota_GetValue(InstanceID, RoleID, 2)])

end

--死亡事件
aux.RegisterMapEvent("a10",  5, "a10_OnRoleDie")

function a10_On_Revive(MapID, InstanceID, RoleID, ReviveType, ReviveHP, ReviveMP, fx, fy, fz, RebornMapID)

	if ReviveType == 7 then

		if Dota_GetPos(InstanceID, RoleID)%2 == 0 then
			return ReviveHP, ReviveMP, 84, 0, 92, RebornMapID
		else
			return ReviveHP, ReviveMP, 426, 0, 429, RebornMapID
		end

	end
	return ReviveHP, ReviveMP, fx, fy, fz, RebornMapID

end

--复活事件
aux.RegisterMapEvent("a10", 11, "a10_On_Revive")

--找到最合适的副本实例
function a10_GetOnePerfectMap(MapID, InstanceID, RoleID)

	return Jidou_instanceindex

end

aux.RegisterMapEvent("a10",  14, "a10_GetOnePerfectMap")

--玩家在活动中掉线后上线
function a10_CanTakeOverWhenOnline(MapID, RoleID)
	--修正玩家的上线地图和坐标todo
	return 3017298383, 880, 10000, 1987
end

aux.RegisterMapEvent("a10",  12, "a10_CanTakeOverWhenOnline")

-------------------------------------------------------------------------------------------------------
--找到地图的出口和坐标
function a10_GetExporMapAndCoord(MapID, InstanceID, RoleID)
	return 1, 3017298383, 880, 10000, 1987
end

aux.RegisterMapEvent("a10",  13, "a10_GetExporMapAndCoord")

--检测玩家是否能够进入活动场景
function a10_CanEnter(MapID, InstanceID, RoleID)

	return 1

end

aux.RegisterMapEvent("a10",  15, "a10_CanEnter")

--组队相关检测

function a10_CanInviteJoinTeam(MapID, InstanceID)

	return false

end

function a10_CanLeaveTeam(MapID, InstanceID)

	return false

end

function a10_CanChangeLeader(MapID, InstanceID)

	return false

end

function a10_CanKickMember(MapID, InstanceID)

	return false

end

aux.RegisterMapEvent("a10",  8, "a10_CanInviteJoinTeam")
aux.RegisterMapEvent("a10",  9, "a10_CanLeaveTeam")
aux.RegisterMapEvent("a10",  10, "a10_CanChangeLeader")
aux.RegisterMapEvent("a10",  17, "a10_CanKickMember")

function c4900901_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)

	map.OpenCloseDoor(MapID, InstanceID, 274, 1)
	map.OpenCloseDoor(MapID, InstanceID, 275, 1)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3726)	--大门已打开
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(4900901, 13, "c4900901_OnDisappear")
