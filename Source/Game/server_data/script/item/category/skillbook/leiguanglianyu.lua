function leiguang_SkillBook(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local level = role.IsLearnedSkill(TargetID, 10108)
    if level ~= nil then
	    role.RemoveSkill(TargetID, 10108)
	end
    local a = TypeID - 3200472
	role.AddSkill(TargetID, 1010800+a)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 2221)
	msg.AddMsgEvent(MsgID, 9, a)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 2221)
	msg.AddMsgEvent(MsgID, 9, a)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)

end


aux.RegisterItemEvent(3200473, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200474, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200475, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200476, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200477, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200478, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200479, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200480, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200481, 1, "leiguang_SkillBook")
aux.RegisterItemEvent(3200482, 1, "leiguang_SkillBook")


function leiguangbook_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会技能且等级合适

	local LearnedSkill = role.IsLearnedSkill(TargetID, 10108)

	if LearnedSkill~=nil and TypeID - 3200472 <= LearnedSkill then
		--提示物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2222)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200473, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200474, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200475, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200476, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200477, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200478, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200479, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200480, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200481, 0, "leiguangbook_CanUse")
aux.RegisterItemEvent(3200482, 0, "leiguangbook_CanUse")
