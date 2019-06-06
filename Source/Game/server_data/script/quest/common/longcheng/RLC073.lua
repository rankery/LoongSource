

--任务接取时
function x10073_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.OpenFramePage(ownerID, 6)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2216)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2216)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)
end



--注册
aux.RegisterQuestEvent(10073, 0, "x10073_OnAccept")

