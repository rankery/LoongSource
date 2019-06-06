--NPC修炼场传送人脚本

function JCXL_chuanru(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local curhour = tonumber(os.date("%H"))

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	87)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	80)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	81)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		local Role_GuildID = guild.GetRoleGuildID(RoleID)

		local Master_GuilID = city.GetCityAppointedAtt(3, 2)

		if Role_GuildID==4294967295 or Role_GuildID~=Master_GuilID then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	88)	--您并非本城占领帮派成员，无法进入禁城修炼场！
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if curhour<15 or curhour>=18 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	89)	--禁城修炼场开放时间未到，请15:00-18:00之间再来！
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else

				local Cur_D = tonumber(os.date("%j"))
				local Last_JCXL_Day = role.GetRoleScriptData(RoleID, 1, RoleDataType["Last_JCXL_Day"])

				if Cur_D~=Last_JCXL_Day then
					role.RoleGotoNewMap(MapID, InstanceID, RoleID, 1356752602, 175, 3, 87)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["Last_JCXL_Day"], Cur_D)
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 	90)	--您今天已经进禁城修炼场修炼过了，无法再次进入！
					msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
					msg.AddMsgEvent(MsgID, 1, 	22)		--取消
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end

	end
end

function JCXL_chuanchu(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	79)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	84)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	85)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3695619387, 800, 10350, 590)

	end
end

-- 注册
aux.RegisterCreatureEvent(4500701, 7, "JCXL_chuanchu")
aux.RegisterCreatureEvent(4500702, 7, "JCXL_chuanru")
