--怪物喊话脚本/示警脚本

-- 怪物进入战斗喊话
function c1006555_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30095)
			     --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1006555, 2, "c1006555_OnEnterCombat")

function c1006555_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local Boss_num=map.GetMapScriptData(MapID, InstanceID, 1, 4)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 4, Boss_num)

	if Boss_num==1111 then
		map.MapCreateCreature(MapID, InstanceID, 1006533, 1509, 10460, 1408)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1746)	--土巴丹已经出现于坐标[1509, 1408]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end


end
--注册
aux.RegisterCreatureEvent(1006555, 4, "c1006555_OnDie")



