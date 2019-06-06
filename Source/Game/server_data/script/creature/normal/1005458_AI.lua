--击杀 玄寂 1005458 在 c01 导航点 刷出 蚩尤恶灵1005459

-- 怪物进入战斗喊话/示警脚本
function c1005458_OnEnterCombat(MapID, InstanceID, CreatureID)
	cre.MonsterSay(MapID, InstanceID, CreatureID, 20088)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1005458, 2, "c1005458_OnEnterCombat")

--怪物死亡时函数
function c1005458_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 20090)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1005459, x+2, y, z+2, 1, "c01")
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
--注册函数
aux.RegisterCreatureEvent(1005458, 4, "c1005458_OnDie")







