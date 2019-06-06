--2000041	[材]稀有石料
--2000044	[材]稀有玉石
--2000047	[材]稀有铁矿
--2000050	[材]稀有银矿
--1330005	镌刻材料包

--2000042	[材]极品石料
--2000045	[材]极品玉石
--2000048	[材]极品铁矿
--2000051	[材]极品银矿
--1330006	极品镌刻材料包


--普通镌刻包
function I1330005_OnUse(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local jinli = role.GetRoleAttValue(MapID, InstanceID, RoleID, 1)
	local neili = role.GetRoleAttValue(MapID, InstanceID, RoleID, 3)
	local k = math.random(1,2)
	if  jinli > neili then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000047, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000050, 4, -1, 8, 420)
	elseif jinli < neili then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000041, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000044, 4, -1, 8, 420)
	elseif jinli == neili then
		if  k == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000047, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000050, 4, -1, 8, 420)
		else
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000041, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000044, 4, -1, 8, 420)
		end
	end
	return 1
end


function I1330005_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end

end

aux.RegisterItemEvent(1330005, 1, "I1330005_OnUse")
aux.RegisterItemEvent(1330005, 0, "I1330005_CanUse")

--极品镌刻包
function I1330006_OnUse(MapID, InstanceID, TypeID, RoleID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
	local jinli = role.GetRoleAttValue(MapID, InstanceID, RoleID, 1)
	local neili = role.GetRoleAttValue(MapID, InstanceID, RoleID, 3)
	local k = math.random(1,2)
	if  jinli > neili then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000048, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000051, 4, -1, 8, 420)
	elseif jinli < neili then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000042, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000045, 4, -1, 8, 420)
	elseif jinli == neili then
		if  k == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000048, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000051, 4, -1, 8, 420)
		else
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000042, 18, -1, 8, 420)
		role.AddRoleItem(MapID, InstanceID, RoleID, 2000045, 4, -1, 8, 420)
		end
	end
	return 1
end


function I1330006_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end

end

aux.RegisterItemEvent(1330006, 1, "I1330006_OnUse")
aux.RegisterItemEvent(1330006, 0, "I1330006_CanUse")





