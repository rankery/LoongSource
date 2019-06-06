--两个复活技能不能在竞技场中使用

function FuHuo_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	if MapID == 2092185140 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 2531)       --竞技场中不能使用该技能
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2531)       --竞技场中不能使用该技能
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 7
	end
	return 0
end


--注册
aux.RegisterSkillEvent(1081301, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081302, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081303, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081304, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081305, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081306, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081307, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1081308, 0, "FuHuo_CanCast")

aux.RegisterSkillEvent(1082801, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1082802, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1082803, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1082804, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1082805, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1082806, 0, "FuHuo_CanCast")

aux.RegisterSkillEvent(1102701, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1102702, 0, "FuHuo_CanCast")
aux.RegisterSkillEvent(1102703, 0, "FuHuo_CanCast")
