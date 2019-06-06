-- 怪物进入战斗喊话/示警脚本
function c1006531_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 30101)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1006531, 2, "c1006531_OnEnterCombat")



--死亡时脚本
--击杀 幽量鬼将[1006531] 和 拔舌鬼将[1006532]之后，刷新土巴丹[1006533]

function c1006531_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       --怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 30102)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
--注册
aux.RegisterCreatureEvent(1006531, 4, "c1006531_OnDie")




