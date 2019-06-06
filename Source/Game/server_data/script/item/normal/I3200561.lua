--使用御蛊秘药3200561取消散功蛊9337601

--可否使用函数
function i3200561_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在魔王殿，不能使用
	if MapID ~= 3000523471 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i3200561_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.CancelBuff(MapID, InstanceID, TargetID, 9337601)
end

--注册
aux.RegisterItemEvent(3200561, 1, "i3200561_QuestUsable")
aux.RegisterItemEvent(3200561, 0, "i3200561_CanUse")

