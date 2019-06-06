--RZL902
--在凤翔（3017298383）的2096，2472（X,Z）附近（x，z正负70）使用幻象宝珠（3200518），在2058，10842，2431刷出离奢幻象（1531002）

--可否使用函数
function i3200518_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在凤翔场景，则不能使用
	if MapID ~= 3017298383 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=2026 or x>=2166 or z<=2402 or z>=2542 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--幻象宝珠使用效果
function i3200518_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531002, 2058, 10842, 2431)
end

--注册
aux.RegisterItemEvent(3200518, 1, "i3200518_QuestUsable")
aux.RegisterItemEvent(3200518, 0, "i3200518_CanUse")
