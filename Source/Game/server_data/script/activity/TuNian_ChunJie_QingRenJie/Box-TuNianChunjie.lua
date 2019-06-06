--2616510 绚彩礼包
--2616509 彩色
--2616508 蓝色
--2616507 红色



XinnianHongseLibao = 0  -- Chunjie.lua内Chunjie_OnTimerMin函数每日重置
XinnianXuancaiLibao = 0 -- Chunjie.lua内Chunjie_OnTimerMin函数每日重置

function I2616507_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if XinnianHongseLibao < 1 then
		local k = math.random(1000)
		if k <= 50 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 12614)   --非常遗憾，您没有开出任何奖励……
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif k <= 200 and k > 50 then
			role.AddRoleSilver(MapID, InstanceID, RoleID, level*50, 102)
		elseif k >200 and k <= 300 then
			role.AddRoleExp(MapID, InstanceID, RoleID, level*50)
		elseif k > 300 and k <= 400 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
		elseif k > 400 and k <= 410 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304002,5,-1,8,420)
		elseif k > 410 and k <= 470 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303035,1,-1,8,420)
		elseif k > 470 and k <= 530 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303036,1,-1,8,420)
		elseif k > 530 and k <= 590 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303037,1,-1,8,420)
		elseif k > 590 and k <= 650 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303038,1,-1,8,420)
		elseif k > 650 and k <= 710 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303039,1,-1,8,420)
		elseif k > 710 and k <= 760 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303040,1,-1,8,420)
		elseif k > 760 and k <= 810 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303041,1,-1,8,420)
		elseif k > 810 and k <= 860 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303042,1,-1,8,420)
		elseif k > 860 and k <= 885 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304007,1,-1,8,420)
		elseif k > 885 and k <= 910 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304004,1,-1,8,420)
		elseif k > 910 and k <= 935 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304005,1,-1,8,420)
		elseif k > 935 and k <= 960 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304006,1,-1,8,420)
		elseif k > 960 and k <= 970 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3400001,1,-1,8,420)
			XinnianHongseLibao = XinnianHongseLibao + 1
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 2616507)
			msg.AddMsgEvent(MsgID, 4, 3400001)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k > 970 and k <= 980 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3400002,1,-1,8,420)
			XinnianHongseLibao = XinnianHongseLibao + 1
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 2616507)
			msg.AddMsgEvent(MsgID, 4, 3400002)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k > 980 and k <= 990 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3400003,1,-1,8,420)
			XinnianHongseLibao = XinnianHongseLibao + 1
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 2616507)
			msg.AddMsgEvent(MsgID, 4, 3400003)
			msg.DispatchWorldMsgEvent(MsgID)
		else
			role.AddRoleItem(MapID,InstanceID,RoleID,3400004,1,-1,8,420)
			XinnianHongseLibao = XinnianHongseLibao + 1
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, 2616507)
			msg.AddMsgEvent(MsgID, 4, 3400004)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	else
		local k = math.random(960)
		if k <= 50 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 12614)   --非常遗憾，您没有开出任何奖励……
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif k <= 200 and k > 50 then
			role.AddRoleSilver(MapID, InstanceID, RoleID, level*50, 102)
		elseif k >200 and k <= 300 then
			role.AddRoleExp(MapID, InstanceID, RoleID, level*50)
		elseif k > 300 and k <= 400 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
		elseif k > 400 and k <= 410 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304002,5,-1,8,420)
		elseif k > 410 and k <= 470 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303035,1,-1,8,420)
		elseif k > 470 and k <= 530 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303036,1,-1,8,420)
		elseif k > 530 and k <= 590 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303037,1,-1,8,420)
		elseif k > 590 and k <= 650 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303038,1,-1,8,420)
		elseif k > 650 and k <= 710 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303039,1,-1,8,420)
		elseif k > 710 and k <= 760 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303040,1,-1,8,420)
		elseif k > 760 and k <= 810 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303041,1,-1,8,420)
		elseif k > 810 and k <= 860 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3303042,1,-1,8,420)
		elseif k > 860 and k <= 885 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304007,1,-1,8,420)
		elseif k > 885 and k <= 910 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304004,1,-1,8,420)
		elseif k > 910 and k <= 935 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304005,1,-1,8,420)
		else
			role.AddRoleItem(MapID,InstanceID,RoleID,3304006,1,-1,8,420)
		end
	end
end

function I2616507_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet,bIgnore = 0,false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if FreeSize < 1  then
		bRet = 40
	end
	return bRet,bIgnore
end

aux.RegisterItemEvent(2616507, 1, "I2616507_OnUse")
aux.RegisterItemEvent(2616508, 1, "I2616508_OnUse")
aux.RegisterItemEvent(2616509, 1, "I2616509_OnUse")
aux.RegisterItemEvent(2616510, 1, "I2616510_OnUse")
aux.RegisterItemEvent(2616507, 0, "I2616507_CanUse")
aux.RegisterItemEvent(2616508, 0, "I2616507_CanUse")
aux.RegisterItemEvent(2616509, 0, "I2616507_CanUse")
aux.RegisterItemEvent(2616510, 0, "I2616507_CanUse")

function I2616508_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(100)
	if k <= 5 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 12614)   --非常遗憾，您没有开出任何奖励……
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif k > 5 and k <= 19 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, level*250, 102)
	elseif k > 19 and k <= 34 then
		role.AddRoleExp(MapID, InstanceID, RoleID, level*250)
	elseif k > 34 and k <= 44 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
	elseif k > 44 and k <= 50 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303035,1,-1,8,420)
	elseif k > 50 and k <= 56 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303036,1,-1,8,420)
	elseif k > 56 and k <= 62 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303037,1,-1,8,420)
	elseif k > 62 and k <= 68 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303038,1,-1,8,420)
	elseif k > 68 and k <= 74 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303039,1,-1,8,420)
	elseif k > 74 and k <= 80 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303040,1,-1,8,420)
	elseif k > 80 and k <= 86 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303041,1,-1,8,420)
	elseif k > 86 and k <= 92 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303042,1,-1,8,420)
	elseif k == 93 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442201,1,-1,8,420)
	elseif k == 94 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442202,1,-1,8,420)
	elseif k == 95 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442203,1,-1,8,420)
	elseif k == 96 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442204,1,-1,8,420)
	elseif k == 97 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442205,1,-1,8,420)
	elseif k == 98 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442206,1,-1,8,420)
	elseif k == 99 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1442207,1,-1,8,420)
	else
		role.AddRoleItem(MapID,InstanceID,RoleID,1442208,1,-1,8,420)
	end
end

function I2616509_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(100)
	if k <= 15 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, level*500, 102)
	elseif k > 15 and k <= 30 then
		role.AddRoleExp(MapID, InstanceID, RoleID, level*500)
	elseif k > 30 and k <= 32 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303035,1,-1,8,420)
	elseif k > 32 and k <= 34 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303036,1,-1,8,420)
	elseif k > 34 and k <= 36 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303037,1,-1,8,420)
	elseif k > 36 and k <= 38 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303038,1,-1,8,420)
	elseif k > 38 and k <= 40 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303039,1,-1,8,420)
	elseif k > 40 and k <= 42 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303040,1,-1,8,420)
	elseif k > 42 and k <= 44 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303041,1,-1,8,420)
	elseif k > 44 and k <= 46 then
		role.AddRoleItem(MapID,InstanceID,RoleID,3303042,1,-1,8,420)
	elseif k > 46 and k <= 61 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6010001,1,-1,8,420)
	elseif k > 61 and k <= 76 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6010101,1,-1,8,420)
	elseif k > 76 and k <= 86 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6010813,1,-1,8,420)
	elseif k > 86 and k <= 96 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6010810,1,-1,8,420)
	else
		role.AddRoleItem(MapID,InstanceID,RoleID,6010811,1,-1,8,420)
	end
end
function I2616510_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if XinnianXuancaiLibao < 1 then
		local k = math.random(100)
		if k <= 20 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010831,250,-1,8,420)
		elseif k == 21 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010108,1,-1,8,420)
		elseif k == 22 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010008,1,-1,8,420)
		elseif k > 22 and k <= 42 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6000212,1,-1,8,420)
		elseif k > 42 and k <= 62 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010830,1,-1,8,420)
		elseif k == 63 then
			XinnianXuancaiLibao = XinnianXuancaiLibao + 1
			role.AddRoleItem(MapID,InstanceID,RoleID,3400041,1,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, TypeID)
			msg.AddMsgEvent(MsgID, 4, 3400041)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k == 64 then
			XinnianXuancaiLibao = XinnianXuancaiLibao + 1
			role.AddRoleItem(MapID,InstanceID,RoleID,3400042,1,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, TypeID)
			msg.AddMsgEvent(MsgID, 4, 3400042)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k == 65 then
			XinnianXuancaiLibao = XinnianXuancaiLibao + 1
			role.AddRoleItem(MapID,InstanceID,RoleID,3400043,1,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, TypeID)
			msg.AddMsgEvent(MsgID, 4, 3400043)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k == 66 then
			XinnianXuancaiLibao = XinnianXuancaiLibao + 1
			role.AddRoleItem(MapID,InstanceID,RoleID,3400044,1,-1,8,420)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 4413)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 4, TypeID)
			msg.AddMsgEvent(MsgID, 4, 3400044)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif k > 66 and k <= 70 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010826,1,-1,8,420)
		elseif k > 70 and k <= 80 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304163,2,-1,8,420)
		else
			role.AddRoleItem(MapID,InstanceID,RoleID,6010812,1,-1,8,420)
		end
	else
		local k = math.random(96)
		if k <= 20 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010831,250,-1,8,420)
		elseif k == 21 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010108,1,-1,8,420)
		elseif k == 22 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010008,1,-1,8,420)
		elseif k > 22 and k <= 42 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6000212,1,-1,8,420)
		elseif k > 42 and k <= 62 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010830,1,-1,8,420)
		elseif k > 62 and k <= 66 then
			role.AddRoleItem(MapID,InstanceID,RoleID,6010826,1,-1,8,420)
		elseif k > 66 and k <= 74 then
			role.AddRoleItem(MapID,InstanceID,RoleID,3304163,5,-1,8,420)
		else
			role.AddRoleItem(MapID,InstanceID,RoleID,6010812,1,-1,8,420)
		end
	end
end
