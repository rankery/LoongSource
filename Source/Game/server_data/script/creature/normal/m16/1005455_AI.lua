--怪物喊话脚本

-- 怪物进入战斗喊话/示警脚本
function c1005455_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 20080)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1005455, 2, "c1005455_OnEnterCombat")


--怪物死亡喊话
function c1005455_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 20081)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,3)+100
	map.SetMapScriptData(MapID,InstanceID,1,3,Boss_num)

	map.MapCreateCreature(MapID, InstanceID, 3030428, 889, 22233, 471)

	if Boss_num==1111 then
		map.MapCreateColCreature(MapID, InstanceID, 1005457, 701, 21199, 477, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1727)	--拘奴已经出现于坐标[701, 477]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1005455, 4, "c1005455_OnDie")

