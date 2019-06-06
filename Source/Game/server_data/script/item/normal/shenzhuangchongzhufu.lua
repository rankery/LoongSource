--剑客专属职业装备重铸

--可否使用函数
function i3200811_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220013
	zhiyetaozhuang[2] = 8220029
	zhiyetaozhuang[3] = 8220045
	zhiyetaozhuang[4] = 8220061
	zhiyetaozhuang[5] = 8220077
	zhiyetaozhuang[6] = 8220093
	zhiyetaozhuang[7] = 8220101
	zhiyetaozhuang[8] = 8220117
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200811_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220013
	zhiyetaozhuang[2] = 8220029
	zhiyetaozhuang[3] = 8220045
	zhiyetaozhuang[4] = 8220061
	zhiyetaozhuang[5] = 8220077
	zhiyetaozhuang[6] = 8220093
	zhiyetaozhuang[7] = 8220101
	zhiyetaozhuang[8] = 8220117
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200811, 1, "i3200811_Use")
aux.RegisterItemEvent(3200811, 0, "i3200811_CanUse")

--刀客专属职业装备重铸

--可否使用函数
function i3200812_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220014
	zhiyetaozhuang[2] = 8220030
	zhiyetaozhuang[3] = 8220046
	zhiyetaozhuang[4] = 8220062
	zhiyetaozhuang[5] = 8220078
	zhiyetaozhuang[6] = 8220094
	zhiyetaozhuang[7] = 8220102
	zhiyetaozhuang[8] = 8220118
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200812_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220014
	zhiyetaozhuang[2] = 8220030
	zhiyetaozhuang[3] = 8220046
	zhiyetaozhuang[4] = 8220062
	zhiyetaozhuang[5] = 8220078
	zhiyetaozhuang[6] = 8220094
	zhiyetaozhuang[7] = 8220102
	zhiyetaozhuang[8] = 8220118
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200812, 1, "i3200812_Use")
aux.RegisterItemEvent(3200812, 0, "i3200812_CanUse")


--神兵专属职业装备重铸

--可否使用函数
function i3200813_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220015
	zhiyetaozhuang[2] = 8220031
	zhiyetaozhuang[3] = 8220047
	zhiyetaozhuang[4] = 8220063
	zhiyetaozhuang[5] = 8220079
	zhiyetaozhuang[6] = 8220095
	zhiyetaozhuang[7] = 8220103
	zhiyetaozhuang[8] = 8220119
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200813_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220015
	zhiyetaozhuang[2] = 8220031
	zhiyetaozhuang[3] = 8220047
	zhiyetaozhuang[4] = 8220063
	zhiyetaozhuang[5] = 8220079
	zhiyetaozhuang[6] = 8220095
	zhiyetaozhuang[7] = 8220103
	zhiyetaozhuang[8] = 8220119
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200813, 1, "i3200813_Use")
aux.RegisterItemEvent(3200813, 0, "i3200813_CanUse")

--弦羽专属职业装备重铸

--可否使用函数
function i3200814_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220016
	zhiyetaozhuang[2] = 8220032
	zhiyetaozhuang[3] = 8220048
	zhiyetaozhuang[4] = 8220064
	zhiyetaozhuang[5] = 8220080
	zhiyetaozhuang[6] = 8220096
	zhiyetaozhuang[7] = 8220104
	zhiyetaozhuang[8] = 8220120
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200814_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220016
	zhiyetaozhuang[2] = 8220032
	zhiyetaozhuang[3] = 8220048
	zhiyetaozhuang[4] = 8220064
	zhiyetaozhuang[5] = 8220080
	zhiyetaozhuang[6] = 8220096
	zhiyetaozhuang[7] = 8220104
	zhiyetaozhuang[8] = 8220120
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200814, 1, "i3200814_Use")
aux.RegisterItemEvent(3200814, 0, "i3200814_CanUse")

--玄师专属职业装备重铸

--可否使用函数
function i3200815_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220005
	zhiyetaozhuang[2] = 8220021
	zhiyetaozhuang[3] = 8220037
	zhiyetaozhuang[4] = 8220053
	zhiyetaozhuang[5] = 8220069
	zhiyetaozhuang[6] = 8220085
	zhiyetaozhuang[7] = 8220109
	zhiyetaozhuang[8] = 8220125
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200815_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220005
	zhiyetaozhuang[2] = 8220021
	zhiyetaozhuang[3] = 8220037
	zhiyetaozhuang[4] = 8220053
	zhiyetaozhuang[5] = 8220069
	zhiyetaozhuang[6] = 8220085
	zhiyetaozhuang[7] = 8220109
	zhiyetaozhuang[8] = 8220125
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200815, 1, "i3200815_Use")
aux.RegisterItemEvent(3200815, 0, "i3200815_CanUse")

--冥师专属职业装备重铸

--可否使用函数
function i3200816_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220006
	zhiyetaozhuang[2] = 8220022
	zhiyetaozhuang[3] = 8220038
	zhiyetaozhuang[4] = 8220054
	zhiyetaozhuang[5] = 8220070
	zhiyetaozhuang[6] = 8220086
	zhiyetaozhuang[7] = 8220110
	zhiyetaozhuang[8] = 8220126
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200816_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220006
	zhiyetaozhuang[2] = 8220022
	zhiyetaozhuang[3] = 8220038
	zhiyetaozhuang[4] = 8220054
	zhiyetaozhuang[5] = 8220070
	zhiyetaozhuang[6] = 8220086
	zhiyetaozhuang[7] = 8220110
	zhiyetaozhuang[8] = 8220126
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200816, 1, "i3200816_Use")
aux.RegisterItemEvent(3200816, 0, "i3200816_CanUse")

--药师专属职业装备重铸
--可否使用函数
function i3200817_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220007
	zhiyetaozhuang[2] = 8220023
	zhiyetaozhuang[3] = 8220039
	zhiyetaozhuang[4] = 8220055
	zhiyetaozhuang[5] = 8220071
	zhiyetaozhuang[6] = 8220087
	zhiyetaozhuang[7] = 8220111
	zhiyetaozhuang[8] = 8220127
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200817_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220007
	zhiyetaozhuang[2] = 8220023
	zhiyetaozhuang[3] = 8220039
	zhiyetaozhuang[4] = 8220055
	zhiyetaozhuang[5] = 8220071
	zhiyetaozhuang[6] = 8220087
	zhiyetaozhuang[7] = 8220111
	zhiyetaozhuang[8] = 8220127
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200817, 1, "i3200817_Use")
aux.RegisterItemEvent(3200817, 0, "i3200817_CanUse")

--乐师专属职业装备重铸

--可否使用函数
function i3200818_CanUse(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang = {}
	zhiyetaozhuang[1] = 8220008
	zhiyetaozhuang[2] = 8220024
	zhiyetaozhuang[3] = 8220040
	zhiyetaozhuang[4] = 8220056
	zhiyetaozhuang[5] = 8220072
	zhiyetaozhuang[6] = 8220088
	zhiyetaozhuang[7] = 8220112
	zhiyetaozhuang[8] = 8220128
	local bRet = 32
	local bIgnore = false

	for i = 1,8 do
		local a = zhiyetaozhuang[i]
		local b = role.GetRoleItemNum(TargetID,a)
		if b > 0 then
		    bRet = 0
		break
		end
	end
	    return bRet, bIgnore

end

--使用效果
function i3200818_Use(MapID, InstanceID, TypeID, TargetID)
	local zhiyetaozhuang={}
	zhiyetaozhuang[1] = 8220008
	zhiyetaozhuang[2] = 8220024
	zhiyetaozhuang[3] = 8220040
	zhiyetaozhuang[4] = 8220056
	zhiyetaozhuang[5] = 8220072
	zhiyetaozhuang[6] = 8220088
	zhiyetaozhuang[7] = 8220112
	zhiyetaozhuang[8] = 8220128
	for i=1, 8 do
		local a = role.GetRoleItemNum(TargetID, zhiyetaozhuang[i])
		if  a > 0 then
		    role.RemoveFromRole(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, 420)
		    role.AddRoleItem(MapID, InstanceID, TargetID, zhiyetaozhuang[i], 1, -1, 8, 420)
			break
		end
	end
end

--注册
aux.RegisterItemEvent(3200818, 1, "i3200818_Use")
aux.RegisterItemEvent(3200818, 0, "i3200818_CanUse")

