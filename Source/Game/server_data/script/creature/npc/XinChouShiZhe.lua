--薪酬使者脚本

function XinChouShiZhe(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Item1Num = role.GetRoleItemNum(RoleID, 3303501) --传道
	local Item2Num = role.GetRoleItemNum(RoleID, 3303502) --狂热
	local Salary_Times = role.GetRoleScriptData(RoleID, 1, RoleDataType["Salary_Times"])
	local Last_Salary_M = role.GetRoleScriptData(RoleID, 1, RoleDataType["Last_Salary_M"])

	local current_M = os.date("%m")
	current_M = tonumber(current_M)

	if current_M ~= Last_Salary_M then
		Salary_Times = 0
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Salary_Times"], 0)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Last_Salary_M"], current_M)
	end

	if TalkIndex == -1 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1611)		--普通对话
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1612)		--“查看领取条件”
		msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1613)		--“领取传道者薪酬”
		msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1614)		--“领取狂热者薪酬”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then	--查看领取说明


		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1615)		--领取条件及领取数额
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1612)		--“查看领取条件”
		msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1613)		--“领取传道者薪酬”
		msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	1614)		--“领取狂热者薪酬”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 5 then --用传道者证明领取

		if rolelevel < 50 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1616)		--“只有50级以上玩家才可领取薪酬！”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if Salary_Times >= 15 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1617)		--“您本月已领取了15次薪酬，无法再次领取！”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				if Item1Num<10 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1618)		--“您的传道者的证明道具数量不足10个，暂时无法领取。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local k = math.floor(rolelevel/10)*10
					role.RemoveFromRole(MapID, InstanceID, RoleID, 3303501, 10, 420)
					role.AddRoleGiftPoints(MapID, InstanceID, RoleID, Salary_RoleLevel_Tabel[k])
					role.SetRoleScriptData(RoleID, 1, RoleDataType["Salary_Times"], Salary_Times+1)

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1620)		--“您已成功领取数额为**点的赠点，请打开商城界面进行查看！”
					msg.AddMsgEvent(MsgID, 9, Salary_RoleLevel_Tabel[k])
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end

	elseif TalkIndex == 6 then	--用狂热者证明领取

		if rolelevel < 50 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1616)		--“只有50级以上玩家才可领取薪酬”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if Salary_Times >= 15 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1617)		--“您本月已领取了15次薪酬，无法再次领取！”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				if Item2Num<5 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1619)		--“您的狂热者的证明道具数量不足5个，暂时无法领取。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local k = math.floor(rolelevel/10)*10
					role.RemoveFromRole(MapID, InstanceID, RoleID, 3303502, 5, 420)
					role.AddRoleGiftPoints(MapID, InstanceID, RoleID, Salary_RoleLevel_Tabel[k])
					role.SetRoleScriptData(RoleID, 1, RoleDataType["Salary_Times"], Salary_Times+1)

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1620)		--“您已成功领取数额为**点的赠点，请打开商城界面进行查看！”
					msg.AddMsgEvent(MsgID, 9, Salary_RoleLevel_Tabel[k])
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end

	end
end

-- 注册
aux.RegisterCreatureEvent(3610202, 7, "XinChouShiZhe")
