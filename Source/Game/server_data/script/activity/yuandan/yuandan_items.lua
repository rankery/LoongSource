--[[小福袋
function i1350410_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i1350410_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local i = math.random(100)
	local level = role.GetRoleLevel(MapID, InstanceID, TargetID)

	level = math.floor(level/10)

	if level > 10 then
	   level = 10
	end

	role.AddRoleSilver(MapID, InstanceID, TargetID, math.random(500,1000), 102)

	if i>96 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340002, 1, -1, 8, 420)
	elseif i>56 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350408, 1, -1, 8, 420)
	elseif i>28 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000001+level, 5, -1, 8, 420)
	elseif i>0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000101+level, 5, -1, 8, 420)
	end
end
--注册
aux.RegisterItemEvent(1350410, 1, "i1350410_QuestUsable")
aux.RegisterItemEvent(1350410, 0, "i1350410_CanUse")

--大福袋
function i1350411_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i1350411_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local i = math.random(100)

	role.AddRoleSilver(MapID, InstanceID, TargetID, math.random(1200,2400), 102)

	if i>92 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340001, 1, -1, 8, 420)
	elseif i>60 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350408, 1, -1, 8, 420)
	elseif i>0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350409, 1, -1, 8, 420)
	end

	role.AddRoleItem(MapID, InstanceID, TargetID, 1350401, 1, -1, 8, 420)
end
--注册
aux.RegisterItemEvent(1350411, 1, "i1350411_QuestUsable")
aux.RegisterItemEvent(1350411, 0, "i1350411_CanUse")

--7天时装
function i1350413_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i1350413_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local sex = role.GetRoleSex(TargetID)
	if sex == 0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140031, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140032, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140033, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140034, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140035, 1, 1, 8, 420)
	elseif sex == 1 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140021, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140022, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140023, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140024, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140025, 1, 1, 8, 420)
	end
end
--注册
aux.RegisterItemEvent(1350413, 1, "i1350413_QuestUsable")
aux.RegisterItemEvent(1350413, 0, "i1350413_CanUse")

--永久时装
function i1350414_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i1350414_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local sex = role.GetRoleSex(TargetID)
	if sex == 0 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140011, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140012, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140013, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140014, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140015, 1, 1, 8, 420)
	elseif sex == 1 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140001, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140002, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140003, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140004, 1, 1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 9140005, 1, 1, 8, 420)
	end
end
--注册
aux.RegisterItemEvent(1350414, 1, "i1350414_QuestUsable")
aux.RegisterItemEvent(1350414, 0, "i1350414_CanUse")
]]