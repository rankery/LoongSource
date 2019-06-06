

--仙人指路活动道具制作需求 特权礼包

Tequanlibao={}
Tequanlibao[1331001]={count=2,item1=1360002,item2=1350914,item3=0,item4=0,item5=0,item6=0,item7=0,item8=0,item9=0,item10=0,item11=0,item12=0,item13=0,item14=0,item15=0,item16=0}
Tequanlibao[1331002]={count=3,item1=1360004,item2=3200464,item3=1360003,item4=0,item5=0,item6=0,item7=0,item8=0,item9=0,item10=0,item11=0,item12=0,item13=0,item14=0,item15=0,item16=0}
Tequanlibao[1331003]={count=4,item1=1330201,item2=4008001,item3=3200711,item4=3200464,item5=0,item6=0,item7=0,item8=0,item9=0,item10=0,item11=0,item12=0,item13=0,item14=0,item15=0,item16=0}
Tequanlibao[1331004]={count=7,item1=1330202,item2=1330203,item3=4009001,item4=3303029,item5=2420015,item6=1350001,item7=1350002,item8=0,item9=0,item10=0,item11=0,item12=0,item13=0,item14=0,item15=0,item16=0}
Tequanlibao[1331005]={count=16,item1=2611606,item2=2611607,item3=2611608,item4=2611609,item5=2611610,item6=1300103,item7=1300003,item8=3200462,item9=3200411,item10=2420015,item11=1350001,item12=1350002,item13=1330205,item14=4009001,item15=4008001,item16=1350915}

function Tequanlibao_OnUse(MapID, InstanceID, TypeID, RoleID)
		local a=Tequanlibao[TypeID].item1
		local b=Tequanlibao[TypeID].item2
		local c=Tequanlibao[TypeID].item3
		local d=Tequanlibao[TypeID].item4
		local e=Tequanlibao[TypeID].item5
		local f=Tequanlibao[TypeID].item6
		local g=Tequanlibao[TypeID].item7
		local h=Tequanlibao[TypeID].item8
		local i=Tequanlibao[TypeID].item9
		local j=Tequanlibao[TypeID].item10
		local k=Tequanlibao[TypeID].item11
		local l=Tequanlibao[TypeID].item12
		local m=Tequanlibao[TypeID].item13
		local n=Tequanlibao[TypeID].item14
		local o=Tequanlibao[TypeID].item15
		local p=Tequanlibao[TypeID].item16
		if TypeID == 1331001 then
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, b, 1, -1, 8, 420)
		end
		if TypeID == 1331002 then
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, b, 20, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, c, 1, -1, 8, 420)
		end
		if TypeID == 1331003 then
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, b, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, c, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, d, 30, -1, 8, 420)
		end
		if TypeID == 1331004 then
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, b, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, c, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, d, 5, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, e, 6, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, f, 5, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, g, 2, -1, 8, 420)
		end
		if TypeID == 1331005 then
			role.AddRoleItem(MapID, InstanceID, RoleID, a, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, b, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, c, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, d, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, e, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, f, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, g, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, h, 5, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, i, 2, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, j, 15, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, k, 5, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, l, 2, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, m, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, n, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, o, 1, -1, 8, 420)
			role.AddRoleItem(MapID, InstanceID, RoleID, p, 1, -1, 8, 420)
		end
end

function Tequanlibao_CanUse(MapID, InstanceID, TypeID, RoleID)
	local n = Tequanlibao[TypeID].count
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < n) then
	--提示玩家背包空间不足
		bRet = 40
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(1331001, 1, "Tequanlibao_OnUse")
aux.RegisterItemEvent(1331002, 1, "Tequanlibao_OnUse")
aux.RegisterItemEvent(1331003, 1, "Tequanlibao_OnUse")
aux.RegisterItemEvent(1331004, 1, "Tequanlibao_OnUse")
aux.RegisterItemEvent(1331005, 1, "Tequanlibao_OnUse")
aux.RegisterItemEvent(1331001, 0, "Tequanlibao_CanUse")
aux.RegisterItemEvent(1331002, 0, "Tequanlibao_CanUse")
aux.RegisterItemEvent(1331003, 0, "Tequanlibao_CanUse")
aux.RegisterItemEvent(1331004, 0, "Tequanlibao_CanUse")
aux.RegisterItemEvent(1331005, 0, "Tequanlibao_CanUse")
