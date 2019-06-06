--使用极寒秘药3200560取消烈焰缠身9337401
--可否使用函数
function i3200560_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
        --不在嫘祖之谷，不能使用
	if MapID ~= 3000523727 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i3200560_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.CancelBuff(MapID, InstanceID, TargetID, 9337401)
end

--注册
aux.RegisterItemEvent(3200560, 1, "i3200560_QuestUsable")
aux.RegisterItemEvent(3200560, 0, "i3200560_CanUse")

