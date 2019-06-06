
-- 怪物进入战斗喊话/示警脚本
function c1006530_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 30099)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1006530, 2, "c1006530_OnEnterCombat")

--死亡时在在地图坐标<1129, 5696, 975>刷新出怪物密探卯蚩
--死亡时函数
function c1006530_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--怪物死亡喊话
	cre.MonsterSay(MapID, InstanceID, TargetID, 30100)
	map.MapCreateCreature(MapID, InstanceID, 3040522, 1129, 5696, 975)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local Boss_num=map.GetMapScriptData(MapID, InstanceID, 1, 4)+100
	map.SetMapScriptData(MapID, InstanceID, 1, 4, Boss_num)

	if Boss_num==1111 then
		map.MapCreateCreature(MapID, InstanceID, 1006533, 1509, 10460, 1408)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1746)	--土巴丹已经出现于坐标[1509, 1408]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

--注册
aux.RegisterCreatureEvent(1006530, 4, "c1006530_OnDie")



