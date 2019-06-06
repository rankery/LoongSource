-- 完成轮回石指引任务自动绑定复活点
function RLC002_OnComplete(MapID, InstanceID, QuestID, RoleID, NpcID)
	role.QuestSetRebornMap(MapID, InstanceID, RoleID, QuestID)
	--任务完成时
	role.OpenFramePage(RoleID, 2)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2213)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2213)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

end


aux.RegisterQuestEvent(10002, 1, "RLC002_OnComplete")
