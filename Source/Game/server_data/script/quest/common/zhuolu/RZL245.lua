--离间之计：12245
--需求1：在营地中[m04: 2587, 1861]附近使用物品伪造信件[2612133]后获得任务道具空信封[2612136]

--物品伪造新建使用判断函数
function i2612133_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	
	--不是涿鹿不可以使用
	if MapID ~= 3017299663 then
	    bRet = 43
	else	
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=2571 or x>=2603 or z<=1845 or z>=1877 then    --必须在该范围内使用
		    bRet = 43
	    end
	end

    --返回
	return bRet, bIgnore
end

--物品伪造信件使用效果函数
function i2612133_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.QuestAddRoleItem(MapID, InstanceID, TargetID, 12245, 2612136, 1, 0, 102)
end

--注册
aux.RegisterItemEvent(2612133, 1, "i2612133_QuestUsable")
aux.RegisterItemEvent(2612133, 0, "i2612133_CanUse")
