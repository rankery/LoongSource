--调查铁锅3060043，失去鲜鱼2614055，获得鲜美的鱼汤2614056

function c3060043_OnCheck(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
 	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14105, 2614056, 1, 0, 102)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614055, 1, 420)	
end

--注册
aux.RegisterCreatureEvent(3060043, 6, "c3060043_OnCheck")
