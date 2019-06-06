--1006504 无间营刀兵（螺祖之谷）进入战斗后召唤
--{1006536 无间营奴隶}
function c1006504_On_Enter_Combat(MapID, InstanceID, TargetID)
	-- 怪物进入战斗喊话
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
	--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, TargetID, 30070)
       end
       local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1006536, x, y, z+5)
	
	
end

aux.RegisterCreatureEvent(1006504,2,"c1006504_On_Enter_Combat")
