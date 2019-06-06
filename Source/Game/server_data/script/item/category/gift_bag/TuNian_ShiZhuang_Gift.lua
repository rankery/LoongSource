
--2011年新春时装礼包

--6010832	新年时装礼盒（7天）
--6010833	新年时装礼盒（30天）
--6010834	新年时装礼盒（永久）

--7天制时装礼包

function I6010832_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local Sex =  role.GetRoleSex(RoleID)
	if  Sex == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143201, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143202, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143203, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143204, 1, 4, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143205, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143206, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143207, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143208, 1, 4, 8, 420)
	end
end

function I6010832_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(6010832, 1, "I6010832_GiftBag")
aux.RegisterItemEvent(6010832, 0, "I6010832_CanUseGiftBag")



--30天时装礼包

function I6010833_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local Sex =  role.GetRoleSex(RoleID)
	if  Sex == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143209, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143210, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143211, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143212, 1, 4, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143213, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143214, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143215, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143216, 1, 4, 8, 420)
	end
end

function I6010833_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(6010833, 1, "I6010833_GiftBag")
aux.RegisterItemEvent(6010833, 0, "I6010833_CanUseGiftBag")

--永久时装礼包

function I6010834_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local Sex =  role.GetRoleSex(RoleID)
	if  Sex == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143217, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143218, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143219, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143220, 1, 4, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143221, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143222, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143223, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 9143224, 1, 4, 8, 420)
	end
end

function I6010834_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(6010834, 1, "I6010834_GiftBag")
aux.RegisterItemEvent(6010834, 0, "I6010834_CanUseGiftBag")































