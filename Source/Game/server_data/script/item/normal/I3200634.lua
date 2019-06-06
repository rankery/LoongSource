--使用培元秘药3200634取消溃灵9425701
--可否使用函数
function i3200634_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
        --不在青绝洞府，不能使用
	if MapID ~= 3084408783 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i3200634_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.CancelBuff(MapID, InstanceID, TargetID, 9425701)
end

--注册
aux.RegisterItemEvent(3200634, 1, "i3200634_QuestUsable")
aux.RegisterItemEvent(3200634, 0, "i3200634_CanUse")

