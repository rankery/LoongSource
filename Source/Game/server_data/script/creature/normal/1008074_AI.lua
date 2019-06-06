-- 怪物进入战斗喊话/示警
function c1008074_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 40046)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008074, 2, "c1008074_OnEnterCombat")

--·击杀 贪金 1008074 后，在其旁边刷出 贪金宝箱 3060026 -----------------------------待修改
function c1008074_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3060026, 2437, 26387, 731)
	--怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 40047)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008074, 4, "c1008074_OnDie")





