--11905

function c1534134_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
end

--×¢²áº¯Êý
aux.RegisterCreatureEvent(1534134, 4, "c1534134_OnDie")
