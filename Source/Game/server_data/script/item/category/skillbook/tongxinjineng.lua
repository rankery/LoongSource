function I3305011_SkillBook(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举

	role.RemoveSkill(TargetID, 11601)

	role.AddSkill(TargetID, 1160101+TypeID-3305011)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2223)
	msg.AddMsgEvent(MsgID, 9, TypeID-3305010)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

end

function I3305011_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 11601)
	local booklevel = TypeID-3305010

	if LearnedSkill==nil and booklevel~=1 then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10007)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end

	if LearnedSkill~=nil and booklevel-LearnedSkill~=1 then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10007)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end

	return bRet, bIgnore
end


aux.RegisterItemEvent(3305011, 1, "I3305011_SkillBook")
aux.RegisterItemEvent(3305012, 1, "I3305011_SkillBook")
aux.RegisterItemEvent(3305013, 1, "I3305011_SkillBook")
aux.RegisterItemEvent(3305014, 1, "I3305011_SkillBook")
aux.RegisterItemEvent(3305015, 1, "I3305011_SkillBook")

aux.RegisterItemEvent(3305011, 0, "I3305011_CanUse")
aux.RegisterItemEvent(3305012, 0, "I3305011_CanUse")
aux.RegisterItemEvent(3305013, 0, "I3305011_CanUse")
aux.RegisterItemEvent(3305014, 0, "I3305011_CanUse")
aux.RegisterItemEvent(3305015, 0, "I3305011_CanUse")
