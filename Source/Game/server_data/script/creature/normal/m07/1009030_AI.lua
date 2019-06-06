--怪物死亡时，如果有任务15107，则可能掉落戾沌兽血2615029
function c1009030_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=10 then
		local a = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 15107)
        if a == true then
	        local FreeSize = role.GetBagFreeSize(RoleID)
            if FreeSize > 0 then
	            role.AddRoleItem(MapID, InstanceID, RoleID, 2615029, 1, -1, 8, 420)
            end
	    end
	end
end

aux.RegisterCreatureEvent(1009030, 4, "c1009030_OnDie")
