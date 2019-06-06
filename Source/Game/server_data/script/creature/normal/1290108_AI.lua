--怪物喊话脚本

-- 怪物进入战斗喊话
function c1290108_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20109)
end

aux.RegisterCreatureEvent(1290108, 2, "c1290108_OnEnterCombat")



