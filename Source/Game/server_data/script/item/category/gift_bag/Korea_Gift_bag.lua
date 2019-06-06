


--3305122	[活]泰山的武器箱
--3305123	[活]心妍的武器箱
--3305124	[活]珠光闪烁的宝石盒

--韩国泰山武器宝箱

function I3305123_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	--local z = math.randomseed(os.time())
	local k = math.random(10000)
	if k <=25 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907001)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907001, 1, 3, 8, 420)
	elseif k >=26 and k<=50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907002)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907002, 1, 3, 8, 420)
	elseif k>= 51 and k <= 75 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907003)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907003, 1, 3, 8, 420)
	elseif k >= 76 and k <= 100 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907004)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907004, 1, 3, 8, 420)
	elseif k >= 101 and k <= 200 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8013090, 1, 5, 8, 420)
	elseif k>=201 and k <= 300 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8023090, 1, 5, 8, 420)
	elseif k >= 301 and k <= 400 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8033090, 1, 5, 8, 420)
	elseif k >= 401 and k <= 500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8043090, 1, 5, 8, 420)
	elseif k >= 501 and k <= 750 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8013080, 1, 5, 8, 420)
	elseif k>= 751 and k <= 1000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8023080, 1, 5, 8, 420)
	elseif k >= 1001 and  k <= 1250 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8033080, 1, 5, 8, 420)
	elseif k >= 1251 and k <= 1500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8043080, 1, 5, 8, 420)
	elseif k >= 1501 and k <= 2000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000905, 5, -1, 8, 420)
	elseif k >= 2001 and k <= 3000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2220322, 5, -1, 8, 420)
	elseif k>= 3001 and k <= 5500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000904, 5, -1, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000903, 5, -1, 8, 420)
	end
end


function I3305122_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3305123, 1, "I3305123_GiftBag")
aux.RegisterItemEvent(3305123, 0, "I3305123_CanUseGiftBag")


--韩国心妍武器宝箱


function I3305122_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	--local z = math.randomseed(os.time())
	local k = math.random(10000)
	if k <=25 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907005)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907005, 1, 3, 8, 420)
	elseif k >=26 and k<=50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907006)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907006, 1, 3, 8, 420)
	elseif k>= 51 and k <= 75 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907007)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907007, 1, 3, 8, 420)
	elseif k >= 76 and k <= 100 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, TypeID)
		msg.AddMsgEvent(MsgID, 4, 8907008)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 8907008, 1, 3, 8, 420)
	elseif k >= 101 and k <= 200 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8063090, 1, 5, 8, 420)
	elseif k>=201 and k <= 300 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8073090, 1, 5, 8, 420)
	elseif k >= 301 and k <= 400 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8073090, 1, 5, 8, 420)
	elseif k >= 401 and k <= 500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8053090, 1, 5, 8, 420)
	elseif k >= 501 and k <= 750 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8063080, 1, 5, 8, 420)
	elseif k>= 751 and k <= 1000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8073080, 1, 5, 8, 420)
	elseif k >= 1001 and  k <= 1250 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8073080, 1, 5, 8, 420)
	elseif k >= 1251 and k <= 1500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 8053080, 1, 5, 8, 420)
	elseif k >= 1501 and k <= 2000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000905, 5, -1, 8, 420)
	elseif k >= 2001 and k <= 3000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2220322, 5, -1, 8, 420)
	elseif k>= 3001 and k <= 5500 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000902, 5, -1, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000901, 5, -1, 8, 420)
	end
end

function I3305123_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 7) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3305122, 1, "I3305122_GiftBag")
aux.RegisterItemEvent(3305122, 0, "I3305122_CanUseGiftBag")




--韩国宝石盒

function I3305124_GiftBag(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	--local z = math.randomseed(os.time())
	local k = math.random(10000)
	if k <=50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420411)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420411, 1, -1, 8, 420)
	elseif k >=51 and k<=100 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420451)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420451, 1, -1, 8, 420)
	elseif k>= 101 and k <= 150 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420421)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420421, 1, -1, 8, 420)
	elseif k >= 151 and k <= 200 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420441)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420441, 1, -1, 8, 420)
	elseif k >= 201 and k <= 250 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420461)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420461, 1, -1, 8, 420)
	elseif k>=251 and k <= 300 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 5001)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 3305124)
		msg.AddMsgEvent(MsgID, 4, 1420431)
		msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420431, 1, -1, 8, 420)
	elseif k >= 301 and k <= 1000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1412005, 2, -1, 8, 420)
	elseif k >= 1001 and k <= 2000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420010, 1, -1, 8, 420)
	elseif k >= 2001 and k <= 3000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420034, 1, -1, 8, 420)
	elseif k>= 3001 and k <= 4000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1413006, 2, -1, 8, 420)
	elseif k >= 4001 and  k <= 5000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420014, 1, -1, 8, 420)
	elseif k >= 5001 and k <= 6000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420018, 1, -1, 8, 420)
	elseif k >= 6001 and k <= 7000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420022, 1, -1, 8, 420)
	elseif k >= 7001 and k <= 8000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1420038, 1, -1, 8, 420)
	elseif k>= 8001 and k <= 9000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1412004, 2, -1, 8, 420)
	else
		role.AddRoleItem(MapID, InstanceID, RoleID, 1423005, 3, -1, 8, 420)
	end
end

function I3305124_CanUseGiftBag(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end


aux.RegisterItemEvent(3305124, 1, "I3305124_GiftBag")
aux.RegisterItemEvent(3305124, 0, "I3305124_CanUseGiftBag")
















