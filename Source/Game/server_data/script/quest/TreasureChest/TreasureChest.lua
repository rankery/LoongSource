Treasure_BeginRate 		= 	0			-- 起始掉率
Treasure_RoleRate		= 	1			-- 角色节点掉率
Treasure_ServerRate		= 	2			-- 服务器节点掉率
Treasure_NormalRate		=	3			-- 普通掉率

-- 设置随机种子
math.randomseed(os.time())

-- 得到当前宝箱内物品掉率
function Treasure_GetRate(MapID, InstanceID, RoleID)

	local RoleChestSum, ServerChestSum = role.GetTreasureSum(MapID, InstanceID, RoleID)

	if ServerChestSum == 1000 then
		return Treasure_ServerRate

	elseif RoleChestSum == 100 then
		return Treasure_RoleRate

	elseif RoleChestSum <= 5 then
		return Treasure_BeginRate

	else
		return Treasure_NormalRate
	end
end

-- 开启宝箱
function Treasure_OpenChest(MapID, InstanceID, RoleID, ChestID, KeyID)

	local bOpened = false
	local ErrorCode = 0

	if role.CanOpenChest(MapID, InstanceID, RoleID, ChestID, KeyID) == true then
		if role.GetBagFreeSize(RoleID) == 0 then	-- 背包空间不够
			bOpened = false
			ErrorCode = -100
		else
			bOpened = true

			-- 行囊中的宝箱和钥匙消耗1
			role.RemoveFromRole(MapID, InstanceID, RoleID, ChestID, 1, 300)
			role.RemoveFromRole(MapID, InstanceID, RoleID, KeyID, 1, 300)
			-- 如果玩家掉线
			local ret = role.IsRoleOnLine(MapID, InstanceID, RoleID)
			if ret == 0 then
				ErrorCode = -301
			else
				ErrorCode = 0
				-- 开宝箱计数加1
				role.IncTreasureSum(MapID, InstanceID, RoleID)
				local Rate = Treasure_GetRate(MapID, InstanceID, RoleID)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Rate"], Rate)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 1)
			end
		end

	else
		bOpened = false
	end

	-- 发送消息通知客户端
	role.SendChestMsg(MapID, InstanceID, RoleID, ChestID, "OpenChest", bOpened, false, 0, 0, ErrorCode)
end


-- 停止宝箱产生随机物品
function Treasure_StopChest(MapID, InstanceID, RoleID, ChestID, KeyID)
	local ErrorCode = 0


	-- 随机出0到1之间的数，必须大于0
	local RandNum = math.random() + 0.0000001
	local Rate = role.GetRoleScriptData(RoleID, 1, RoleDataType["Chest_Rate"])
	local Step = role.GetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"])

	-- 如果玩家掉线
	local ret = role.IsRoleOnLine(MapID, InstanceID, RoleID)
	if ret == 0 then
		ErrorCode = -301
	else
		ErrorCode = 0
	end

	-- 得到随机产生的物品id和数量
	local ItemID, Num = role.GetChestItem(MapID, InstanceID, RoleID, ChestID, Rate, RandNum)
	if ItemID == nil or Num == nil or (Step ~= 1 and Step ~= 3) then
		ErrorCode = -2
	else
	    if Step == 1 then
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 2)
		elseif Step == 3 then
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 4)
		else
		   ErrorCode = -2
		end
	end

	-- 发送消息给客户端
	role.SendChestMsg(MapID, InstanceID, RoleID, ChestID, "StopChest", true, false, ItemID, Num, ErrorCode)

end

-- 再试一次重新选取物品
function Treasure_AgainChest(MapID, InstanceID, RoleID)
	local ErrorCode = 0
	local Step = role.GetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"])
	-- 50%几率损坏
	local rand = math.random(0, 1)
    local bDestroy = true
	-- 如果玩家掉线
	local ret = role.IsRoleOnLine(MapID, InstanceID, RoleID)
	if ret == 0 or Step ~= 2 then
		ErrorCode = -301
	else
		ErrorCode = 0
    	if rand == 0 then
            bDestroy = false
            role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 3)
        else
            bDestroy = true
            role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 5)
        end
	end




	-- 发送消息给客户端
	role.SendChestMsg(MapID, InstanceID, RoleID, ChestID, "AgainChest", true, bDestroy, 0, 0, ErrorCode)

end

-- 得到最后物品，结束开启宝箱
function Treasure_GetItem(MapID, InstanceID, RoleID, ChestID, ItemID, ItemNum)
	local ErrorCode = 0
	-- 如果玩家掉线
	local ret = role.IsRoleOnLine(MapID, InstanceID, RoleID)
	local Step = role.GetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"])
	if ret == 0 or (Step ~= 2 and Step ~= 4)then
		ErrorCode = -301
	else
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["Chest_Step"], 5)
		ErrorCode = 0
		role.AddRoleItem(MapID, InstanceID, RoleID, ItemID, ItemNum, -1, 3, 104)
		local needbroadcast = role.ItemNeedBroadcast(ItemID)
		if needbroadcast == 1 then
			-- 广播物品
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 280)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, ChestID)
			msg.AddMsgEvent(MsgID, 4, ItemID)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	end

	-- 发送消息给客户端
	role.SendChestMsg(MapID, InstanceID, RoleID, ChestID, "GetItem", true, false, ItemID, ItemNum, ErrorCode)

end

aux.RegisterRoleEvent(7, "Treasure_OpenChest")
aux.RegisterRoleEvent(8, "Treasure_StopChest")
aux.RegisterRoleEvent(9, "Treasure_AgainChest")
aux.RegisterRoleEvent(10, "Treasure_GetItem")

