

--15206 接取任务后在(M07,x961,y7402,z2580)附近使用道具灵宝婵香[2615104]后，获得物品婵娟仙石[2615105]


function i2615104_QuestUsable(MapID, InstanceID, TypeID, RoleID)
	role.AddRoleItem(MapID, InstanceID, RoleID, 2615105, 1, -1, 8, 420)

end

function i2615104_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)

        if(FreeSize < 1) then
	--提示玩家背包空间不足
	bRet = 40
	return bRet, bIgnore
	end

	--必须在昆仑使用
	if MapID ~= 3017299407 then
	    bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=945 or x>=975 or z<=2565 or z>=2580 then --必须在该位置附近使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--注册
aux.RegisterItemEvent(2615104, 1, "i2615104_QuestUsable")
aux.RegisterItemEvent(2615104, 0, "i2615104_CanUse")