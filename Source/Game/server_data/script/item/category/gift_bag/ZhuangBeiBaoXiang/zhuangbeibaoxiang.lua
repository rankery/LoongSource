function WuQiBaoXiang_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

function FangJuBaoXiang_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 5) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(1350916, 0, "WuQiBaoXiang_CanUse")
aux.RegisterItemEvent(1350917, 0, "WuQiBaoXiang_CanUse")
aux.RegisterItemEvent(1350918, 0, "WuQiBaoXiang_CanUse")
aux.RegisterItemEvent(1350919, 0, "WuQiBaoXiang_CanUse")
aux.RegisterItemEvent(1350920, 0, "FangJuBaoXiang_CanUse")
aux.RegisterItemEvent(1350921, 0, "FangJuBaoXiang_CanUse")
aux.RegisterItemEvent(1350922, 0, "FangJuBaoXiang_CanUse")
aux.RegisterItemEvent(1350923, 0, "FangJuBaoXiang_CanUse")

function ZhuangBeiBaoXiang_Broad(TargetID, StringID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, StringID)
	msg.AddMsgEvent(MsgID, 2, TargetID)
	msg.DispatchWorldMsgEvent(MsgID)

end

function I1350916_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=122 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901002, 1, 4, 8, 420)
	elseif k<=244 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901003, 1, 4, 8, 420)
	elseif k<=366 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901004, 1, 4, 8, 420)
	elseif k<=488 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901005, 1, 4, 8, 420)
	elseif k<=494 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901002, 1, 5, 8, 420)
	elseif k<=500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901003, 1, 5, 8, 420)
	elseif k<=515 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901004, 1, 5, 8, 420)
	elseif k<=530 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901005, 1, 5, 8, 420)
	elseif k<=641 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800001, 1, 4, 8, 420)
	elseif k<=752 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800003, 1, 4, 8, 420)
	elseif k<=863 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800005, 1, 4, 8, 420)
	elseif k<=974 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800007, 1, 4, 8, 420)
	elseif k<=986 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800001, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800003, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800005, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800007, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	end
end

function I1350917_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=154 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901006, 1, 4, 8, 420)
	elseif k<=308 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901007, 1, 4, 8, 420)
	elseif k<=462 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901008, 1, 4, 8, 420)
	elseif k<=471 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901006, 1, 5, 8, 420)
	elseif k<=526 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901007, 1, 5, 8, 420)
	elseif k<=554 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8901008, 1, 5, 8, 420)
	elseif k<=688 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800009, 1, 4, 8, 420)
	elseif k<=822 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800011, 1, 4, 8, 420)
	elseif k<=957 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800013, 1, 4, 8, 420)
	elseif k<=960 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800009, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800011, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8800013, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	end
end

function I1350918_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=122 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902001, 1, 4, 8, 420)
	elseif k<=244 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902002, 1, 4, 8, 420)
	elseif k<=366 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902003, 1, 4, 8, 420)
	elseif k<=488 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902004, 1, 4, 8, 420)
	elseif k<=494 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8012050, 1, 5, 8, 420)
	elseif k<=500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8022050, 1, 5, 8, 420)
	elseif k<=515 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8032050, 1, 5, 8, 420)
	elseif k<=530 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8042050, 1, 5, 8, 420)
	elseif k<=641 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902001, 1, 4, 8, 420)
	elseif k<=752 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902002, 1, 4, 8, 420)
	elseif k<=863 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902003, 1, 4, 8, 420)
	elseif k<=974 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902004, 1, 4, 8, 420)
	elseif k<=986 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8012050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8022050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8032050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8042050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	end
end

function I1350919_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=154 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902005, 1, 4, 8, 420)
	elseif k<=308 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902006, 1, 4, 8, 420)
	elseif k<=462 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902007, 1, 4, 8, 420)
	elseif k<=471 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8052050, 1, 5, 8, 420)
	elseif k<=526 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8062050, 1, 5, 8, 420)
	elseif k<=554 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8072050, 1, 5, 8, 420)
	elseif k<=688 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902005, 1, 4, 8, 420)
	elseif k<=822 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902006, 1, 4, 8, 420)
	elseif k<=957 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8902007, 1, 4, 8, 420)
	elseif k<=960 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8052050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8062050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8072050, 1, 5, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		ZhuangBeiBaoXiang_Broad(TargetID, 287)
	end
end

function I1350920_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=158 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083030, 1, 3, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093030, 1, 3, 8, 420)
	elseif k<=478 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103030, 1, 3, 8, 420)
	elseif k<=634 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113030, 1, 3, 8, 420)
	elseif k<=792 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123030, 1, 3, 8, 420)
	elseif k<=826 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083030, 1, 4, 8, 420)
	elseif k<=866 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093030, 1, 4, 8, 420)
	elseif k<=902 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103030, 1, 4, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113030, 1, 4, 8, 420)
	elseif k<=984 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123030, 1, 4, 8, 420)
	elseif k<=987 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=990 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=993 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123030, 1, 4, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 289)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 290)
	end
end

function I1350921_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=158 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083050, 1, 3, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093050, 1, 3, 8, 420)
	elseif k<=478 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103050, 1, 3, 8, 420)
	elseif k<=634 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113050, 1, 3, 8, 420)
	elseif k<=792 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123050, 1, 3, 8, 420)
	elseif k<=826 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083050, 1, 4, 8, 420)
	elseif k<=866 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093050, 1, 4, 8, 420)
	elseif k<=902 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103050, 1, 4, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113050, 1, 4, 8, 420)
	elseif k<=984 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123050, 1, 4, 8, 420)
	elseif k<=987 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=990 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=993 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123050, 1, 4, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 289)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8083050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8093050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8103050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8113050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8123050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 290)
	end
end

function I1350922_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=158 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082030, 1, 3, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092030, 1, 3, 8, 420)
	elseif k<=478 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102030, 1, 3, 8, 420)
	elseif k<=634 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112030, 1, 3, 8, 420)
	elseif k<=792 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122030, 1, 3, 8, 420)
	elseif k<=826 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082030, 1, 4, 8, 420)
	elseif k<=866 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092030, 1, 4, 8, 420)
	elseif k<=902 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102030, 1, 4, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112030, 1, 4, 8, 420)
	elseif k<=984 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122030, 1, 4, 8, 420)
	elseif k<=987 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=990 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=993 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112030, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122030, 1, 4, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 289)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112030, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122030, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 290)
	end
end

function I1350923_GiftBag(MapID, InstanceID, TypeID, TargetID)

	local k = math.random(1000)

	if k<=158 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082050, 1, 3, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092050, 1, 3, 8, 420)
	elseif k<=478 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102050, 1, 3, 8, 420)
	elseif k<=634 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112050, 1, 3, 8, 420)
	elseif k<=792 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122050, 1, 3, 8, 420)
	elseif k<=826 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082050, 1, 4, 8, 420)
	elseif k<=866 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092050, 1, 4, 8, 420)
	elseif k<=902 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102050, 1, 4, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112050, 1, 4, 8, 420)
	elseif k<=984 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122050, 1, 4, 8, 420)
	elseif k<=987 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=990 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=993 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=995 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=998 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 288)
	elseif k<=999 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112050, 1, 4, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122050, 1, 4, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 289)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8082050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8092050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8102050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8112050, 1, 5, 8, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 8122050, 1, 5, 8, 420)
		ZhuangBeiBaoXiang_Broad(TargetID, 290)
	end
end

aux.RegisterItemEvent(1350916, 1, "I1350916_GiftBag")
aux.RegisterItemEvent(1350917, 1, "I1350917_GiftBag")
aux.RegisterItemEvent(1350918, 1, "I1350918_GiftBag")
aux.RegisterItemEvent(1350919, 1, "I1350919_GiftBag")
aux.RegisterItemEvent(1350920, 1, "I1350920_GiftBag")
aux.RegisterItemEvent(1350921, 1, "I1350921_GiftBag")
aux.RegisterItemEvent(1350922, 1, "I1350922_GiftBag")
aux.RegisterItemEvent(1350923, 1, "I1350923_GiftBag")