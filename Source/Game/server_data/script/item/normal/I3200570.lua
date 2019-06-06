--使用试炼炉（3200570），传送进试炼幻境（1826990375）
--可否使用函数
function i3200570_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID ~= 3017298383 or x < 652 or x > 1122 or z < 1872 or z > 2334 then
		return 43,false
	end
	return bRet, bIgnore
end

--使用效果
function i3200570_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.RoleGotoNewMap(MapID, InstanceID, TargetID, 1826990375, 171, 5, 32)
end

--注册
aux.RegisterItemEvent(3200570, 1, "i3200570_QuestUsable")
aux.RegisterItemEvent(3200570, 0, "i3200570_CanUse")


