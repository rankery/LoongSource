--调查引蛊坛（3040621）获得相思虫（2613613）25%和思乡虫（2613612）25%或者BUFF被毒虫咬了（2018801）
function c3040621_OnCheck(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       local i = math.random(1,4)
       local a = role.GetRoleItemNum(RoleID, 2613613)
       local b = role.GetRoleItemNum(RoleID, 2613612)
       if i==1 and a<3 then
           role.AddRoleItem(MapID, InstanceID, RoleID, 2613613, 1, -1, 8, 420)
       elseif i==2 and b<3 then
           role.AddRoleItem(MapID, InstanceID, RoleID, 2613612, 1, -1, 8, 420)
       elseif i==3 or i==4 then
           unit.AddBuff(MapID, InstanceID, RoleID, 2018801, RoleID)
       end
end

aux.RegisterCreatureEvent(3040621, 6, "c3040621_OnCheck")

--使用相思虫2613613的效果2019101
--可否使用函数
function i2613613_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在巫水，则不能使用
	if MapID ~= 3017299919 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i2613613_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 2019101, TargetID)
end

--注册
aux.RegisterItemEvent(2613613, 1, "i2613613_QuestUsable")
aux.RegisterItemEvent(2613613, 0, "i2613613_CanUse")

--使用思乡虫2613612的效果2019001
function i2613612_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在巫水，则不能使用
	if MapID ~= 3017299919 then
	    bRet = 43
	end

	--返回
	return bRet, bIgnore
end

--使用效果
function i2613612_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 2019001, TargetID)
end

--注册
aux.RegisterItemEvent(2613612, 1, "i2613612_QuestUsable")
aux.RegisterItemEvent(2613612, 0, "i2613612_CanUse")


--使用怪味包子2613614的效果3000601
function i2613614_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--返回
	return bRet, bIgnore
end

--使用效果
function i2613614_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       unit.AddBuff(MapID, InstanceID, TargetID, 3000601, TargetID)
end

--注册
aux.RegisterItemEvent(2613614, 1, "i2613614_QuestUsable")
aux.RegisterItemEvent(2613614, 0, "i2613614_CanUse")
