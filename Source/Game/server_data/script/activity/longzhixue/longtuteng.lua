function longtuteng(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	3601+TargetTypeID-4901001)		--请选择
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	3605)			--“提交”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then

		if 0==role.RemoveFromRole(MapID, InstanceID, RoleID, 1350051+TargetTypeID-4901001, 1, 420) then

			local ask_times = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1)+1
			cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1, ask_times)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	3606)		--提交成功，当前已提交次数：x/10。
			msg.AddMsgEvent(MsgID, 9,	ask_times)
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	3607)			--“继续提交”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

			if ask_times==10 then

				local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
				map.MapCreateCreature(MapID, InstanceID, 1550201+TargetTypeID-4901001, x, y, z)

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	3608)		--神龙已被唤醒
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

				map.MapDeleteCreature(MapID, InstanceID, TargetID)

			end

		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	3609)		--没有真龙四宝
			msg.AddMsgEvent(MsgID, 4,	1350051+TargetTypeID-4901001)
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

		end

	end

end

aux.RegisterCreatureEvent(4901001, 7, "longtuteng")
aux.RegisterCreatureEvent(4901002, 7, "longtuteng")
aux.RegisterCreatureEvent(4901003, 7, "longtuteng")
aux.RegisterCreatureEvent(4901004, 7, "longtuteng")
