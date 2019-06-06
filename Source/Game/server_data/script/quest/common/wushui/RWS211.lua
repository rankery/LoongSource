--勇士之路(下)：13211
--需求1：在多力墓园[m05: 1915, 331]附近使用物品血酒[2613108]后获得任务道具空碗[2613109]

--物品血酒使用判断函数
function i2613108_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	--不在龙城不能使用
	if MapID ~= 3017298127 then
	    bRet = 43
	else
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
        if x<=1899 or x>=1931 or z<=315 or z>=347 then  --必须在该范围内使用
            bRet = 43
        end
    end

    --返回
	return bRet, bIgnore
end

--物品血酒使用效果函数
function i2613108_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.QuestAddRoleItem(MapID, InstanceID, TargetID, 13211, 2613109, 1, 0, 102)
end

--注册
aux.RegisterItemEvent(2613108, 1, "i2613108_QuestUsable")
aux.RegisterItemEvent(2613108, 0, "i2613108_CanUse")
