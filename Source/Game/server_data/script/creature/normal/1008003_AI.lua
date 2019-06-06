-- 怪物进入战斗喊话
function c1008003_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40023)
	end
end
aux.RegisterCreatureEvent(1008003, 2, "c1008003_OnEnterCombat")

--杀死秘境鱼人 1008003 几率给游泳buff2016601

function c1008003_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 2016601, RoleID)
end
aux.RegisterCreatureEvent(1008003, 4, "c1008003_OnDie")



