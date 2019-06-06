--怪物喊话脚本

-- 怪物进入战斗喊话/示警脚本
function c1005457_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20084)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1005457, 2, "c1005457_OnEnterCombat")

--怪物死亡喊话
function c1005457_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 20086)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	map.MapCreateColCreature(MapID, InstanceID, 1005458, 701, 21173, 678, 1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1728)	--玄寂已经出现于坐标[701, 678]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1005457, 4, "c1005457_OnDie")




