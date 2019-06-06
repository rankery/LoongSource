--使用整理好的锦囊（2614244），刷出坟墓（3050192）
--可否使用函数
function i2614244_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID ~= 1146339967 or x < 1468 or x > 1568 or z < 1202 or z > 1302 then
		return 43,false
	end
	return bRet, bIgnore
end

--使用效果
function i2614244_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 3050192, x, y, z)
end

--注册
aux.RegisterItemEvent(2614244, 1, "i2614244_QuestUsable")
aux.RegisterItemEvent(2614244, 0, "i2614244_CanUse")


