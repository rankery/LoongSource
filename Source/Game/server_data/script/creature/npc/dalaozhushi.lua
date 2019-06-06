--对话 大牢主事3010248，如果道德高于0，则被传送出牢狱
function n3010248_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	11002)		--“是否传送出去”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
	    local i=role.GetRoleAttValue(MapID, InstanceID, RoleID, 74)
	    if i >= 0 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298127, 2687, 6296, 2655)
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	11003)		--“道德值不足，不放你出去”
		    msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		    msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
		    msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end

end
aux.RegisterCreatureEvent(3010248, 7, "n3010248_OnTalk")
