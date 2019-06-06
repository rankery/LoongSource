--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003108_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 37)
       end	
end

aux.RegisterCreatureEvent(1003108, 2, "c1003108_OnEnterCombat")

