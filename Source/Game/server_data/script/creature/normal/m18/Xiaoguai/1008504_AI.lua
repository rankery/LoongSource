
-- 怪物进入战斗触发示警
function c1008504_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008504, 2, "c1008504_OnEnterCombat")


--击杀千眼冥王(typeID：1008504)后，在(m18,569,33046,454)位置刷新typeID为1008507的碰撞怪物（若必须添加导航点名称，请添加B03，场景中无此导航点，怪物将呆立）
function c1008504_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1008507, 569, 33046, 454, 1, "B03")
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008504, 4, "c1008504_OnDie")
