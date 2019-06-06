--对话 黑猫 3010006 对话如果有任务[10005],没有尺子[2610018]，给予尺子[2610018]
function n3010006_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 269)	-- "这只猫的肚子下面似乎有一把尺子，要拿吗？"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 10005) == true then
			if role.GetRoleItemNum(RoleID, 2610018) == 0 then
				role.AddRoleItem(MapID, InstanceID, RoleID, 2610018, 1, -1, 8, 420)
				TalkIndex = -1
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 270)	-- "你已经有一把尺子了"

				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1

			end
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 271)	-- "你被这只猫狠狠的抓了一下，好疼啊…"

			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			TalkIndex = -1

		end
	end

end
aux.RegisterCreatureEvent(3010006, 7, "n3010006_OnTalk")
