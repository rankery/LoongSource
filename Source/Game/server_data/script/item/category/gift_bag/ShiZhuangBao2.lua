function ShiZhuang_GiftBag_7_1(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130001, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130002, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130003, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130004, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130005, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130006, 1, 1, 8, 420)
	elseif sex == 0 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130061, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130062, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130063, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130064, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130065, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130066, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_1_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_2(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130031, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130032, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130033, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130034, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130035, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130036, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130021, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130022, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130023, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130024, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130025, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130026, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_2_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_3(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130051, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130052, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130053, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130054, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130055, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130056, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130071, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130072, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130073, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130074, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130075, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130076, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_3_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_4(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130131, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130132, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130133, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130134, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130135, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130136, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130141, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130142, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130143, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130144, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130145, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130146, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_4_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_5(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130261, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130262, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130263, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130264, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130265, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130037, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130038, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130039, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130040, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130041, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130042, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_5_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_6(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130091, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130092, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130093, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130094, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130095, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130101, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130102, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130103, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130104, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130105, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_6_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_7(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130161, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130162, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130163, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130164, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130165, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130011, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130012, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130013, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130014, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130015, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130016, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_7_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_8(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130151, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130152, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130153, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130154, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130155, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130191, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130192, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130193, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130194, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130195, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_8_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_9(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130111, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130112, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130113, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130114, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130115, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130121, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130122, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130123, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130124, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130125, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_9_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_10(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130271, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130272, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130273, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130274, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130275, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130281, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130282, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130283, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130284, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130285, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_10_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_11(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130181, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130182, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130183, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130184, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130185, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130201, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130202, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130203, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130204, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130205, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_11_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_12(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130251, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130252, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130253, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130254, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130255, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130081, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130082, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130083, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130084, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130085, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130086, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_12_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_13(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130171, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130172, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130173, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130211, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130212, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9130213, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_13_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_14(MapID, InstanceID, TypeID, TargetID)
    local male = {9130001, 9130002, 9130003, 9130004, 9130005, 9130006, 9130031, 9130032, 9130033, 9130034, 9130035, 9130036, 9130051, 9130052, 9130053, 9130054, 9130055, 9130056, 9130131, 9130132, 9130133, 9130134, 9130135, 9130136, 9130261, 9130262, 9130263, 9130264, 9130265, 9130091, 9130092, 9130093, 9130094, 9130095, 9130161, 9130162, 9130163, 9130164, 9130165, 9130151, 9130152, 9130153, 9130154, 9130155, 9130111, 9130112, 9130113, 9130114, 9130115, 9130271, 9130272, 9130273, 9130274, 9130275, 9130181, 9130182, 9130183, 9130184, 9130185, 9130251, 9130252, 9130253, 9130254, 9130255, 9130171, 9130172, 9130173}
    local female = { 9130061, 9130062, 9130063, 9130064, 9130065, 9130066, 9130021, 9130022, 9130023, 9130024, 9130025, 9130026, 9130071, 9130072, 9130073, 9130074, 9130075, 9130076, 9130141, 9130142, 9130143, 9130144, 9130145, 9130146, 9130037, 9130038, 9130039, 9130040, 9130041, 9130042, 9130101, 9130102, 9130103, 9130104, 9130105, 9130011, 9130012, 9130013, 9130014, 9130015, 9130016, 9130191, 9130192, 9130193, 9130194, 9130195, 9130121, 9130122, 9130123, 9130124, 9130125, 9130281, 9130282, 9130283, 9130284, 9130285, 9130201, 9130202, 9130203, 9130204, 9130205, 9130081, 9130082, 9130083, 9130084, 9130085, 9130086, 9130211, 9130212, 9130213}
	local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	    local k = math.random(1,67)
		if male[k] ~= nil then
	        role.AddRoleItem(MapID, InstanceID, TargetID, male[k], 1, 1, 8, 420)
		else
		    role.AddRoleItem(MapID, InstanceID, TargetID, male[1], 1, 1, 8, 420)
		end
	elseif sex == 0 then
	    local k = math.random(1,70)
		if female[k] ~= nil then
	        role.AddRoleItem(MapID, InstanceID, TargetID, female[k], 1, 1, 8, 420)
		else
		    role.AddRoleItem(MapID, InstanceID, TargetID, female[1], 1, 1, 8, 420)
		end
	end
end
function ShiZhuang_GiftBag_7_14_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_15(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140211, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140212, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140213, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140214, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140215, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140221, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140222, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140223, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140224, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140225, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_15_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_16(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140231, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140232, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140233, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140234, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140235, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140241, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140242, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140243, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140244, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140245, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_16_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_17(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140151, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140152, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140153, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140154, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140155, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140161, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140162, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140163, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140164, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140165, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_17_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_7_18(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140191, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140192, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140193, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140194, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140195, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140201, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140202, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140203, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140204, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140205, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_7_18_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350901, 1, "ShiZhuang_GiftBag_7_1")
aux.RegisterItemEvent(1350901, 0, "ShiZhuang_GiftBag_7_1_CanUse")
aux.RegisterItemEvent(1350902, 1, "ShiZhuang_GiftBag_7_2")
aux.RegisterItemEvent(1350902, 0, "ShiZhuang_GiftBag_7_2_CanUse")
aux.RegisterItemEvent(1350903, 1, "ShiZhuang_GiftBag_7_3")
aux.RegisterItemEvent(1350903, 0, "ShiZhuang_GiftBag_7_3_CanUse")
aux.RegisterItemEvent(1350904, 1, "ShiZhuang_GiftBag_7_4")
aux.RegisterItemEvent(1350904, 0, "ShiZhuang_GiftBag_7_4_CanUse")
aux.RegisterItemEvent(1350905, 1, "ShiZhuang_GiftBag_7_5")
aux.RegisterItemEvent(1350905, 0, "ShiZhuang_GiftBag_7_5_CanUse")
aux.RegisterItemEvent(1350906, 1, "ShiZhuang_GiftBag_7_6")
aux.RegisterItemEvent(1350906, 0, "ShiZhuang_GiftBag_7_6_CanUse")
aux.RegisterItemEvent(1350907, 1, "ShiZhuang_GiftBag_7_7")
aux.RegisterItemEvent(1350907, 0, "ShiZhuang_GiftBag_7_7_CanUse")
aux.RegisterItemEvent(1350908, 1, "ShiZhuang_GiftBag_7_8")
aux.RegisterItemEvent(1350908, 0, "ShiZhuang_GiftBag_7_8_CanUse")
aux.RegisterItemEvent(1350909, 1, "ShiZhuang_GiftBag_7_9")
aux.RegisterItemEvent(1350909, 0, "ShiZhuang_GiftBag_7_9_CanUse")
aux.RegisterItemEvent(1350910, 1, "ShiZhuang_GiftBag_7_10")
aux.RegisterItemEvent(1350910, 0, "ShiZhuang_GiftBag_7_10_CanUse")
aux.RegisterItemEvent(1350911, 1, "ShiZhuang_GiftBag_7_11")
aux.RegisterItemEvent(1350911, 0, "ShiZhuang_GiftBag_7_11_CanUse")
aux.RegisterItemEvent(1350912, 1, "ShiZhuang_GiftBag_7_12")
aux.RegisterItemEvent(1350912, 0, "ShiZhuang_GiftBag_7_12_CanUse")
aux.RegisterItemEvent(1350913, 1, "ShiZhuang_GiftBag_7_13")
aux.RegisterItemEvent(1350913, 0, "ShiZhuang_GiftBag_7_13_CanUse")
aux.RegisterItemEvent(1350900, 1, "ShiZhuang_GiftBag_7_14")
aux.RegisterItemEvent(1350900, 0, "ShiZhuang_GiftBag_7_14_CanUse")
aux.RegisterItemEvent(1350914, 1, "ShiZhuang_GiftBag_7_15")
aux.RegisterItemEvent(1350914, 0, "ShiZhuang_GiftBag_7_15_CanUse")
aux.RegisterItemEvent(1350915, 1, "ShiZhuang_GiftBag_7_16")
aux.RegisterItemEvent(1350915, 0, "ShiZhuang_GiftBag_7_16_CanUse")
aux.RegisterItemEvent(3303152, 1, "ShiZhuang_GiftBag_7_17")
aux.RegisterItemEvent(3303152, 0, "ShiZhuang_GiftBag_7_17_CanUse")
aux.RegisterItemEvent(3303153, 1, "ShiZhuang_GiftBag_7_18")
aux.RegisterItemEvent(3303153, 0, "ShiZhuang_GiftBag_7_18_CanUse")
