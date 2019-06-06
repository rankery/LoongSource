--使用炽魂秘药3200635取消冰寒彻骨9425801
--可否使用函数
function i3200635_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
        --不在玉虚峰，不能使用
	if MapID ~= 3084408527 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i3200635_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.CancelBuff(MapID, InstanceID, TargetID, 9425801)
end

--注册
aux.RegisterItemEvent(3200635, 1, "i3200635_QuestUsable")
aux.RegisterItemEvent(3200635, 0, "i3200635_CanUse")

