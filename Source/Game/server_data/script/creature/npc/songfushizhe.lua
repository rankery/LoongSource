--送福使者 2月14日之前不能买东西
function n4900403_IsShopOpen(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local curmonth = tonumber(os.date("%m"))
	local curday = tonumber(os.date("%d"))
	if curmonth == 2 then
		if curday < 14 then
			return 0
		else
			return 1
		end
	elseif curmonth < 2 then
		return 0
	elseif curmonth > 2 then
		return 1
	end
	return 0
end
aux.RegisterCreatureEvent(4900403, 15, "n4900403_IsShopOpen")
