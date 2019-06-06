--RKL147
--需求1：使用天弦妖令2615047刷出天弦水魔1009059

--可否使用函数
function i2615047_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 2810 or x > 2910 or z < 1530 or z > 1630 then
	          bRet = 43 
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615047_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 1009059, 2865, 7423, 1582)
end

--注册
aux.RegisterItemEvent(2615047, 1, "i2615047_QuestUsable")
aux.RegisterItemEvent(2615047, 0, "i2615047_CanUse")