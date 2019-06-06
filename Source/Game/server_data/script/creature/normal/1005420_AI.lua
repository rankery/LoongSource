
-- 怪物进入战斗喊话/示警脚本
function c1005420_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20064)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end
aux.RegisterCreatureEvent(1005420, 2, "c1005420_OnEnterCombat")


-- 击杀 冰魄灵王 1005420， 刷出 招魂碑 3030402，坐标：x 272 y 5270 z 172
function c1005420_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 20066)
	map.MapCreateCreature(MapID, InstanceID, 3030402, 272, 5270, 172)

	--刷新祭魔台
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531101, x, y, z)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
aux.RegisterCreatureEvent(1005420, 4, "c1005420_OnDie")


