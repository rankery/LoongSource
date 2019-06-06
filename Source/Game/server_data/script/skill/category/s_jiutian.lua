--技能——九天雷劫
--[[
1110801 消耗1
1110802 消耗1
1110803 消耗2
1110804 消耗2
1110805 消耗3
]]
--10003 使用该技能需要一个风火石
--10005 使用该技能需要两个风火石
--10006 使用该技能需要三个风火石

function jiutian1_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	if role.GetRoleItemNum(OwnerID, 3200464) > 0 then
		return 0
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 10003)       --使用该技能需要一个风火石
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 7
	end
end

function jiutian1_Cast(MapID, InstanceID, SkillID, OwnerID)
    role.RemoveFromRole(MapID, InstanceID, OwnerID, 3200464, 1, 420)
	return 0
end

--注册

aux.RegisterSkillEvent(1110801, 1, "jiutian1_Cast")
aux.RegisterSkillEvent(1110801, 0, "jiutian1_CanCast")
aux.RegisterSkillEvent(1110802, 1, "jiutian1_Cast")
aux.RegisterSkillEvent(1110802, 0, "jiutian1_CanCast")

function jiutian2_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	if role.GetRoleItemNum(OwnerID, 3200464) > 1 then
		return 0
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 10005)       --使用该技能需要两个风火石
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 7
	end
end

function jiutian2_Cast(MapID, InstanceID, SkillID, OwnerID)
    role.RemoveFromRole(MapID, InstanceID, OwnerID, 3200464, 2, 420)
	return 0
end

--注册

aux.RegisterSkillEvent(1110803, 1, "jiutian2_Cast")
aux.RegisterSkillEvent(1110803, 0, "jiutian2_CanCast")
aux.RegisterSkillEvent(1110804, 1, "jiutian2_Cast")
aux.RegisterSkillEvent(1110804, 0, "jiutian2_CanCast")

function jiutian3_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	if role.GetRoleItemNum(OwnerID, 3200464) > 2 then
		return 0
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 10006)       --使用该技能需要三个风火石
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 7
	end
end

function jiutian3_Cast(MapID, InstanceID, SkillID, OwnerID)
    role.RemoveFromRole(MapID, InstanceID, OwnerID, 3200464, 3, 420)
	return 0
end


--注册

aux.RegisterSkillEvent(1110805, 1, "jiutian3_Cast")
aux.RegisterSkillEvent(1110805, 0, "jiutian3_CanCast")
