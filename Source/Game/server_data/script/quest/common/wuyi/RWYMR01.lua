--RWYMR01:»¢±ªÖ®Õù
--ÐèÇó1£ºÓÐÎïÆ·[½£ÈÐ»¢Èâ][2614023]£¬É±ËÀÑ×½ð°ßÎÆ±ª[1007005][1007006]£¬Ë¢³ö[Ñ×½ðÐ×±ª][1007030]£¬É¾³ý[½£ÈÐ»¢Èâ][2614023]
--ÐèÇó2£ºÓÐÎïÆ·[Ñ×½ð±ªÈâ][2614024]£¬É±ËÀ½£ÈÐ»¢[1007003][1007004], Ë¢³ö[½£ÈÐÃÍ»¢][1007031], É¾³ý[Ñ×½ð±ªÈâ][2614024]




--ÐèÇó1£ºÓÐÎïÆ·[½£ÈÐ»¢Èâ][2614023]£¬É±ËÀÑ×½ð±ª[1007005][1007006]£¬Ë¢³ö[Ñ×½ðÐ×±ª][1007030]£¬É¾³ý[½£ÈÐ»¢Èâ][2614023]
function c1007005_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local num = role.GetRoleItemNum(RoleID, 2614023)
	if num > 0 then
	map.MapCreateCreature(MapID, InstanceID, 1007030, x+2, y, z+2)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614023, 1, 101)
	end
end

aux.RegisterCreatureEvent(1007005, 4, "c1007005_OnDie")

function c1007006_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local num = role.GetRoleItemNum(RoleID, 2614023)
	if num > 0 then
	map.MapCreateCreature(MapID, InstanceID, 1007030, x+2, y, z+2)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614023, 1, 101)
	end
end

aux.RegisterCreatureEvent(1007006, 4, "c1007006_OnDie")


--ÐèÇó2£ºÓÐÎïÆ·[Ñ×½ð±ªÈâ][2614024]£¬É±ËÀ½£ÈÐ»¢[1007003][1007004], Ë¢³ö[½£ÈÐÃÍ»¢][1007031], É¾³ý[Ñ×½ð±ªÈâ][2614024]
function c1007003_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local num = role.GetRoleItemNum(RoleID, 2614024)
	if num > 0 then
	map.MapCreateCreature(MapID, InstanceID, 1007031, x+2, y, z+2)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614024, 1, 101)
	end
end

aux.RegisterCreatureEvent(1007003, 4, "c1007003_OnDie")

function c1007004_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local num = role.GetRoleItemNum(RoleID, 2614024)
	if num > 0 then
	map.MapCreateCreature(MapID, InstanceID, 1007031, x+2, y, z+2)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614024, 1, 101)
	end
end

aux.RegisterCreatureEvent(1007004, 4, "c1007004_OnDie")
