function I3303005_GiftBag(MapID, InstanceID, TypeID, TargetID)
    local k = math.random(100)
    local i = math.random(100)
    local j = math.random(100)
	if k <= 30 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303006, 1, -1, 8, 420)
	elseif k <= 45 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303009, 1, -1, 8, 420)
	elseif k <= 60 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303011, 1, -1, 8, 420)
	elseif k <= 80 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303007, 1, -1, 8, 420)
	elseif k <= 95 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303001, 1, -1, 8, 420)
	elseif k <= 100 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303002, 1, -1, 8, 420)
	end
	if i <= 14 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350501, 2, -1, 8, 420)
	elseif i <= 28 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350502, 2, -1, 8, 420)
	elseif i <= 42 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350503, 2, -1, 8, 420)
	elseif i <= 56 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350504, 2, -1, 8, 420)
	elseif i <= 70 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350505, 2, -1, 8, 420)
	elseif i <= 85 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350506, 2, -1, 8, 420)
	elseif i <= 100 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 1350507, 2, -1, 8, 420)
	end
	if j <= 15 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303035, 1, -1, 8, 420)
	elseif j <= 30 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303036, 1, -1, 8, 420)
	elseif j <= 45 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303037, 1, -1, 8, 420)
	elseif j <= 60 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303038, 1, -1, 8, 420)
	elseif j <= 75 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303039, 1, -1, 8, 420)
	elseif j <= 83 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303040, 1, -1, 8, 420)
	elseif j <= 91 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303041, 1, -1, 8, 420)
	elseif j <= 100 then
	    role.AddRoleItem(MapID, InstanceID, TargetID, 3303042, 1, -1, 8, 420)
	end
	local curmon = tonumber(os.date("%m"))
	local curday = tonumber(os.date("%d"))
	if (curmon == 2 and curday >= 5) or (curmon == 3 and curday <= 7) then
	    local FreeSize = role.GetBagFreeSize(TargetID)
	    if(FreeSize > 0) then
		    local kj = math.random(100)
            if kj <= 15 then
				role.AddRoleItem(MapID, InstanceID, TargetID, 3303131, 1, -1, 8, 420)
            elseif kj <= 25 then
	            role.AddRoleItem(MapID, InstanceID, TargetID, 3303127, 1, -1, 8, 420)
            elseif kj <= 28 then
	            role.AddRoleItem(MapID, InstanceID, TargetID, 9140121, 1, -1, 8, 420)
			end
		end
	end
end

function I3303005_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3303005, 1, "I3303005_GiftBag")
aux.RegisterItemEvent(3303005, 0, "I3303005_CanUseGiftBag")
