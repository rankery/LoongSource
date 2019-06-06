function x20341_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local ManInTeam = 0
	local AddSex = 0    --如果值是10则满足一男一女
	local RoleID1 =0
	local RoleID2 =0
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local sex = role.GetRoleSex(Role[i])
		    if sex == 1 then
		        AddSex = AddSex + 1 --是男性的话就加3
		    elseif sex == 0 then
		        AddSex = AddSex + 7 --是女性的话就加7
		    end
				ManInTeam = ManInTeam + 1
	    end
	end
	local FriendShip1 = role.GetFriendShipValue(RoleID1, RoleID2)
	local FriendShip2 = role.GetFriendShipValue(RoleID2, RoleID1)
	if ManInTeam == 2 and AddSex == 8 then
	    return 1
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26,12611) --只有在两个互为好友的异性玩家组队时才能接取该任务
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
end

function x20341_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 142)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1
end

function x20336_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	local IsHave = role.IsRoleHaveBuff(MapID, InstanceID,RoleID,99996)
	if IsHave == true then
		unit.CancelBuff(MapID, InstanceID,RoleID,9999601)
	end
	unit.AddBuff(MapID,InstanceID,RoleID,1131701,RoleID)
end
aux.RegisterQuestEvent(20336, 1, "x20336_OnComplete")
aux.RegisterQuestEvent(20341, 4, "x20341_OnCheckAccept")
aux.RegisterQuestEvent(20341, 5, "x20341_OnCheckComplete")
