--·调查 贪金宝箱 3060026 有30%几率刷出 干戍奇煞狂魔君 1008081 坐标：x 2600 y 25797 z 447
function c3060026_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = math.random(1,100)
	if a > 70 then
	map.MapCreateCreature(MapID, InstanceID, 1008081, 2600, 25797, 447)
	end
end
aux.RegisterCreatureEvent(3060026, 6, "c3060026_OnInvest")
