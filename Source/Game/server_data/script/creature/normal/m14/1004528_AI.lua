--怪物喊话脚本/示警脚本

-- 怪物进入战斗喊话
function c1004528_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 10061)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1004528, 2, "c1004528_OnEnterCombat")

--怪物死亡喊话
function c1004528_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 10063)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,10)+1000
	map.SetMapScriptData(MapID,InstanceID,1,10,Boss_num)

	if Boss_num==1111 then
		local HuanJiao_ID=map.GetMapScriptData(MapID,InstanceID,1,11)
		unit.CancelBuff(MapID, InstanceID, HuanJiao_ID, 9422601)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1737)	--妖邪祭祀幻角的无敌状态已解除！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1004528, 4, "c1004528_OnDie")


