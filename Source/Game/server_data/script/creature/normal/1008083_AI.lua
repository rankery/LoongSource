-- 怪物进入战斗喊话
function c1008083_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40051)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008083, 2, "c1008083_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008083 刷出 干戍奇煞狂魔君 1008084 坐标：x 2379 y 26387 z 737
function c1008083_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008084, 2379, 26387, 737)
end
aux.RegisterCreatureEvent(1008083, 4, "c1008083_OnDie")


