--2616513 情人节礼盒
--2616516 情侣精品礼盒
--2616520 恋人礼包






function I2616513_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet,bIgnore = 0,false
	local FreeSize = role.GetBagFreeSize(RoleID)
	if FreeSize < 1  then
		bRet = 40
	end
	return bRet,bIgnore
end
aux.RegisterItemEvent(2616516, 1, "I2616516_OnUse")
aux.RegisterItemEvent(2616513, 1, "I2616513_OnUse")
aux.RegisterItemEvent(2616520, 1, "I2616520_OnUse")
aux.RegisterItemEvent(2616513, 0, "I2616513_CanUse")
aux.RegisterItemEvent(2616516, 0, "I2616513_CanUse")
aux.RegisterItemEvent(2616520, 0, "I2616513_CanUse")
function I2616513_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(100)
	if k <= 15 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, level*1000, 102)
	elseif k > 15 and k <= 30 then
		role.AddRoleExp(MapID, InstanceID, RoleID, level*1000)
	elseif k > 30 and k <= 40 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
	elseif k > 40 and k <= 50 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000008,10,-1,8,420)
	elseif k > 50 and k <= 60 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000002,1,-1,8,420)
	elseif k >60 and k <= 68 then
		local ShenQi = math.random(1501046,1501053)
		role.AddRoleItem(MapID,InstanceID,RoleID,ShenQi,1,5,8,420)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 4413)
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 4, 2616513)
		msg.AddMsgEvent(MsgID, 4, ShenQi)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif k > 68 then
		local ChengHao = math.random(1350331,1350339)
		role.AddRoleItem(MapID,InstanceID,RoleID,ChengHao,3,-1,8,420)
	end
end

function I2616516_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(100)
	if k <= 15 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, level*1000, 102)
	elseif k > 15 and k <= 30 then
		role.AddRoleExp(MapID, InstanceID, RoleID, level*1000)
	elseif k > 30 and k <= 40 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
	elseif k > 40 and k <= 50 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000008,10,-1,8,420)
	elseif k > 50 and k <= 60 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000000,1,-1,8,420)
	elseif k > 60 and k <= 70 then
		role.AddRoleItem(MapID,InstanceID,RoleID,1101001,10,-1,8,420)
	elseif k >70 then
		local ChengHao1 = math.random(1350331,1350339)
		role.AddRoleItem(MapID,InstanceID,RoleID,ChengHao1,1,-1,8,420)
	end
end

function I2616520_OnUse(MapID,InstanceID,TypeID,RoleID)
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.random(100)
	if k <= 15 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, level*1000, 102)
	elseif k > 15 and k <= 30 then
		role.AddRoleExp(MapID, InstanceID, RoleID, level*1000)
	elseif k > 30 and k <= 40 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000007,3,-1,8,420)
	elseif k > 40 and k <= 50 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000008,10,-1,8,420)
	elseif k > 50 and k <= 60 then
		role.AddRoleItem(MapID,InstanceID,RoleID,6000001,1,-1,8,420)
	elseif k > 60 and k <= 64 then
		local xinnianEquip = math.random(1501042,1501045)
		role.AddRoleItem(MapID,InstanceID,RoleID,xinnianEquip,1,-1,8,420)
	elseif k > 64 then
		local ChengHao2 = math.random(1350331,1350339)
		role.AddRoleItem(MapID,InstanceID,RoleID,ChengHao2,1,-1,8,420)
	end
end
