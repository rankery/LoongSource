

--[[羽灵蛋染色剂

6010701	棕色染剂·乾
6010702	黑色染剂·乾
6010703	蓝色染剂·坤
6010704	粉色染剂·坤]]

function RanSeJi_canUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)	--判断背包空闲空间是否足够
	local Num_qian = 0
	local Num_kun = 0
	if (FreeSize < 1) then
		bRet = 40	--提示玩家背包空间不足
		return bRet, bIgnore
	else
		if TypeID == 6010701 or TypeID == 6010702 then
			for i = 8 ,16 do
				if role.GetRoleItemNum(RoleID,6010000+i) > 0 then
					Num_qian = Num_qian + 1
				end
			end
			if Num_qian > 0 then
				return bRet, bIgnore
			else
				bRet = 32
				return bRet, bIgnore
			end
		elseif TypeID == 6010703 or TypeID == 6010704 then
			for k = 8 ,16 do
				if role.GetRoleItemNum(RoleID,6010100+k) >0 then
					Num_kun = Num_kun + 1
				end
			end
			if Num_kun > 0 then
				return bRet, bIgnore
			else
				bRet = 32
				return bRet, bIgnore
			end
		end
	end
end

aux.RegisterItemEvent(6010701, 0, "RanSeJi_canUse")
aux.RegisterItemEvent(6010702, 0, "RanSeJi_canUse")
aux.RegisterItemEvent(6010703, 0, "RanSeJi_canUse")
aux.RegisterItemEvent(6010704, 0, "RanSeJi_canUse")

function RanSeJi_OnUse(MapID, InstanceID, TypeID, RoleID)
	if TypeID == 6010701 then
		for i = 8 ,16 do
			if role.GetRoleItemNum(RoleID,6010000+i) > 0 then
				if role.GetRoleItemEnmity(RoleID,6010000+i) == 80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010011, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+i,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010000+i) == 90 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010012, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+i,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010000+i) == 100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010013, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+i,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				end
			end
		end
	elseif TypeID == 6010702 then
		for k = 8 ,16 do
			if role.GetRoleItemNum(RoleID,6010000+k) > 0 then
				if role.GetRoleItemEnmity(RoleID,6010000+k) == 80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010014, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+k,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010000+k) == 90 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010015, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+k,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010000+k) == 100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010016, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010000+k,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				end
			end
		end
	elseif TypeID == 6010703 then
		for j = 8 ,16 do
			if role.GetRoleItemNum(RoleID,6010100+j) > 0 then
				if role.GetRoleItemEnmity(RoleID,6010100+j) == 80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010111, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+j,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010100+j) == 90 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010112, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+j,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010100+j) == 100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010113, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+j,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				end
			end
		end
	elseif TypeID == 6010704 then
		for l = 8 ,16 do
			if role.GetRoleItemNum(RoleID,6010100+l) > 0 then
				if role.GetRoleItemEnmity(RoleID,6010100+l) == 80 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010114, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+l,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010100+l) == 90 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010115, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+l,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				elseif
					role.GetRoleItemEnmity(RoleID,6010100+l) == 100 then
					role.AddRoleItem(MapID, InstanceID, RoleID, 6010116, 1, -1, 8, 420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,TypeID,1,420)
					role.RemoveFromRole(MapID,InstanceID,RoleID,6010100+l,1,420)
					local MsgID = msg.BeginMsgEvent() -- 染色成功
					msg.AddMsgEvent(MsgID, 26, 4308)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					break
				end
			end
		end
	end
end

aux.RegisterItemEvent(6010701, 1, "RanSeJi_OnUse")
aux.RegisterItemEvent(6010702, 1, "RanSeJi_OnUse")
aux.RegisterItemEvent(6010703, 1, "RanSeJi_OnUse")
aux.RegisterItemEvent(6010704, 1, "RanSeJi_OnUse")









