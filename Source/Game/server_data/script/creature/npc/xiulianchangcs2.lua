--NPC修炼场传送人脚本

function xiulianchangcs2(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	83)		--“是否进入副本”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	84)			--“确定”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	85)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		local bhave = role.IsRoleHaveBuff(MapID, -1, RoleID, 20202)
		if bhave == true then
			--删除免费修炼场BUFF
			unit.CancelBuff(MapID, -1, RoleID, 2020201)
		end
		if TargetTypeID==4500401 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298383, 2485, 13210, 1532)
		elseif TargetTypeID==4500421 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299663, 1602, 15687, 1507)
		elseif TargetTypeID==4500441 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299919, 2512, 22813, 529)
		elseif TargetTypeID==4500461 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299151, 593, 3500, 1774)
		elseif TargetTypeID==4500521 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298127, 2177, 115, 2098)
		elseif TargetTypeID==4500481 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 1146339967, 553, 12988, 1970)
        elseif TargetTypeID==4500501 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299407, 1759, 302, 2693)
		end
	end
end

-- 注册
aux.RegisterCreatureEvent(4500401, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500421, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500441, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500461, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500521, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500481, 7, "xiulianchangcs2")
aux.RegisterCreatureEvent(4500501, 7, "xiulianchangcs2")
