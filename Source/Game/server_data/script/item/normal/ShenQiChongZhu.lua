--可否使用函数
function i3200604_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8200209, 8200216 do
		if role.GetRoleItemNum(TargetID, i) > 0 then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

function i3200604_Use(MapID, InstanceID, TypeID, TargetID)
    for i = 8200209, 8200216 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, -1, 8, 420)
		end
	end
end

aux.RegisterItemEvent(3200604, 1, "i3200604_Use")
aux.RegisterItemEvent(3200604, 0, "i3200604_CanUse")


function i3200605_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8200309, 8200316 do
		if role.GetRoleItemNum(TargetID, i) > 0 then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

function i3200605_Use(MapID, InstanceID, TypeID, TargetID)
    for i = 8200309, 8200316 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, -1, 8, 420)
		end
	end
end

aux.RegisterItemEvent(3200605, 1, "i3200605_Use")
aux.RegisterItemEvent(3200605, 0, "i3200605_CanUse")

function i3200606_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8200409, 8200416 do
		if role.GetRoleItemNum(TargetID, i) > 0 then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

function i3200606_Use(MapID, InstanceID, TypeID, TargetID)
    for i = 8200409, 8200416 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, -1, 8, 420)
		end
	end
end

aux.RegisterItemEvent(3200606, 1, "i3200606_Use")
aux.RegisterItemEvent(3200606, 0, "i3200606_CanUse")

function i3200640_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8200509, 8200516 do
		if role.GetRoleItemNum(TargetID, i) > 0 then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

function i3200640_Use(MapID, InstanceID, TypeID, TargetID)
    for i = 8200509, 8200516 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, -1, 8, 420)
		end
	end
end

aux.RegisterItemEvent(3200640, 1, "i3200640_Use")
aux.RegisterItemEvent(3200640, 0, "i3200640_CanUse")

function i3200641_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	for i = 8200609, 8200616 do
		if role.GetRoleItemNum(TargetID, i) > 0 then
			return bRet, bIgnore
		end
	end
	--返回
	return 32, bIgnore
end

function i3200641_Use(MapID, InstanceID, TypeID, TargetID)
    for i = 8200609, 8200616 do
		if 0 == role.RemoveFromRole(MapID, InstanceID, TargetID, i, 1, 420) then
			role.AddRoleItem(MapID, InstanceID, TargetID, i, 1, -1, 8, 420)
		end
	end
end

aux.RegisterItemEvent(3200641, 1, "i3200641_Use")
aux.RegisterItemEvent(3200641, 0, "i3200641_CanUse")
