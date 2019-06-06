--出征：13629
--需求1：调查祭坛[3040420]后原地[m05: 1733, 26804, 722]刷新出怪物通灵鬼师[1006412]

--地物祭坛被调查函数
function c3040420_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1006412, 1733, 26804, 722, 1, "g12")
end

--注册
aux.RegisterCreatureEvent(3040420, 6, "c3040420_OnInvest")
