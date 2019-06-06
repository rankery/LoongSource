

-- 怪物进入战斗触发示警
function c1006729_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为3个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1006729, 2, "c1006729_OnEnterCombat")

--需求8  击杀 1006729 后，给 1006720 添加防御增强buff(2017601)防御加50%


function c1006729_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--unit.AddBuff(MapID, InstanceID, 1006720, 2017601, 1006720)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 10)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 11)+8
	map.SetMapScriptData(MapID, InstanceID, 1, 11, Boss_num)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1757)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end
--注册
aux.RegisterCreatureEvent(1006729, 4, "c1006729_OnDie")
