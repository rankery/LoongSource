function ShiZhuang_GiftBag_1_1(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120001, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120002, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120003, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120004, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120005, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120006, 1, 1, 8, 420)
	elseif sex == 0 then
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120061, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120062, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120063, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120064, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120065, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120066, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_1_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_2(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120031, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120032, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120033, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120034, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120035, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120036, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120021, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120022, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120023, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120024, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120025, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120026, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_2_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_3(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120051, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120052, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120053, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120054, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120055, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120056, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120071, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120072, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120073, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120074, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120075, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120076, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_3_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_4(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120131, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120132, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120133, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120134, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120135, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120136, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120141, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120142, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120143, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120144, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120145, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120146, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_4_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_5(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120261, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120262, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120263, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120264, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120265, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120037, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120038, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120039, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120040, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120041, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120042, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_5_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_6(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120091, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120092, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120093, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120094, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120095, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120101, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120102, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120103, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120104, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120105, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_6_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_7(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120161, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120162, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120163, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120164, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120165, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120011, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120012, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120013, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120014, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120015, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120016, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_7_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_8(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120151, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120152, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120153, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120154, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120155, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120191, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120192, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120193, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120194, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120195, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_8_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_9(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120111, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120112, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120113, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120114, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120115, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120121, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120122, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120123, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120124, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120125, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_9_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_10(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120271, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120272, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120273, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120274, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120275, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120281, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120282, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120283, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120284, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120285, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_10_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_11(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120181, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120182, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120183, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120184, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120185, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120201, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120202, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120203, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120204, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120205, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_11_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_12(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120251, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120252, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120253, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120254, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120255, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120081, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120082, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120083, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120084, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120085, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120086, 1, 1, 8, 420)
	end
end

function ShiZhuang_GiftBag_1_12_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_13(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120171, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120172, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120173, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120211, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120212, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9120213, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_13_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_14(MapID, InstanceID, TypeID, TargetID)
    local male = {9120001, 9120002, 9120003, 9120004, 9120005, 9120006, 9120031, 9120032, 9120033, 9120034, 9120035, 9120036, 9120051, 9120052, 9120053, 9120054, 9120055, 9120056, 9120131, 9120132, 9120133, 9120134, 9120135, 9120136, 9120261, 9120262, 9120263, 9120264, 9120265, 9120091, 9120092, 9120093, 9120094, 9120095, 9120161, 9120162, 9120163, 9120164, 9120165, 9120151, 9120152, 9120153, 9120154, 9120155, 9120111, 9120112, 9120113, 9120114, 9120115, 9120271, 9120272, 9120273, 9120274, 9120275, 9120181, 9120182, 9120183, 9120184, 9120185, 9120251, 9120252, 9120253, 9120254, 9120255, 9120171, 9120172, 9120173}
    local female = {9120061, 9120062, 9120063, 9120064, 9120065, 9120066, 9120021, 9120022, 9120023, 9120024, 9120025, 9120026, 9120071, 9120072, 9120073, 9120074, 9120075, 9120076, 9120141, 9120142, 9120143, 9120144, 9120145, 9120146, 9120037, 9120038, 9120039, 9120040, 9120041, 9120042, 9120101, 9120102, 9120103, 9120104, 9120105, 9120011, 9120012, 9120013, 9120014, 9120015, 9120016, 9120191, 9120192, 9120193, 9120194, 9120195, 9120121, 9120122, 9120123, 9120124, 9120125, 9120281, 9120282, 9120283, 9120284, 9120285, 9120201, 9120202, 9120203, 9120204, 9120205, 9120081, 9120082, 9120083, 9120084, 9120085, 9120086, 9120211, 9120212, 9120213}
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
function ShiZhuang_GiftBag_1_14_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_15(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140131, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140132, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140133, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140134, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140135, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140141, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140142, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140143, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140144, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140145, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_15_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

function ShiZhuang_GiftBag_1_16(MapID, InstanceID, TypeID, TargetID)
    local sex = role.GetRoleSex(TargetID)
	if sex == 1 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140171, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140172, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140173, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140174, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140175, 1, 1, 8, 420)
	elseif sex == 0 then
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140181, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140182, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140183, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140184, 1, 1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 9140185, 1, 1, 8, 420)
	end
end
function ShiZhuang_GiftBag_1_16_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350801, 1, "ShiZhuang_GiftBag_1_1")
aux.RegisterItemEvent(1350801, 0, "ShiZhuang_GiftBag_1_1_CanUse")
aux.RegisterItemEvent(1350802, 1, "ShiZhuang_GiftBag_1_2")
aux.RegisterItemEvent(1350802, 0, "ShiZhuang_GiftBag_1_2_CanUse")
aux.RegisterItemEvent(1350803, 1, "ShiZhuang_GiftBag_1_3")
aux.RegisterItemEvent(1350803, 0, "ShiZhuang_GiftBag_1_3_CanUse")
aux.RegisterItemEvent(1350804, 1, "ShiZhuang_GiftBag_1_4")
aux.RegisterItemEvent(1350804, 0, "ShiZhuang_GiftBag_1_4_CanUse")
aux.RegisterItemEvent(1350805, 1, "ShiZhuang_GiftBag_1_5")
aux.RegisterItemEvent(1350805, 0, "ShiZhuang_GiftBag_1_5_CanUse")
aux.RegisterItemEvent(1350806, 1, "ShiZhuang_GiftBag_1_6")
aux.RegisterItemEvent(1350806, 0, "ShiZhuang_GiftBag_1_6_CanUse")
aux.RegisterItemEvent(1350807, 1, "ShiZhuang_GiftBag_1_7")
aux.RegisterItemEvent(1350807, 0, "ShiZhuang_GiftBag_1_7_CanUse")
aux.RegisterItemEvent(1350808, 1, "ShiZhuang_GiftBag_1_8")
aux.RegisterItemEvent(1350808, 0, "ShiZhuang_GiftBag_1_8_CanUse")
aux.RegisterItemEvent(1350809, 1, "ShiZhuang_GiftBag_1_9")
aux.RegisterItemEvent(1350809, 0, "ShiZhuang_GiftBag_1_9_CanUse")
aux.RegisterItemEvent(1350810, 1, "ShiZhuang_GiftBag_1_10")
aux.RegisterItemEvent(1350810, 0, "ShiZhuang_GiftBag_1_10_CanUse")
aux.RegisterItemEvent(1350811, 1, "ShiZhuang_GiftBag_1_11")
aux.RegisterItemEvent(1350811, 0, "ShiZhuang_GiftBag_1_11_CanUse")
aux.RegisterItemEvent(1350812, 1, "ShiZhuang_GiftBag_1_12")
aux.RegisterItemEvent(1350812, 0, "ShiZhuang_GiftBag_1_12_CanUse")
aux.RegisterItemEvent(1350813, 1, "ShiZhuang_GiftBag_1_13")
aux.RegisterItemEvent(1350813, 0, "ShiZhuang_GiftBag_1_13_CanUse")
aux.RegisterItemEvent(1350800, 1, "ShiZhuang_GiftBag_1_14")
aux.RegisterItemEvent(1350800, 0, "ShiZhuang_GiftBag_1_14_CanUse")
aux.RegisterItemEvent(1350814, 1, "ShiZhuang_GiftBag_1_15")
aux.RegisterItemEvent(1350814, 0, "ShiZhuang_GiftBag_1_15_CanUse")
aux.RegisterItemEvent(1350815, 1, "ShiZhuang_GiftBag_1_16")
aux.RegisterItemEvent(1350815, 0, "ShiZhuang_GiftBag_1_16_CanUse")
