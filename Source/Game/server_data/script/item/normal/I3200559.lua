--使用沸血秘药3200559取消凝血9337301
--可否使用函数
function i3200559_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
        --不在礼天殿，不能使用
	if MapID ~= 3000521935 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i3200559_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.CancelBuff(MapID, InstanceID, TargetID, 9337301)
end

--注册
aux.RegisterItemEvent(3200559, 1, "i3200559_QuestUsable")
aux.RegisterItemEvent(3200559, 0, "i3200559_CanUse")

