

function ChongYang_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	local a = role.GetRoleSilver(MapID, InstanceID,RoleID)

	if a < 60000 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 4113)   --金币数量不足，接取该任务需要消耗6金
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		return 0
	end

	return 1
end

aux.RegisterQuestEvent(20332, 4, "ChongYang_OnCheckAccept")
aux.RegisterQuestEvent(20333, 4, "ChongYang_OnCheckAccept")
aux.RegisterQuestEvent(20334, 4, "ChongYang_OnCheckAccept")
aux.RegisterQuestEvent(20335, 4, "ChongYang_OnCheckAccept")

function ChongYang_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	role.DecRoleSilver(MapID, InstanceID, RoleID, 60000, 101)

end
aux.RegisterQuestEvent(20332, 0, "ChongYang_OnAccept")
aux.RegisterQuestEvent(20333, 0, "ChongYang_OnAccept")
aux.RegisterQuestEvent(20334, 0, "ChongYang_OnAccept")
aux.RegisterQuestEvent(20335, 0, "ChongYang_OnAccept")
