--需求2：进入场景乾珠界[m102]，获得buff乾珠界修炼[2017801]
--需求6：只有在场景乾珠界[m102]中，才可使用道具三清祛瘴散[3200415]

function c03_1_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 20278)
	if bhave == false then
		unit.AddBuff(MapID, InstanceID, RoleID, 2027801, RoleID)
	end
end

--注册
aux.RegisterMapEvent("c03_1", 2, "c03_1_OnPlayerEnter")
