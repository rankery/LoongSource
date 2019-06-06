--RZL106
--[[使用小药包2612723获得青砂2612717芨灰2612718银粉2612719各一个。
function i2612723_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i = role.GetBagFreeSize(TargetID)
	local a = role.GetRoleItemNum(TargetID, 2612717)
        local b = role.GetRoleItemNum(TargetID, 2612718)
        local c = role.GetRoleItemNum(TargetID, 2612719)
	if i <= 2 or a == 3 or b == 3 or c == 3 then
	    bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i2612723_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612717, 1, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612718, 1, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612719, 1, -1, 8, 420)
end

--注册
aux.RegisterItemEvent(2612723, 1, "i2612723_QuestUsable")
aux.RegisterItemEvent(2612723, 0, "i2612723_CanUse")


--使用小药包2612724获得青砂2612717芨灰2612718银粉2612719各两个。
function i2612724_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i = role.GetBagFreeSize(TargetID)
	local a = role.GetRoleItemNum(TargetID, 2612717)
        local b = role.GetRoleItemNum(TargetID, 2612718)
        local c = role.GetRoleItemNum(TargetID, 2612719)
	if i <= 2 or a >= 2 or b >= 2 or c >= 2 then
	    bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i2612724_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612717, 2, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612718, 2, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612719, 2, -1, 8, 420)
end

--注册
aux.RegisterItemEvent(2612724, 1, "i2612724_QuestUsable")
aux.RegisterItemEvent(2612724, 0, "i2612724_CanUse")


--使用小药包2612725获得青砂2612717芨灰2612718银粉2612719各三个。
function i2612725_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i = role.GetBagFreeSize(TargetID)
	local a = role.GetRoleItemNum(TargetID, 2612717)
        local b = role.GetRoleItemNum(TargetID, 2612718)
        local c = role.GetRoleItemNum(TargetID, 2612719)
	if i <= 2 or a >= 1 or b >= 1 or c >= 1 then
	    bRet = 40
	end
	return bRet, bIgnore
end

--使用效果
function i2612725_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612717, 3, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612718, 3, -1, 8, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 2612719, 3, -1, 8, 420)
end

--注册
aux.RegisterItemEvent(2612725, 1, "i2612725_QuestUsable")
aux.RegisterItemEvent(2612725, 0, "i2612725_CanUse")]]


--使用炼丹炉2612726
function i2612726_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    --local a = role.GetRoleItemNum(TargetID, 2612717)
    --local b = role.GetRoleItemNum(TargetID, 2612718)
    --local c = role.GetRoleItemNum(TargetID, 2612719)
	local i = role.GetBagFreeSize(TargetID)
	if i < 1 then
	    bRet = 40

    --if a == 0 or b == 0 or c == 0 then
	--    bRet = 32
   -- elseif a ~= b or b ~= c or a ~= c then
    --        bRet = 32
    end
    --返回
    return bRet, bIgnore
end

--使用效果
function i2612726_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    role.AddRoleItem(MapID, InstanceID, TargetID, 2612721, 1, -1, 8, 420)
    --[[local a = role.GetRoleItemNum(TargetID, 2612717)
    local b = role.GetRoleItemNum(TargetID, 2612718)
    local c = role.GetRoleItemNum(TargetID, 2612719)
    if a == 1 and b ==1 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612720, 1, -1, 8, 420)
    elseif a == 2 and b == 2 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612721, 1, -1, 8, 420)
    elseif a == 3 and b == 3 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612722, 1, -1, 8, 420)
	elseif a == 1 and b == 1 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003001, 1, -1, 8, 420)
	elseif a == 1 and b == 2 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003003, 1, -1, 8, 420)
    elseif a == 1 and b == 2 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003004, 1, -1, 8, 420)
	elseif a == 1 and b == 2 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000001, 1, -1, 8, 420)
	elseif a == 1 and b == 3 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2431043, 1, -1, 8, 420)
    elseif a == 1 and b == 3 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612717, 1, -1, 8, 420)
    elseif a == 1 and b == 3 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003004, 1, -1, 8, 420)
    elseif a == 2 and b == 1 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612719, 1, -1, 8, 420)
    elseif a == 2 and b == 1 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612718, 1, -1, 8, 420)
    elseif a == 2 and b == 1 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2431043, 1, -1, 8, 420)
    elseif a == 2 and b == 2 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2431043, 1, -1, 8, 420)
    elseif a == 2 and b == 2 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000101, 1, -1, 8, 420)
    elseif a == 2 and b == 3 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612718, 1, -1, 8, 420)
    elseif a == 2 and b == 3 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003002, 1, -1, 8, 420)
    elseif a == 2 and b == 3 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003003, 1, -1, 8, 420)
    elseif a == 3 and b == 1 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003004, 1, -1, 8, 420)
    elseif a == 3 and b == 1 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612717, 1, -1, 8, 420)
    elseif a == 3 and b == 1 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 1, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000001, 1, -1, 8, 420)
    elseif a == 3 and b == 2 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000101, 1, -1, 8, 420)
    elseif a == 3 and b == 2 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612719, 1, -1, 8, 420)
    elseif a == 3 and b == 2 and c == 3 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 2, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 3, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000101, 1, -1, 8, 420)
    elseif a == 3 and b == 3 and c == 1 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 1, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612727, 1, -1, 8, 420)
    elseif a == 3 and b == 3 and c == 2 then
		role.RemoveFromRole(MapID, InstanceID, TargetID, 2612717, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612718, 3, 420)
	    role.RemoveFromRole(MapID, InstanceID, TargetID, 2612719, 2, 420)
		role.AddRoleItem(MapID, InstanceID, TargetID, 2611017, 1, -1, 8, 420)
    end]]
end

--注册
aux.RegisterItemEvent(2612726, 1, "i2612726_QuestUsable")
aux.RegisterItemEvent(2612726, 0, "i2612726_CanUse")

--112	2003001	矿渣
--113	2612719	银粉
--121	2003003	糟糠
--122	2003004	角料
--123	1000001	[药]活血丹
--131	2431043	墙砖
--132	2612717	青砂
--133	2003004	角料
--211	2612719	银粉
--212	2612718	芨灰
--213	2431043	墙砖
--221	2431043	墙砖
--222	2612721	大力丸
--223	1000101	[药]回气散
--231	2612718	芨灰
--232	2003002	枯草
--233	2003003	糟糠
--311	2003004	角料
--312	2612717	青砂
--313	1000001	[药]活血丹
--321	1000101	[药]回气散
--322	2612719	银粉
--323	1000101	[药]回气散
--331	2612727	还童丹
--332	2611017	《灵兽指南》
--333	2612722	含笑丸

--使用化气散2612720
--function i2612720_CanUse(MapID, InstanceID, TypeID, TargetID)
--    local bRet, bIgnore = 0, false
	--返回
--	return bRet, bIgnore
--end

--使用效果
--function i2612720_QuestUsable(MapID, InstanceID, TypeID, TargetID)
--       unit.AddBuff(MapID, InstanceID, TargetID, 2018501, TargetID)
--end

--注册
--aux.RegisterItemEvent(2612720, 1, "i2612720_QuestUsable")
--aux.RegisterItemEvent(2612720, 0, "i2612720_CanUse")



--使用大力丸2612721
--function i2612721_CanUse(MapID, InstanceID, TypeID, TargetID)
--    local bRet, bIgnore = 0, false
	--返回
--	return bRet, bIgnore
--end

--使用效果
--function i2612721_QuestUsable(MapID, InstanceID, TypeID, TargetID)
--       unit.AddBuff(MapID, InstanceID, TargetID, 2018601, TargetID)
--end

--注册
--aux.RegisterItemEvent(2612721, 1, "i2612721_QuestUsable")
--aux.RegisterItemEvent(2612721, 0, "i2612721_CanUse")


--使用大力丸2612722
--function i2612722_CanUse(MapID, InstanceID, TypeID, TargetID)
--    local bRet, bIgnore = 0, false
	--返回
--	return bRet, bIgnore
--end

--使用效果
--function i2612722_QuestUsable(MapID, InstanceID, TypeID, TargetID)
--       unit.AddBuff(MapID, InstanceID, TargetID, 2018701, TargetID)
--end

--注册
--aux.RegisterItemEvent(2612722, 1, "i2612722_QuestUsable")
--aux.RegisterItemEvent(2612722, 0, "i2612722_CanUse")
