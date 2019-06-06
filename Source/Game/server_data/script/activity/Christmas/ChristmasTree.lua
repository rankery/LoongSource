--RoleDataType["Christmas_Box"]  = 162  记录玩家当天是否已将空礼盒挂在圣诞树上
--6010808 空的炫彩礼盒
--6010809 炫彩礼盒



function ChristmasTree_OnTalk(MapID,InstanceID,TargetID,TargetTypeID,RoleID,TalkIndex)

	local Christmas_Box = role.GetRoleScriptData(RoleID,1,RoleDataType["Christmas_Box"]) --获取玩家当天的空礼盒是否已挂在树上
	local curday = tonumber(os.date("%j"))	 	-- 获取服务器当前日期
	local curhour = tonumber(os.date("%H"))  	-- 获取服务器当前小时数
	local Shengdan = tonumber(os.date("%j"))   --获取服务器当前日期记录

	if TalkIndex == -1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 4401)	-- "你每天可以挂上一个礼盒，圣诞老人会送你好东西哦！"
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 4402)	-- "挂上空的礼盒"
			msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
			msg.AddMsgEvent(MsgID, 1, 4403)	-- "领取礼盒"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		if curhour < 21 and Shengdan ~= Christmas_Box then
			if role.GetRoleItemNum(RoleID, 6010808) < 1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 4404)		--如果您有空的礼盒，请将它放进您的背包来找我。
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				role.RemoveFromRole(MapID, InstanceID, RoleID, 6010808, 1, 420)		--删除玩家背包内的空盒子
				role.SetRoleScriptData(RoleID, 1, RoleDataType["Christmas_Box"], curday) --增加玩家的挂盒子记录
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 4412)		--您的礼盒已经挂在了圣诞树上，请您耐心等待圣诞老人给您带来的惊喜吧！
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif Christmas_Box == Shengdan and curhour < 21 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 4405)		--您今天已经将空礼盒挂在圣诞树上了，请在21:00-24:00之间来领取礼物吧！
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif curhour >= 21 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 4408)		--只有在00：00-21：00之间才可以将空礼盒挂在树上
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	elseif TalkIndex == 5 then
		if Christmas_Box ~= Shengdan and curhour >= 21 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 4407)		--今天您没有将空礼盒挂在圣诞树上哦！
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Christmas_Box == Shengdan and curhour >= 21 then
			if role.GetBagFreeSize(RoleID) > 0 then		-- 判断玩家是否有足够背包空间
				role.AddRoleItem(MapID, InstanceID, RoleID, 6010809, 1, -1, 8, 420)	--给玩家背包内增加礼盒
				role.SetRoleScriptData(RoleID, 1, RoleDataType["Christmas_Box"], 0) --将玩家的挂盒子记录清除
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 4409)		--您的背包空间不足！
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif curhour < 21 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 4406)		--还没到领取时间，请您在21:00-24:00之间来领取礼物吧！
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end
end

aux.RegisterCreatureEvent(4900714, 7, "ChristmasTree_OnTalk")
