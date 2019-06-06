--使用龙魂爆发图鉴3200566，学会技能龙魂爆发9001301
--使用效果
function i3200566_Usable(MapID, InstanceID, TypeID, TargetID)
	role.AddSkill(TargetID, 9001301)
end

--注册
aux.RegisterItemEvent(3200566, 1, "i3200566_Usable")


function I3200566_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90013)

	if LearnedSkill~=nil then
		--提示玩加物品不可使用
		bRet = 32
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200566, 0, "I3200566_CanUse")
