--神迹争夺奖励人脚本

function n_ShenJiJiangLi(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local ShenJiPoint = role.GodMiraclePoints(MapID, InstanceID, RoleID)
	local FreeSize = role.GetBagFreeSize(RoleID)
	local Last_Recv_D = role.GetRoleScriptData(RoleID, 1, RoleDataType["Last_Recv_D"])

	if TalkIndex == -1 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2239)
		msg.AddMsgEvent(MsgID, 9, ShenJiPoint)
		--战神：
		--    您可以使用在神迹争夺战中所获得的神迹积分在此处兑换奖励，
		--各项兑换需要的积分及说明如下：
		--    仙界天劫令：5点神迹积分
		--    经验值：30点神迹积分（每天只可兑换一次）
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	2230)		--兑换仙界天劫令×1
		msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	2231)		--兑换仙界天劫令×5
		msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	2232)		--兑换经验值
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		if ShenJiPoint<5 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2233)	--您的神迹积分不足，无法兑换！
			msg.AddMsgEvent(MsgID, 24,	TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if FreeSize>=1 then
				role.ModifyMiraclePoints(MapID, InstanceID, RoleID, -5)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3300103, 1, -1, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2234)	--您已用5点神迹积分兑换一枚仙界天劫令！
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2235)	--您行囊空间不足，请清理行囊空间后再来兑换！
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end

	elseif TalkIndex == 5 then
		if ShenJiPoint<25 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2233)	--您的神迹积分不足，无法兑换！
			msg.AddMsgEvent(MsgID, 24,	TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if FreeSize>=1 then
				role.ModifyMiraclePoints(MapID, InstanceID, RoleID, -25)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3300103, 5, -1, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2236)	--您已用25点神迹积分兑换五枚仙界天劫令！
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2235)	--您行囊空间不足，请清理行囊空间后再来兑换！
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end

	elseif TalkIndex == 6 then
		if ShenJiPoint<30 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2233)	--您的神迹积分不足，无法兑换！
			msg.AddMsgEvent(MsgID, 24,	TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local Cur_D = tonumber(os.date("%j"))
			if Cur_D ~= Last_Recv_D then
				local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, RoleID)
				local addexp = LevelUpNeedExp * 150 / (rolelevel+10)^2 * 5
				role.ModifyMiraclePoints(MapID, InstanceID, RoleID, -30)
				role.AddRoleExp(MapID, InstanceID, RoleID, addexp)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["Last_Recv_D"], Cur_D)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2237)	--您已用30点神迹积分兑换XX点经验值（不同等级兑换获得的经验值不同）！
				msg.AddMsgEvent(MsgID, 9, addexp)
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2238)	--您今天已领取过经验值奖励，经验值奖励每天只可领取一次！
				msg.AddMsgEvent(MsgID, 24,	TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end

	end
end

-- 注册
aux.RegisterCreatureEvent(4900524, 7, "n_ShenJiJiangLi")
