function Marriage_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local Married = role.IsRoleMarried(RoleID)
		if(Married == 1) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 517)      --您已经结婚，不能接取该任务
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	return 1
end

aux.RegisterQuestEvent(20154, 4, "Marriage_OnCheckAccept")
aux.RegisterQuestEvent(20155, 4, "Marriage_OnCheckAccept")
aux.RegisterQuestEvent(20156, 4, "Marriage_OnCheckAccept")
aux.RegisterQuestEvent(20157, 4, "Marriage_OnCheckAccept")
