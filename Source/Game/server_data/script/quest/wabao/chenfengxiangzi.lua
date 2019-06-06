--1350018	仙币
--1350001	[活]七杀图
--1350002	[活]破军图
--2000001	[材]普通石料
--2000011	[材]普通铁矿
--2000206	[材]普通皮革
--2000211	[材]普通棉布
--2000006	[材]普通玉石
--2000016	[材]普通银矿
--2000201	[材]普通麻布
--2000216	[材]普通素纱
--1350016	[活]七星护宝锤
--1350017	[活]紫薇护宝锤
--1350019	星辰符
--1350051	[活]逆天龙鳞
--1350052	[活]真凰龙须
--1350053	[活]万邪龙眼
--1350054	[活]玄金龙爪
--2000002	[材]精良石料
--2000007	[材]精良玉石
--2000012	[材]精良铁矿
--2000017	[材]精良银矿
--2000207	[材]精良皮革
--2000212	[材]精良棉布
--2000202	[材]精良麻布
--2000217	[材]精良素纱

--尘封箱子


function I1350031_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
    local  k = math.random(1,100)
		if  k >=1 and k <= 30  then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 5, -1, 8, 420)
		elseif k>=31 and k<=38 then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 10, -1, 8, 420)
		elseif k>=39 and k<=42 then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 20, -1, 8, 420)
		elseif k>=43 and k<=50 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350001, 3, -1, 8, 420)
		elseif k>=51 and k<=52 then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350002)
			msg.DispatchWorldMsgEvent(MsgID)
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350002, 1, -1, 8, 420)
		elseif k>=53 and k<=57 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000001, 5, -1, 8, 420)
		elseif k>=58 and k<=62 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000011, 5, -1, 8, 420)
		elseif k>=63 and k<=67 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000206, 5, -1, 8, 420)
		elseif k>=68 and k<=72 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000211, 5, -1, 8, 420)
		elseif k>=73 and k<=77 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000006, 5, -1, 8, 420)
		elseif k>=78 and k<=82 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000016, 5, -1, 8, 420)
		elseif k>=83 and k<=87 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000201, 5, -1, 8, 420)
		elseif k>=88 and k<=92 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000216, 5, -1, 8, 420)
		elseif k>=93 and k<=97 then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350016)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350016, 1, -1, 8, 420)
		elseif k>=98 and k<=99 then
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350017)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350017, 1, -1, 8, 420)
		else
			--“###（姓名）获得####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 418)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350019)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350019, 1, -1, 8, 420)
		end

end

function I1350031_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350031, 1, "I1350031_GiftBag")
aux.RegisterItemEvent(1350031, 0, "I1350031_CanUseGiftBag")

--镶金箱子

function I1350032_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
    local  k = math.random(1,100)
		if  k >=1 and k <= 10  then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 5, -1, 8, 420)
		elseif k>=11 and k<=18 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 10, -1, 8, 420)
		elseif k>=19 and k<=20 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350018)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350018, 20, -1, 8, 420)
		elseif k>=21 and k<=25 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000002, 3, -1, 8, 420)
		elseif k>=26 and k<=30 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000012, 3, -1, 8, 420)
		elseif k>=31 and k<=35 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000207, 3, -1, 8, 420)
		elseif k>=36 and k<=40 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000212, 3, -1, 8, 420)
		elseif k>=41 and k<=45 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000007, 3, -1, 8, 420)
		elseif k>=46 and k<=50 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000017, 3, -1, 8, 420)
		elseif k>=51 and k<=55 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000202, 3, -1, 8, 420)
		elseif k>=56 and k<=60 then
		role.AddRoleItem(MapID, InstanceID, TargetID, 2000217, 3, -1, 8, 420)
		elseif k>=61 and k<=75 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350016)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350016, 3, -1, 8, 420)
		elseif k>=76 and k<=95 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350017)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350017, 1, -1, 8, 420)
		elseif k==96 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350019)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350019, 1, -1, 8, 420)
		elseif k==97 then
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350051)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350051, 1, -1, 8, 420)
		elseif k==98 then
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350052)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350052, 1, -1, 8, 420)
		elseif k==99 then
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350053)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350053, 1, -1, 8, 420)
		else
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 419)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350054)
			msg.DispatchWorldMsgEvent(MsgID)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350054, 1, -1, 8, 420)
		end
end

function I1350032_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1350032, 1, "I1350032_GiftBag")
aux.RegisterItemEvent(1350032, 0, "I1350032_CanUseGiftBag")
