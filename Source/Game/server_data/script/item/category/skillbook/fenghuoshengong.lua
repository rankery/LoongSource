function I3200463_SkillBook(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举

	role.AddSkill(TargetID, 1010701)

end

function I3200463_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会风火神功技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 10107)

	if LearnedSkill~=nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10001)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200463, 1, "I3200463_SkillBook")
aux.RegisterItemEvent(3200463, 0, "I3200463_CanUse")
