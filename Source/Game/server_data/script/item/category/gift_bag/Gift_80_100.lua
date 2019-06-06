function I1330901_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330902, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1350801, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303006, 5, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303009, 20, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303010, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303011, 20, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303012, 1, -1, 8, 420)
end

function I1330901_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 7) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330901, 1, "I1330901_GiftBag")
aux.RegisterItemEvent(1330901, 0, "I1330901_CanUseGiftBag")

function I1330902_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330903, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200716, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200710, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4008001, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4100109, 1, -1, 8, 420)

end

function I1330902_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330902, 1, "I1330902_GiftBag")
aux.RegisterItemEvent(1330902, 0, "I1330902_CanUseGiftBag")

function I1330903_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330904, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303014, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303017, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303018, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303043, 5, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303117, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1360101, 1, -1, 8, 420)


end

function I1330903_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 7) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330903, 1, "I1330903_GiftBag")
aux.RegisterItemEvent(1330903, 0, "I1330903_CanUseGiftBag")

function I1330904_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330905, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4100103, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4100106, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4100112, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 4009001, 1, -1, 8, 420)


end

function I1330904_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330904, 1, "I1330904_GiftBag")
aux.RegisterItemEvent(1330904, 0, "I1330904_CanUseGiftBag")

function I1330905_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330906, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3300102, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3300103, 5, -1, 8, 420)



end

function I1330905_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330905, 1, "I1330905_GiftBag")
aux.RegisterItemEvent(1330905, 0, "I1330905_CanUseGiftBag")

function I1330906_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330907, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2005202, 10, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1360101, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1350016, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1350017, 1, -1, 8, 420)

end

function I1330906_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330906, 1, "I1330906_GiftBag")
aux.RegisterItemEvent(1330906, 0, "I1330906_CanUseGiftBag")

function I1330907_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330908, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200411, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1340001, 2, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200462, 2, -1, 8, 420)

end

function I1330907_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 4) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330907, 1, "I1330907_GiftBag")
aux.RegisterItemEvent(1330907, 0, "I1330907_CanUseGiftBag")

function I1330908_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330909, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3000008, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3000009, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3000010, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3000011, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1300103, 1, -1, 8, 420)

end

function I1330908_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 6) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330908, 1, "I1330908_GiftBag")
aux.RegisterItemEvent(1330908, 0, "I1330908_CanUseGiftBag")

function I1330909_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330910, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1412003, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1411004, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2005301, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 2005201, 1, -1, 8, 420)

end

function I1330909_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330909, 1, "I1330909_GiftBag")
aux.RegisterItemEvent(1330909, 0, "I1330909_CanUseGiftBag")

function I1330910_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330911, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3200606, 1, -1, 8, 420)

end

function I1330910_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330910, 1, "I1330910_GiftBag")
aux.RegisterItemEvent(1330910, 0, "I1330910_CanUseGiftBag")

function I1330911_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330912, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1230021, 5, -1, 8, 420)

end

function I1330911_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330911, 1, "I1330911_GiftBag")
aux.RegisterItemEvent(1330911, 0, "I1330911_CanUseGiftBag")

function I1330912_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330913, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330323, 1, -1, 8, 420)

end

function I1330912_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 2) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330912, 1, "I1330912_GiftBag")
aux.RegisterItemEvent(1330912, 0, "I1330912_CanUseGiftBag")

function I1330913_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330914, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330321, 1, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330322, 1, -1, 8, 420)

end

function I1330913_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330913, 1, "I1330913_GiftBag")
aux.RegisterItemEvent(1330913, 0, "I1330913_CanUseGiftBag")

function I1330914_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1330802, 1, -1, 8, 420)

end

function I1330914_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330914, 1, "I1330914_GiftBag")
aux.RegisterItemEvent(1330914, 0, "I1330914_CanUseGiftBag")

function I1330315_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800045, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800046, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800047, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800048, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800049, 1, 2, 8, 420)

end

function I1330315_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330315, 1, "I1330315_GiftBag")
aux.RegisterItemEvent(1330315, 0, "I1330315_CanUseGiftBag")

function I1330317_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800045, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800046, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800047, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800048, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800049, 1, 3, 8, 420)

end

function I1330317_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330317, 1, "I1330317_GiftBag")
aux.RegisterItemEvent(1330317, 0, "I1330317_CanUseGiftBag")

function I1330316_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800040, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800041, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800042, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800043, 1, 2, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800044, 1, 2, 8, 420)

end

function I1330316_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330316, 1, "I1330316_GiftBag")
aux.RegisterItemEvent(1330316, 0, "I1330316_CanUseGiftBag")

function I1330318_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800040, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800041, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800042, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800043, 1, 3, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 8800044, 1, 3, 8, 420)

end

function I1330318_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330318, 1, "I1330318_GiftBag")
aux.RegisterItemEvent(1330318, 0, "I1330318_CanUseGiftBag")

function I1330404_GiftBag(MapID, InstanceID, TypeID, TargetID)

	role.AddQuest(TargetID, 210)

end
aux.RegisterItemEvent(1330404, 1, "I1330404_GiftBag")

function I1330323_GiftBag(MapID, InstanceID, TypeID, TargetID)

	role.AddQuest(TargetID, 215)

end
aux.RegisterItemEvent(1330323, 1, "I1330323_GiftBag")

function q216_OnComplete(MapID, InstanceID, QuestID, OwnerID, EnderID)

	role.AddRoleGiftPoints(MapID, InstanceID, OwnerID, 1888)

end
aux.RegisterQuestEvent(216, 1, "q216_OnComplete")

function q217_OnComplete(MapID, InstanceID, QuestID, OwnerID, EnderID)

	role.AddRoleGiftPoints(MapID, InstanceID, OwnerID, 5888)

end
aux.RegisterQuestEvent(217, 1, "q217_OnComplete")

function q218_OnComplete(MapID, InstanceID, QuestID, OwnerID, EnderID)

	role.AddRoleGiftPoints(MapID, InstanceID, OwnerID, 8888)

end
aux.RegisterQuestEvent(218, 1, "q218_OnComplete")

function I1330112_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1230018, 5, -1, 8, 420)

end

function I1330112_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330112, 1, "I1330112_GiftBag")
aux.RegisterItemEvent(1330112, 0, "I1330112_CanUseGiftBag")

function I1330212_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	role.AddRoleItem(MapID, InstanceID, TargetID, 1230019, 5, -1, 8, 420)

end

function I1330212_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1330212, 1, "I1330212_GiftBag")
aux.RegisterItemEvent(1330212, 0, "I1330212_CanUseGiftBag")
