--·濂虐魔将 1008039 进入战斗后，刷新 侍魔小妖 1008040 坐标：x 1471 y 15188 z 380
--			       刷新 侍魔小妖 1008040 坐标：x 1469 y 15188 z 418
--		           刷新 侍魔小妖 1008040 坐标：x 1434 y 15188 z 417
--			       刷新 侍魔小妖 1008040 坐标：x 1434 y 15188 z 382

function c1008039_On_Enter_Combat(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 1008040, 1471, 15188, 380)
	map.MapCreateCreature(MapID, InstanceID, 1008040, 1469, 15188, 418)
	map.MapCreateCreature(MapID, InstanceID, 1008040, 1434, 15188, 417)
	map.MapCreateCreature(MapID, InstanceID, 1008040, 1434, 15188, 382)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008039,2,"c1008039_On_Enter_Combat")


--·杀死 濂虐魔将 1008039 刷出 传送石 3060028 / 播放特效 txcj09 坐标：x 1488 y 15341 z 399
function c1008039_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3060028, 1488, 15341, 399)

	--刷新祭魔台
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531103, x, y, z)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008039, 4, "c1008039_OnDie")
