function ShengChanXinDe_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local Produce_Skill_Num=0

	for i=0, 9 do
		local LearnedSkill = role.IsLearnedSkill(TargetID, 90000+i)
		if LearnedSkill~=nil then
			Produce_Skill_Num = Produce_Skill_Num +1
		end
	end

	if Produce_Skill_Num==0 then
		bRet = 32
		--提示玩家没有学会生产技能
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10001)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
	end

	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < Produce_Skill_Num) then
		--提示玩家背包空间不足
		bRet = 40
	end

	return bRet, bIgnore

end

function ShengChanXinDe(MapID, InstanceID, TypeID, TargetID)

	for i=0, 9 do
		local LearnedSkill = role.IsLearnedSkill(TargetID, 90000+i)
		if LearnedSkill~=nil then
			role.AddRoleItem(MapID, InstanceID, TargetID, 3200226+i, 1, -1, 8, 420)
		end
	end

end

aux.RegisterItemEvent(3200215, 0, "ShengChanXinDe_CanUse")
aux.RegisterItemEvent(3200215, 1, "ShengChanXinDe")
