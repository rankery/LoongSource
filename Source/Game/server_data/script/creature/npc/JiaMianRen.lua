--NPC修炼场传送人脚本

function JiaMianRen(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	3029)		--“你就是新任凤翔城主吗？果然是英姿飒爽，人中豪杰。我将赐予你城主之称号及战袍，望你万勿辱没凤翔城主之名！”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	3030)			--“领取城主称号及战袍”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		local Role_GuildID = guild.GetRoleGuildID(RoleID)
		local Master_GuilID = city.GetCityAppointedAtt(3, 2)

		local Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, RoleID)

		if Role_GuildID==4294967295 or Role_GuildID~=Master_GuilID or Role_ZhiWei~= 2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	3031)		--只有这凤翔承天府占领帮派的帮主才是凤翔城主，休想骗过我的眼睛！
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else

			local FreeSize = role.GetBagFreeSize(RoleID)

			if FreeSize<6 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	3032)		--领取城主战袍需要6个以上行囊空格，请整理背包后再次领取！
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				role.SigTitleEvent(RoleID, 262)

				local role_sex = role.GetRoleSex(RoleID)
				if role_sex == 0 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140307, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140308, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140309, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140310, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140311, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140312, 1, 4, 0, 380)
				elseif role_sex == 1 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140301, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140302, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140303, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140304, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140305, 1, 4, 0, 380)
					role.AddRoleItem(MapID, InstanceID, RoleID, 9140306, 1, 4, 0, 380)
				end

				map.MapDeleteCreature(MapID, InstanceID, TargetID)

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	3033)		--XX帮派的XX已经正式即为为新任凤翔城主，真乃人中之龙！
				msg.AddMsgEvent(MsgID, 19, Role_GuildID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchWorldMsgEvent(MsgID)
			end
		end

	end
end

aux.RegisterCreatureEvent(4500705, 7, "JiaMianRen")
