--怪物土巴丹脚本——ID：1006533/进入战斗示警脚本
--死亡时在同地图坐标<774, 6482, 1474>刷新出怪物阴煞尸王
--1006533	土巴丹（螺祖之谷）进入 战斗后召唤
--{1006546 无间营勇士 x 1460 y 10168 z 1397
--{1006549 阿鼻营勇士 x 1460 y 10168 z 1405
--{1006550 寒冰营勇士 x 1460 y 10168 z 1414
function c1006533_On_Enter_Combat(MapID, InstanceID, CreatureID)
	-- 怪物进入战斗喊话
	cre.MonsterSay(MapID, InstanceID, CreatureID, 30106)
       map.MapCreateCreature(MapID, InstanceID, 1006546, 1460, 10168, 1397)
	map.MapCreateCreature(MapID, InstanceID, 1006549, 1460, 10168, 1405)
	map.MapCreateCreature(MapID, InstanceID, 1006550, 1460, 10168, 1414)
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
--注册
aux.RegisterCreatureEvent(1006533,2,"c1006533_On_Enter_Combat")


--死亡时脚本
function c1006533_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       --怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 30107)
	map.MapCreateCreature(MapID, InstanceID, 1006534, 774, 6482, 1474)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

end
--注册
aux.RegisterCreatureEvent(1006533, 4, "c1006533_OnDie")






