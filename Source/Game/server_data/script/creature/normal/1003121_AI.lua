--怪物喊话/示警脚本

-- 怪物进入战斗喊话
function c1003121_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 80)
	end
	   --得到怪物的当前目标
       local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
       --同步仇恨给范围为8个格子，高度为20的友方怪物
       g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1003121, 2, "c1003121_OnEnterCombat")

--怪物死亡
function c1003121_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 1, "jinjieshili")
end

aux.RegisterCreatureEvent(1003121, 4, "c1003121_OnDie")
