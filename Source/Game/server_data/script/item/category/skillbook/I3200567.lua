--使用龙魂激增图鉴3200567，学会技能龙魂激增9001401
--使用效果
function i3200567_Usable(MapID, InstanceID, TypeID, TargetID)
	role.AddSkill(TargetID, 9001401)
end

--注册
aux.RegisterItemEvent(3200567, 1, "i3200567_Usable")

function I3200567_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断玩家是否已学会骑程技能

	local LearnedSkill = role.IsLearnedSkill(TargetID, 90014)

	if LearnedSkill~=nil then
		--提示玩加物品不可使用
		bRet = 32
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3200567, 0, "I3200567_CanUse")
