--11905
function I2611133_OnUse(MapID, InstanceID, TypeID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 1534134, x, y, z, 1, "")
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	map.SetMapScriptData(MapID,InstanceID,1,2,a+1)
end

function I2611133_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	if(a >= 3) then
		bRet = 32
	end
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID ~= 3017298383 or x < 858 or x > 938 or z < 1902 or z > 1982 then
	    bRet = 32
	end

    return bRet, bIgnore
end

aux.RegisterItemEvent(2611133, 1, "I2611133_OnUse")
aux.RegisterItemEvent(2611133, 0, "I2611133_CanUse")
