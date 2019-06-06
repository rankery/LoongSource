function I4200000_SkillBook(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举

	role.AddSkill(TargetID, 9001201)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2220)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2220)
	msg.DispatchRoleMsgEvent(ownerID, MsgID)

end

function I4200000_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90012)

	if LearnedSkill~=nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10001)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(4200000, 1, "I4200000_SkillBook")
aux.RegisterItemEvent(4200000, 0, "I4200000_CanUse")
