

--离开震珠界
function ZZJ_xinshi_On_Talk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4221) -- "我可以将你带出震珠界"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 4222)	-- "我要离开震珠界"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

	if TalkIndex == 4 then
			role.RoleGotoNewMap(MapID, InstanceID,RoleID,3017299407,1786,15007,2732)
	end

end

aux. RegisterCreatureEvent(4902007, 7, "ZZJ_xinshi_On_Talk")




































