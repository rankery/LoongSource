--15429与3070736对话，失去雪魂珠（2615214）10个，召唤万修的三弟子1009218(1234,10730,414)

function n3070736_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
   if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 20, 518)	-- "你想用雪魂珠引出万修的三弟子吗？"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
	    msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
	    msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        local i = role.GetRoleItemNum(RoleID, 2615214)
        if i < 10 then
            local MsgID = msg.BeginMsgEvent()
            msg.AddMsgEvent(MsgID, 20, 519)	-- "需要10个雪魂珠才能引出万修的三弟子。"
            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
        else
            role.RemoveFromRole(MapID, InstanceID, RoleID, 2615214, 10, 420)
	    map.MapCreateCreature(MapID, InstanceID, 1009218, 1234, 10730, 414)
        end
    end
end

aux.RegisterCreatureEvent(3070736, 7, "n3070736_OnTalk")
