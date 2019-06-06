--RWS108 使用百家饭（2613625）后获得BUFF五内翻腾（2018901），空碗（2613626）和接取任务13109

--可否使用函数
function i2613625_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在巫水，则不能使用
	if MapID ~= 3017299919 then
	    bRet = 43
	end
    return bRet, bIgnore
end

--使用效果
function i2613625_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.QuestAddRoleItem(MapID, InstanceID, TargetID, 13108, 2613626, 1, 0, 102)
       unit.AddBuff(MapID, InstanceID, TargetID, 2018901, TargetID)
       role.AddQuest(TargetID, 13109)
end

--注册
aux.RegisterItemEvent(2613625, 1, "i2613625_QuestUsable")
aux.RegisterItemEvent(2613625, 0, "i2613625_CanUse")
