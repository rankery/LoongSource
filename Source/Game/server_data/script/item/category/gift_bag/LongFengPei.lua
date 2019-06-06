function LongFengPei_1_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 3312501,TargetID)
end
function LongFengPei_1_CanUse(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then --男性可用
	return 0,false
	else
	return 34,false
	end

end

function LongFengPei_2_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 3312701,TargetID)
end
function LongFengPei_2_CanUse(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 0 then --女性可用
	return 0,false
	else
	return 34,false
	end

end

function DaLongFengPei_1_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 3312501,TargetID)
end
function DaLongFengPei_1_CanUse(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then --男性可用
	return 0,false
	else
	return 34,false
	end

end

function DaLongFengPei_2_Use(MapID, InstanceID, TypeID, TargetID)
	unit.AddBuff(MapID, InstanceID, TargetID, 3312701,TargetID)
end
function DaLongFengPei_2_CanUse(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 0 then --女性可用
	return 0,false
	else
	return 34,false
	end

end

aux.RegisterItemEvent(3303137, 1, "LongFengPei_1_Use")
aux.RegisterItemEvent(3303137, 0, "LongFengPei_1_CanUse")
aux.RegisterItemEvent(3303138, 1, "LongFengPei_2_Use")
aux.RegisterItemEvent(3303138, 0, "LongFengPei_2_CanUse")
aux.RegisterItemEvent(3303139, 1, "DaLongFengPei_1_Use")
aux.RegisterItemEvent(3303139, 0, "DaLongFengPei_1_CanUse")
aux.RegisterItemEvent(3303140, 1, "DaLongFengPei_2_Use")
aux.RegisterItemEvent(3303140, 0, "DaLongFengPei_2_CanUse")
