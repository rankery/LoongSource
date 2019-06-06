--技能——雷火炼狱 1010801
--使用一次消耗一个风火石

function leihuo_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
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

function leihuo_Cast(MapID, InstanceID, SkillID, OwnerID)
    role.RemoveFromRole(MapID, InstanceID, OwnerID, 3200464, 1, 420)
	return 0
end


--注册

aux.RegisterSkillEvent(1010801, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010801, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010802, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010802, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010803, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010803, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010804, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010804, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010805, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010805, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010806, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010806, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010807, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010807, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010808, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010808, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010809, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010809, 0, "leihuo_CanCast")
aux.RegisterSkillEvent(1010810, 1, "leihuo_Cast")
aux.RegisterSkillEvent(1010810, 0, "leihuo_CanCast")
