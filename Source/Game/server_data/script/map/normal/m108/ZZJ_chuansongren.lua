

--[[
?	进入震珠界修炼场：
?	玩家点击震珠界传送人后，弹出对话界面：
?	对话内容：
震珠界藏于昆仑山山底属于十大洞天之一，是修行练功的极佳场所，本为玉虚派所属。可叹造化弄人，十年之间，玉虚一派惨遭魔族屠戮，玉虚掌门清虚真人损元仙逝，玉虚神女受魔王蛊惑，已然成魔，弑魔之人，恐再无来者。人世间逢此末世浩劫，可悲，可悲啊……
（震珠界中无法随意更换装备）
?	选项一：请送我去震珠界
?	点击后进行判断，需要消耗玩家一枚震珠界通行令，没有道具的话，弹框提示缺少震珠界通行令；
]]--


function ZZJ_chuansongren_On_Talk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 4217) -- "震珠界藏于昆仑山山底属于十大洞天之一，是修行练功的极佳场所，本为玉虚派所属。可叹造化弄人，十年之间，玉虚一派惨遭魔族屠戮，玉虚掌门清虚真人损元仙逝，玉虚神女受魔王蛊惑，已然成魔，弑魔之人，恐再无来者。人世间逢此末世浩劫，可悲，可悲啊……\n（震珠界中无法随意更换装备）"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 4218)	-- "请送我去震珠界（需要消耗一枚震珠界通行令）"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

	if TalkIndex == 4 then
		local Item = role.GetRoleItemNum(RoleID,3200488)
		local level = role.GetRoleLevel(MapID, InstanceID, RoleID) --玩家等级
		if level < 100 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 82)   --等级不足，无法进入
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		else
			if Item <= 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 4219)   --缺少道具
				msg.AddMsgEvent(MsgID, 4, 3200488) --道具名称
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			else
				role.RoleGotoNewMap(MapID, InstanceID,RoleID,2291723889,526,160,554)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3200488, 1, 420)
			end
		end
	end

end


aux. RegisterCreatureEvent(4902003, 7, "ZZJ_chuansongren_On_Talk")
































