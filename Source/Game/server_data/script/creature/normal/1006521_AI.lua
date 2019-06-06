--1006521	铁磨营鬼师（螺祖之谷）进入战斗后召唤
--<随机召唤其中两个小鬼,出现在[左前]、[右前]>
--  {1006551 小鬼A}
--  {1006552 小鬼B}
--  {1006553 小鬼C}
--  {1006554 小鬼D}
function c1006521_On_Enter_Combat(MapID, InstanceID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

    --随机选取一个随机数
	local i = math.random(1,4)

	if i == 1 then
		map.MapCreateCreature(MapID, InstanceID, 1006551, x-5, y, z+5)
	elseif i == 2 then
		map.MapCreateCreature(MapID, InstanceID, 1006552, x-5, y, z+5)
	elseif i == 3 then
		map.MapCreateCreature(MapID, InstanceID, 1006553, x-5, y, z+5)
	elseif i == 4 then
		map.MapCreateCreature(MapID, InstanceID, 1006554, x-5, y, z+5)
	end

	--刷第二个不一样的小怪
	local k = math.random(1,4)
	while k == i do
	    k = math.random(1,4)
	end

	if k == 1 then
		map.MapCreateCreature(MapID, InstanceID, 1006551, x+5, y, z+5)
	elseif k == 2 then
		map.MapCreateCreature(MapID, InstanceID, 1006552, x+5, y, z+5)
	elseif k == 3 then
		map.MapCreateCreature(MapID, InstanceID, 1006553, x+5, y, z+5)
	elseif k == 4 then
		map.MapCreateCreature(MapID, InstanceID, 1006554, x+5, y, z+5)
	end

end
aux.RegisterCreatureEvent(1006521,2,"c1006521_On_Enter_Combat")
