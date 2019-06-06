--·开启 英灵宝箱 3060035 刷出 英灵亡魂 1008033 / 播放特效 txxy03 坐标：x 1132 y 14867 z 519
function c3060035_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008033, 1132, 14867, 519)
end
aux.RegisterCreatureEvent(3060035, 6, "c3060035_OnInvest")
