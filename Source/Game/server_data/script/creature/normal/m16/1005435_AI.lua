--1005435 独足（礼天殿[前]）进入战斗后召唤
    --{1005486 召唤法}
	--{1005476 单体战}
	--{1005472 召唤}/示警脚本
function c1005435_On_Enter_Combat(MapID, InstanceID, CreatureID)
	-- 怪物进入战斗喊话
	cre.MonsterSay(MapID, InstanceID, CreatureID, 20071)

       local x, y ,z = unit.GetPosition(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 1005486, x-5, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1005476, x+5, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1005472, x, y, z+5)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end
aux.RegisterCreatureEvent(1005435,2,"c1005435_On_Enter_Combat")

--怪物死亡喊话
function c1005435_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")


end

aux.RegisterCreatureEvent(1005435, 4, "c1005435_OnDie")


