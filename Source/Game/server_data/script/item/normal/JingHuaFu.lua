

--净化符 用于重铸羽灵
--6010807	净化符


function JingHuaFu_canUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)	--判断背包空闲空间是否足够
	local Num = 0
	if (FreeSize < 1) then
		bRet = 40	--提示玩家背包空间不足
		return bRet, bIgnore
	else
		for i = 1 ,16 do
			if role.GetRoleItemNum(RoleID,8710000+i) > 0 or role.GetRoleItemNum(RoleID,8710100+i) > 0 then
				Num = Num + 1
			end
		end
		if Num > 0 then
			return bRet, bIgnore
		else
			bRet = 32	--提示物品不可使用
			return bRet, bIgnore
		end
	end
end

aux.RegisterItemEvent(6010807, 0, "JingHuaFu_canUse")



function JingHuaFu_OnUse(MapID, InstanceID, TypeID, RoleID)

	for i = 1 ,16 do
		if role.GetRoleItemNum(RoleID,8710000+i) > 0 then
			if i >=1 and i <=4 then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710000+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+i,1, 2, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif
				i >= 5 and i <= 7 then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710000+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+i,1, 3, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif
				i == 8 or i == 9 or i == 11 or i == 12 or i == 14 or i == 15   then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710000+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+i,1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif i == 10 or i == 13 or i == 16  then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710000+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710000+i,1, 5, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			end
		elseif  role.GetRoleItemNum(RoleID,8710100+i) > 0 then
			if i >=1 and i <=4 then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710100+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+i,1, 2, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif
				i >= 5 and i <= 7 then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710100+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+i,1, 3, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif
				i == 8 or i == 9 or i == 11 or i == 12 or i == 14 or i == 15   then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710100+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+i,1, 4, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			elseif i == 10 or i == 13 or i == 16  then
				role.RemoveFromRole(MapID,InstanceID,RoleID,8710100+i,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 8710100+i,1, 5, 8, 420)
				local MsgID = msg.BeginMsgEvent() -- 净化成功
				msg.AddMsgEvent(MsgID, 26, 4309)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				break
			end
		end
	end
end

aux.RegisterItemEvent(6010807, 1, "JingHuaFu_OnUse")



