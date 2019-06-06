

--任务完成时
function q10303_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.OpenFramePage(ownerID, 2)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2219)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2219)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)
end

--注册
aux.RegisterQuestEvent(10303, 1, "q10303_OnComplete")

