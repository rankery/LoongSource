function korea_baoxiang_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore

end

aux.RegisterItemEvent(3305201, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305202, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305203, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305204, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305205, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305206, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305207, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305208, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305209, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305210, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305211, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305212, 0, "korea_baoxiang_CanUse")
aux.RegisterItemEvent(3305213, 0, "korea_baoxiang_CanUse")


function I3305201_GiftBag(MapID, InstanceID, TypeID, TargetID)     --武器材料箱子

	local k = math.random(1000)

	if k<=200 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000013, 1, 1, 8, 420)
	elseif k<=300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000018, 1, 1, 8, 420)
	elseif k<=500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000003, 1, 1, 8, 420)
	elseif k<=600 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000008, 1, 1, 8, 420)
	elseif k<=650 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000023, 1, 1, 8, 420)
	elseif k<=725 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000014, 1, 1, 8, 420)
	elseif k<=765 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000019, 1, 1, 8, 420)
	elseif k<=840 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000004, 1, 1, 8, 420)
	elseif k<=880 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000009, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000024, 1, 1, 8, 420)
	elseif k<=915 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000015, 1, 1, 8, 420)
	elseif k<=923 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000020, 1, 1, 8, 420)
	elseif k<=938 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000005, 1, 1, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000010, 1, 1, 8, 420)
	elseif k<=950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000025, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1220021, 1, 1, 8, 420)

	end
end

function I3305202_GiftBag(MapID, InstanceID, TypeID, TargetID)		--护甲材料箱子

	local k = math.random(1000)

	if k<=200 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000208, 1, 1, 8, 420)
	elseif k<=300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000203, 1, 1, 8, 420)
	elseif k<=500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000213, 1, 1, 8, 420)
	elseif k<=600 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000218, 1, 1, 8, 420)
	elseif k<=650 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000223, 1, 1, 8, 420)
	elseif k<=725 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000209, 1, 1, 8, 420)
	elseif k<=765 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000204, 1, 1, 8, 420)
	elseif k<=840 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000214, 1, 1, 8, 420)
	elseif k<=880 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000219, 1, 1, 8, 420)
	elseif k<=900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000224, 1, 1, 8, 420)
	elseif k<=915 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000205, 1, 1, 8, 420)
	elseif k<=923 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000020, 1, 1, 8, 420)
	elseif k<=938 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000215, 1, 1, 8, 420)
	elseif k<=946 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000220, 1, 1, 8, 420)
	elseif k<=950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000225, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1220021, 1, 1, 8, 420)

	end
end

function I3305203_GiftBag(MapID, InstanceID, TypeID, TargetID)		--生活必需品宝箱

	local k = math.random(1000)

	if k<=70 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1140002, 5, 1, 8, 420)
	elseif k<=130 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000327, 1, 1, 8, 420)
	elseif k<=190 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000334, 1, 1, 8, 420)
	elseif k<=260 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1320023, 10, 1, 8, 420)
	elseif k<=330 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340012, 1, 1, 8, 420)
	elseif k<=410 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340015, 5, 1, 8, 420)
	elseif k<=470 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200710, 1, 1, 8, 420)
	elseif k<=530 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360001, 30, 1, 8, 420)
	elseif k<=610 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340009, 10, 1, 8, 420)
	elseif k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1340008, 10, 1, 8, 420)
	elseif k<=780 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360103, 10, 1, 8, 420)
	elseif k<=860 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005301, 10, 1, 8, 420)
	elseif k<=960 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2220401, 3, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005201, 5, 1, 8, 420)

	end
end

function I3305204_GiftBag(MapID, InstanceID, TypeID, TargetID)		--勇士男装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9311001, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9311002, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9311003, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9311004, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9311005, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9221036, 1, 1, 8, 420)

	end
end

function I3305205_GiftBag(MapID, InstanceID, TypeID, TargetID)		--勇士女装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9341006, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9341007, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9341008, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9341009, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9341010, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9221042, 1, 1, 8, 420)

	end
end

function I3305206_GiftBag(MapID, InstanceID, TypeID, TargetID)		--结实的男装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9312001, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9312002, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9312003, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9312004, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9312005, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9222036, 1, 1, 8, 420)

	end
end

function I3305207_GiftBag(MapID, InstanceID, TypeID, TargetID)		--结实的女装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9342006, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9342007, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9342008, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9342009, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9342010, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9222042, 1, 1, 8, 420)

	end
end

function I3305208_GiftBag(MapID, InstanceID, TypeID, TargetID)		--耀眼的男装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9313001, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9313002, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9313003, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9313004, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9313005, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9223036, 1, 1, 8, 420)

	end
end

function I3305209_GiftBag(MapID, InstanceID, TypeID, TargetID)		--耀眼的女装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9343006, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9343007, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9343008, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9343009, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9343010, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9223042, 1, 1, 8, 420)

	end
end

function I3305210_GiftBag(MapID, InstanceID, TypeID, TargetID)		--纯洁的男装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9314001, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9314002, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9314003, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9314004, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9314005, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9224036, 1, 1, 8, 420)

	end
end

function I3305211_GiftBag(MapID, InstanceID, TypeID, TargetID)		--纯洁的女装宝箱

	local k = math.random(1000)

	if k<=150 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9344006, 1, 1, 8, 420)
	elseif k<=320 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9344007, 1, 1, 8, 420)
	elseif k<=490 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9344008, 1, 1, 8, 420)
	elseif k<=660 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9344009, 1, 1, 8, 420)
	elseif k<=830 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9344010, 1, 1, 8, 420)
	elseif k<=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 9224042, 1, 1, 8, 420)

	end
end

function I3305212_GiftBag(MapID, InstanceID, TypeID, TargetID)		--超级神器外功材料宝箱

	local k = math.random(10000)

	if k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000910, 3, 1, 8, 420)
	elseif k<=2100 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000906, 4, 1, 8, 420)
	elseif k<=5300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000907, 4, 1, 8, 420)
	elseif k<=5900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000905, 3, 1, 8, 420)
	elseif k<=7100 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000904, 4, 1, 8, 420)
	elseif k<=9500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000903, 4, 1, 8, 420)
	elseif k<=9625 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907001, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907001)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=9750 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907002, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907002)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=9875 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907003, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907003)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=10000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907004, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907004)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

function I3305213_GiftBag(MapID, InstanceID, TypeID, TargetID)		--超级神器内功材料宝箱

	local k = math.random(10000)

	if k<=700 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000910, 3, 1, 8, 420)
	elseif k<=2100 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000909, 4, 1, 8, 420)
	elseif k<=5300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000908, 4, 1, 8, 420)
	elseif k<=5900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000905, 3, 1, 8, 420)
	elseif k<=7100 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000902, 4, 1, 8, 420)
	elseif k<=9500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000901, 4, 1, 8, 420)
	elseif k<=9625 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907005, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907005)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=9750 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907006, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907006)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=9875 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907007, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907007)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k<=10000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 8907008, 1, 3, 8, 420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907008)
		msg.DispatchWorldMsgEvent(MsgID)

	end
end

aux.RegisterItemEvent(3305201, 1, "I3305201_GiftBag")
aux.RegisterItemEvent(3305202, 1, "I3305202_GiftBag")
aux.RegisterItemEvent(3305203, 1, "I3305203_GiftBag")
aux.RegisterItemEvent(3305204, 1, "I3305204_GiftBag")
aux.RegisterItemEvent(3305205, 1, "I3305205_GiftBag")
aux.RegisterItemEvent(3305206, 1, "I3305206_GiftBag")
aux.RegisterItemEvent(3305207, 1, "I3305207_GiftBag")
aux.RegisterItemEvent(3305208, 1, "I3305208_GiftBag")
aux.RegisterItemEvent(3305209, 1, "I3305209_GiftBag")
aux.RegisterItemEvent(3305210, 1, "I3305210_GiftBag")
aux.RegisterItemEvent(3305211, 1, "I3305211_GiftBag")
aux.RegisterItemEvent(3305212, 1, "I3305212_GiftBag")
aux.RegisterItemEvent(3305213, 1, "I3305213_GiftBag")


