--四象灵石：13802
--需求1：调查四个土包[3040601, 3040602, 3040603, 3040604]后分别刷新出怪物火灵君[1006603]，水灵君[1006604]，雷灵君[1006605]，土灵君[1006606]
--坐标：
--[m05: 1664, 17345, 1565]
--[m05: 2441, 16878, 2761]
--[m05: 1999, 24790, 1408]
--[m05: 2586, 25637, 1695]

--土包1被调查函数
function c3040601_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006603, 1664, 17345, 1565)    --火灵君
end

--土包2被调查函数
function c3040602_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006604, 2441, 16878, 2761)    --水灵君
end

--土包3被调查函数
function c3040603_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006605, 1999, 24790, 1408)    --雷灵君
end

--土包4被调查函数
function c3040604_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006606, 2586, 25637, 1695)    --土灵君
end

--注册
aux.RegisterCreatureEvent(3040601, 6, "c3040601_OnInvest")
aux.RegisterCreatureEvent(3040602, 6, "c3040602_OnInvest")
aux.RegisterCreatureEvent(3040603, 6, "c3040603_OnInvest")
aux.RegisterCreatureEvent(3040604, 6, "c3040604_OnInvest")
