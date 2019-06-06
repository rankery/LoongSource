--刀剑纵横骇八荒：18
--需求1：接取任务后获得BUFF元始无伤[2010201]

--任务接取时
function x018_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2010201, ownerID)
end



--注册
aux.RegisterQuestEvent(18, 0, "x018_OnAccept")

--任务完成时
function q018_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.OpenFramePage(ownerID, 2)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2210)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2210)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)
end

--注册
aux.RegisterQuestEvent(18, 1, "q018_OnComplete")
