--怪物喊话脚本

-- 怪物进入战斗喊话
function c1005113_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 20008)
       end	
end

aux.RegisterCreatureEvent(1005113, 2, "c1005113_OnEnterCombat")

