-- 怪物进入战斗喊话
function c1008084_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40052)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008084, 2, "c1008084_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008084 刷出 干戍奇煞狂魔君 1008085 坐标：x 2820 y 26387 z 302
function c1008084_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008085, 2820, 26387, 302)
end
aux.RegisterCreatureEvent(1008084, 4, "c1008084_OnDie")



