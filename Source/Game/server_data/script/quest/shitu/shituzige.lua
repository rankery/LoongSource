function shituzige_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	role.RoleBecomeMaster(MapID, InstanceID, RoleID)

	--播放服务器广播
	local MsgID = msg.BeginMsgEvent();
	msg.AddMsgEvent(MsgID,102,100027);
	msg.AddMsgEvent(MsgID,2,RoleID);
	msg.DispatchBroadcast(MsgID,-1,-1,-1);
end

--注册
aux.RegisterQuestEvent(8001, 1, "shituzige_OnComplete")
