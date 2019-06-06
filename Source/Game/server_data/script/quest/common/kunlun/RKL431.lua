--15431与3070738对话，失去寒魂珠（2615216）10个，召唤万修的大弟子1009215(1070,12740,258)

function n3070738_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
   if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 20, 522)	-- "你想用寒魂珠引出万修的大弟子吗？"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
	    msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
	    msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        local i = role.GetRoleItemNum(RoleID, 2615216)
        if i < 10 then
            local MsgID = msg.BeginMsgEvent()
            msg.AddMsgEvent(MsgID, 20, 523)	-- "需要10个寒魂珠才能引出万修的大弟子。"
            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
        else
            role.RemoveFromRole(MapID, InstanceID, RoleID, 2615216, 10, 420)
	    map.MapCreateCreature(MapID, InstanceID, 1009215, 1070, 12740, 258)
        end
    end
end

aux.RegisterCreatureEvent(3070738, 7, "n3070738_OnTalk")
