--[[
龙鱼

游戏币	2000银	100%	必定获得

以下随机获得一个。
风火石	1	20%
精巧乾天图样	1	15%
精巧离火图样	1	15%
精巧震雷图样	1	15%
精巧坎水图样	1	15%
精巧巽风图样	1	15%
战斗符咒（1小时）	1	5%


黄金龙鱼

游戏币	1金	100%	必定获得

以下随机获得一个。
[氏]日月珠	5	30%
[氏]太极图	5	30%
[氏]阴阳印	5	30%
风火石	5	30%
筑基丹	1	5%
普通合成符?壹	1	3%
普通合成符?贰	1	1%
战斗符咒（8小时）	1	1%



	]]


function I3200712_GiftBag(MapID, InstanceID, TypeID, TargetID)
	local a = math.random(1000)
	local Temp = 0

	local FishingLevel = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	if FishingLevel > 100 then
		FishingLevel = 100
	end
	if FishingLevel < 0 then
		FishingLevel = 1
	end
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],FishingLevel)

	role.AddRoleSilver(MapID, InstanceID, TargetID, 2000, 102)

	if a <= 200 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3200464, 1, -1, 8, 420)
	elseif a <= 350 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005003, 1, -1, 8, 420)
	elseif a <= 500 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005023, 1, -1, 8, 420)
	elseif a <= 650 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005033, 1, -1, 8, 420)
	elseif a <= 800 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005043, 1, -1, 8, 420)
	elseif a <= 950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2005053, 1, -1, 8, 420)
	elseif a <=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360123, 1, -1, 8, 420)
		Temp= 1360123
	end

	if Temp ~= 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100111)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 3200712)
		msg.AddMsgEvent(MsgID, 4, Temp)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end
end

function I3200712_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
	return 40, false
	end
	return 0, false
end

aux.RegisterItemEvent(3200712, 1, "I3200712_GiftBag")
aux.RegisterItemEvent(3200712, 0, "I3200712_CanUseGiftBag")


function I3200713_GiftBag(MapID, InstanceID, TypeID, TargetID)
	local a = math.random(1000)
	local Temp = 0

	local FishingLevel = role.GetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"])
	if FishingLevel > 100 then
		FishingLevel = 100
	end
	if FishingLevel < 0 then
		FishingLevel = 1
	end
	role.SetRoleScriptData(TargetID, 1, RoleDataType["FishingLevel"],FishingLevel)

	role.AddRoleSilver(MapID, InstanceID, TargetID, 10000, 102)

	if a <= 300 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2615304, 1, -1, 8, 420)
	elseif a <= 600 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2615306, 1, -1, 8, 420)
	elseif a <= 900 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2615308, 1, -1, 8, 420)
	elseif a <= 950 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 3304001, 1, -1, 8, 420)
		Temp=3304001
	elseif a <= 980 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1412001, 1, -1, 8, 420)
	elseif a <= 990 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1412002, 1, -1, 8, 420)
		Temp=1412002
	elseif a <=1000 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1360122, 1, -1, 8, 420)
		Temp= 1360122
	end

	if Temp ~= 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100111)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, 3200713)
		msg.AddMsgEvent(MsgID, 4, Temp)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end

end


function I3200713_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
	return 40, false
	end
	return 0, false
end

aux.RegisterItemEvent(3200713, 1, "I3200713_GiftBag")
aux.RegisterItemEvent(3200713, 0, "I3200713_CanUseGiftBag")
