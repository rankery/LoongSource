--小龙女（玩家回归奖励领取人）脚本

function XiaoLongNv(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local AwayTime = role.GetPlayerAccountOfflineTime(RoleID)
	local CanReceive = role.CanRecAccountRegressionReward(RoleID)
	local FreeSize = role.GetBagFreeSize(RoleID)

	if TalkIndex == -1 then

		if CanReceive ~= true then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2201)
				--“迎老友回归，获极品奖励”
				--“老玩家回归奖励规则调整。当玩家超过7天未登录游戏，则当其回归
				--“《龙》的世界时，不仅自己可获得回归礼品，同时可选则他的任何一
				--“名朋友送出回归大礼。朋友可获得的礼品内容根据回归玩家的等级决定：
				--“1-50级玩家回归：功倍符×1
				--“50-80级玩家回归：功倍符×1，神器材料或饺子
				--”80-100级玩家回归：功倍符×1，神器材料或饺子，点金石
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if AwayTime < 259200 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2201)		--“”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif AwayTime >= 259200 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2202)		--“小女子便是人称江湖新秀的小龙女了。听闻阁下乃江湖前辈，身怀绝技却隐居不出，
				--心中仰慕已久！今日听闻大侠重出江湖，特备下厚礼一份，还望侠士不要推辞哦～
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	2203)		--“领取重出江湖奖励”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end

	elseif TalkIndex == 4 then

		if CanReceive == true and FreeSize>=1 then
			if AwayTime >= 259200 and AwayTime<604800 then
				role.AddRoleItem(MapID, InstanceID, RoleID, 1330635, 1, -1, 8, 420)
				role.SetRecAccountRegressionRewardFlag(RoleID, 1)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2204)		--“礼品已经放到侠士的行囊了，使用后即可获取奖励！”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif AwayTime >= 604800 then
				role.AddRoleItem(MapID, InstanceID, RoleID, 1330634, 1, -1, 8, 420)
				role.SetRecAccountRegressionRewardFlag(RoleID, 1)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2204)		--“礼品已经放到侠士的行囊了，使用后输入好友名称则您和您的好友都可获得相应奖励！”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

				local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, RoleID)
				local temp = rolelevel
				if temp<=80 then temp=80 end

				role.AddRoleExp(MapID, InstanceID, RoleID, LevelUpNeedExp*125/math.pow(temp-70, 2))

			end
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2205)		--“您的行囊已满，或者您已领取过重出江湖礼品。请为行囊留出一格空格再来领取～”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(3610201, 7, "XiaoLongNv")
