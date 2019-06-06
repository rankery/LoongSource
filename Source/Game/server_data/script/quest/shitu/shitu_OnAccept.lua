function shitu_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断玩家是否有师父
	local MasterID = role.GetMasterID(MapID, InstanceID, RoleID)
	if MasterID == 4294967295 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2305)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)

	--判断玩家是否已与师父组队
	if TeamID==4294967295 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2301)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

	local IsMasterInTeam =0

	for i=1, 6 do
	    if Role[i]== MasterID then
	        IsMasterInTeam=1
	    end
	end

	if IsMasterInTeam==0 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2301)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断师父是否在身边
	local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, MasterID)
	local x2, y2, z2 = unit.GetPosition(MapID, InstanceID, RoleID)

	if x1== nil then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2302)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	if x1-x2>30 or x2-x1>30 or z1-z2>30 or z2-z1>30 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2302)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1

end

--注册
aux.RegisterQuestEvent(8002, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8003, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8004, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8005, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8006, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8007, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8008, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8009, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8010, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8011, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8012, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8013, 4, "shitu_OnCheckAccept")
aux.RegisterQuestEvent(8014, 4, "shitu_OnCheckAccept")

function shitu_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local MasterID = role.GetMasterID(MapID, InstanceID, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2310)
	msg.DispatchRoleMsgEvent(MasterID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	if QuestID==8002 then
		msg.AddMsgEvent(MsgID, 1, 2311)
	elseif QuestID==8003 or QuestID==8004 then
		msg.AddMsgEvent(MsgID, 1, 2312)
	elseif QuestID==8005 or QuestID==8006 or QuestID==8007 or QuestID==8008 or QuestID==8009 then
		msg.AddMsgEvent(MsgID, 1, 2313)
	elseif QuestID==8010 or QuestID==8011 or QuestID==8012 then
		msg.AddMsgEvent(MsgID, 1, 2314)
	elseif QuestID==8013 or QuestID==8014 then
		msg.AddMsgEvent(MsgID, 1, 2315)
	end

	msg.DispatchRoleMsgEvent(MasterID, MsgID)

end

aux.RegisterQuestEvent(8002, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8003, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8004, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8005, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8006, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8007, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8008, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8009, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8010, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8011, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8012, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8013, 0, "shitu_OnAccept")
aux.RegisterQuestEvent(8014, 0, "shitu_OnAccept")
