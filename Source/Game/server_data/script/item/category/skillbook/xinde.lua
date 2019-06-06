function I3200205_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90000, 240)

end

function I3200205_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90000)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200205, 1, "I3200205_SkillBook")
aux.RegisterItemEvent(3200205, 0, "I3200205_CanUse")

function I3200206_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90001, 240)

end

function I3200206_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90001)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200206, 1, "I3200206_SkillBook")
aux.RegisterItemEvent(3200206, 0, "I3200206_CanUse")

function I3200207_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90002, 240)

end

function I3200207_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90002)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200207, 1, "I3200207_SkillBook")
aux.RegisterItemEvent(3200207, 0, "I3200207_CanUse")

function I3200208_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90003, 240)

end

function I3200208_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90003)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200208, 1, "I3200208_SkillBook")
aux.RegisterItemEvent(3200208, 0, "I3200208_CanUse")

function I3200209_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90004, 240)

end

function I3200209_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90004)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200209, 1, "I3200209_SkillBook")
aux.RegisterItemEvent(3200209, 0, "I3200209_CanUse")

function I3200210_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90005, 240)

end

function I3200210_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90005)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200210, 1, "I3200210_SkillBook")
aux.RegisterItemEvent(3200210, 0, "I3200210_CanUse")

function I3200211_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90006, 240)

end

function I3200211_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90006)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200211, 1, "I3200211_SkillBook")
aux.RegisterItemEvent(3200211, 0, "I3200211_CanUse")

function I3200212_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90007, 240)

end

function I3200212_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90007)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200212, 1, "I3200212_SkillBook")
aux.RegisterItemEvent(3200212, 0, "I3200212_CanUse")

function I3200213_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90008, 240)

end

function I3200213_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90008)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200213, 1, "I3200213_SkillBook")
aux.RegisterItemEvent(3200213, 0, "I3200213_CanUse")

function I3200214_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90009, 240)

end

function I3200214_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90009)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200214, 1, "I3200214_SkillBook")
aux.RegisterItemEvent(3200214, 0, "I3200214_CanUse")

---------------------------------高级生产技能心得书----------------------------


function I3200216_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90000, 2400)

end

function I3200216_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90000)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200216, 1, "I3200216_SkillBook")
aux.RegisterItemEvent(3200216, 0, "I3200216_CanUse")

function I3200217_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90001, 2400)

end

function I3200217_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90001)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200217, 1, "I3200217_SkillBook")
aux.RegisterItemEvent(3200217, 0, "I3200217_CanUse")

function I3200218_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90002, 2400)

end

function I3200218_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90002)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200218, 1, "I3200218_SkillBook")
aux.RegisterItemEvent(3200218, 0, "I3200218_CanUse")

function I3200219_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90003, 2400)

end

function I3200219_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90003)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200219, 1, "I3200219_SkillBook")
aux.RegisterItemEvent(3200219, 0, "I3200219_CanUse")

function I3200220_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90004, 2400)

end

function I3200220_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90004)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200220, 1, "I3200220_SkillBook")
aux.RegisterItemEvent(3200220, 0, "I3200220_CanUse")

function I3200221_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90005, 2400)

end

function I3200221_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90005)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200221, 1, "I3200221_SkillBook")
aux.RegisterItemEvent(3200221, 0, "I3200221_CanUse")

function I3200222_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90006, 2400)

end

function I3200222_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90006)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200222, 1, "I3200222_SkillBook")
aux.RegisterItemEvent(3200222, 0, "I3200222_CanUse")

function I3200223_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90007, 2400)

end

function I3200223_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90007)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200223, 1, "I3200223_SkillBook")
aux.RegisterItemEvent(3200223, 0, "I3200223_CanUse")

function I3200224_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90008, 2400)

end

function I3200224_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90008)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200224, 1, "I3200224_SkillBook")
aux.RegisterItemEvent(3200224, 0, "I3200224_CanUse")

function I3200225_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90009, 2400)

end

function I3200225_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90009)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200225, 1, "I3200225_SkillBook")
aux.RegisterItemEvent(3200225, 0, "I3200225_CanUse")

---------------------------------师徒生产技能心得书----------------------------


function I3200226_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90000, 160)

end

function I3200226_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90000)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200226, 1, "I3200226_SkillBook")
aux.RegisterItemEvent(3200226, 0, "I3200226_CanUse")

function I3200227_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90001, 160)

end

function I3200227_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90001)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200227, 1, "I3200227_SkillBook")
aux.RegisterItemEvent(3200227, 0, "I3200227_CanUse")

function I3200228_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90002, 160)

end

function I3200228_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90002)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200228, 1, "I3200228_SkillBook")
aux.RegisterItemEvent(3200228, 0, "I3200228_CanUse")

function I3200229_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90003, 160)

end

function I3200229_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90003)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200229, 1, "I3200229_SkillBook")
aux.RegisterItemEvent(3200229, 0, "I3200229_CanUse")

function I3200230_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90004, 160)

end

function I3200230_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90004)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200230, 1, "I3200230_SkillBook")
aux.RegisterItemEvent(3200230, 0, "I3200230_CanUse")

function I3200231_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90005, 160)

end

function I3200231_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90005)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200231, 1, "I3200231_SkillBook")
aux.RegisterItemEvent(3200231, 0, "I3200231_CanUse")

function I3200232_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90006, 160)

end

function I3200232_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90006)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200232, 1, "I3200232_SkillBook")
aux.RegisterItemEvent(3200232, 0, "I3200232_CanUse")

function I3200233_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90007, 160)

end

function I3200233_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90007)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200233, 1, "I3200233_SkillBook")
aux.RegisterItemEvent(3200233, 0, "I3200233_CanUse")

function I3200234_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90008, 160)

end

function I3200234_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90008)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3200234, 1, "I3200234_SkillBook")
aux.RegisterItemEvent(3200234, 0, "I3200234_CanUse")

function I3200235_SkillBook(MapID, InstanceID, TypeID, TargetID)

	role.ChangeSkillExp(TargetID, 90009, 160)

end

function I3200235_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90009)

	if LearnedSkill==nil then
		--提示玩加物品不可使用
		bRet = 32
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10002)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200235, 1, "I3200235_SkillBook")
aux.RegisterItemEvent(3200235, 0, "I3200235_CanUse")
