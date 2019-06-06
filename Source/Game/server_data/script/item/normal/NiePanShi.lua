
--[[
暗器羽灵强化相关

6010813	涅槃石·壹级
6010814	涅槃石·贰级
6010815	涅槃石·叁级
6010816	涅槃石·肆级
6010817	涅槃石·伍级
6010818	普通涅槃石融剂
6010819	上等涅槃石融剂
6010820	优质涅槃石融剂
6010821	极品涅槃石融剂

6010822	精炼涅槃石·壹级
6010823	精炼涅槃石·贰级
6010824	精炼涅槃石·叁级
6010825	精炼涅槃石·肆级
6010826	精炼涅槃石·伍级
6010827	普通涅槃石极速融剂
6010828	上等涅槃石极速融剂
6010829	优质涅槃石极速融剂
6010830	极品涅槃石极速融剂
]]

--使用判断
function NiePanShi_canUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	local FreeSize = role.GetBagFreeSize(RoleID)	--判断背包空闲空间是否足够
	if (FreeSize < 1) then
		bRet = 40	--提示玩家背包空间不足
		return bRet, bIgnore
	else
		--涅槃石融剂
		if  TypeID == 6010818 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010813)
			Num_2 = role.GetRoleItemNum(RoleID,6010822)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010819 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010814)
			Num_2 = role.GetRoleItemNum(RoleID,6010823)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010820 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010815)
			Num_2 = role.GetRoleItemNum(RoleID,6010824)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010821 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010816)
			Num_2 = role.GetRoleItemNum(RoleID,6010825)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
			--涅槃石极速融剂
		elseif  TypeID == 6010827 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010813)
			Num_2 = role.GetRoleItemNum(RoleID,6010822)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010828 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010814)
			Num_2 = role.GetRoleItemNum(RoleID,6010823)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010829 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010815)
			Num_2 = role.GetRoleItemNum(RoleID,6010824)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end
		elseif  TypeID == 6010830 then
			local Num_1 = 0
			local Num_2 = 0
			Num_1 = role.GetRoleItemNum(RoleID,6010816)
			Num_2 = role.GetRoleItemNum(RoleID,6010825)
			if Num_1 < 5 and Num_2 < 5 then
				bRet = 32	--物品不可使用
				return bRet, bIgnore
			else
				return bRet, bIgnore
			end

		end
	end
end
aux.RegisterItemEvent(6010818, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010819, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010820, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010821, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010827, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010828, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010829, 0, "NiePanShi_canUse")
aux.RegisterItemEvent(6010830, 0, "NiePanShi_canUse")

--使用道具

function NiePanShi_OnUse(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	--涅槃石融剂
	if  TypeID == 6010818 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010813)
		Num_2 = role.GetRoleItemNum(RoleID,6010822)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010813,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010814, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010822,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010814, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010819 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010814)
		Num_2 = role.GetRoleItemNum(RoleID,6010823)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010814,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010815, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010823,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010815, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010820 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010815)
		Num_2 = role.GetRoleItemNum(RoleID,6010824)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010815,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010816, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010824,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010816, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010821 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010816)
		Num_2 = role.GetRoleItemNum(RoleID,6010825)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010816,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010817, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010825,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010817, 1, -1, 8, 420)
		end
		--涅槃石极速融剂
	elseif  TypeID == 6010827 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010813)
		Num_2 = role.GetRoleItemNum(RoleID,6010822)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010813,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010823, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010822,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010823, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010828 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010814)
		Num_2 = role.GetRoleItemNum(RoleID,6010823)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010814,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010824, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010823,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010824, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010829 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010815)
		Num_2 = role.GetRoleItemNum(RoleID,6010824)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010815,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010825, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010824,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010825, 1, -1, 8, 420)
		end
	elseif  TypeID == 6010830 then
		local Num_1 = 0
		local Num_2 = 0
		Num_1 = role.GetRoleItemNum(RoleID,6010816)
		Num_2 = role.GetRoleItemNum(RoleID,6010825)
		if Num_1 >= 5 then
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010816,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010826, 1, -1, 8, 420)
		else
			role.RemoveFromRole(MapID,InstanceID,RoleID,6010825,5,420)
			role.AddRoleItem(MapID, InstanceID, RoleID, 6010826, 1, -1, 8, 420)
		end
	end
end
aux.RegisterItemEvent(6010818, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010819, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010820, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010821, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010827, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010828, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010829, 1, "NiePanShi_OnUse")
aux.RegisterItemEvent(6010830, 1, "NiePanShi_OnUse")




































