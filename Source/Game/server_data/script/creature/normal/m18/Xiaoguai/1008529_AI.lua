--怪物喊话脚本

-- 怪物进入战斗喊话
function c1008529_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40074)
       end	
end

aux.RegisterCreatureEvent(1008529, 2, "c1008529_OnEnterCombat")

