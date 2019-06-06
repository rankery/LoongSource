--山神喊话脚本

-- 怪物进入战斗喊话
function ShanShen_OnEnterCombat(MapID, InstanceID, CreatureID)
	cre.MonsterSay(MapID, InstanceID, CreatureID, 89)
end

aux.RegisterCreatureEvent(1534001, 2, "ShanShen_OnEnterCombat")
aux.RegisterCreatureEvent(1534002, 2, "ShanShen_OnEnterCombat")
aux.RegisterCreatureEvent(1534003, 2, "ShanShen_OnEnterCombat")
aux.RegisterCreatureEvent(1534004, 2, "ShanShen_OnEnterCombat")
aux.RegisterCreatureEvent(1534005, 2, "ShanShen_OnEnterCombat")
aux.RegisterCreatureEvent(1534006, 2, "ShanShen_OnEnterCombat")

