--15430与3070737对话，失去冰魂珠（2615215）10个，召唤万修的二弟子1009217(961,12571,351)

function n3070737_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
   if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 20, 520)	-- "你想用冰魂珠引出万修的二弟子吗？"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
	    msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
	    msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        local i = role.GetRoleItemNum(RoleID, 2615215)
        if i < 10 then
            local MsgID = msg.BeginMsgEvent()
            msg.AddMsgEvent(MsgID, 20, 521)	-- "需要10个冰魂珠才能引出万修的二弟子。"
            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
        else
            role.RemoveFromRole(MapID, InstanceID, RoleID, 2615215, 10, 420)
	    map.MapCreateCreature(MapID, InstanceID, 1009217, 961, 12571, 351)
        end
    end
end

aux.RegisterCreatureEvent(3070737, 7, "n3070737_OnTalk")
