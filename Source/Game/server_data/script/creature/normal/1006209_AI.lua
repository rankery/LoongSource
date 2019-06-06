--怪物喊话脚本

-- 怪物进入战斗喊话
function c1006209_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 30010)

end

aux.RegisterCreatureEvent(1006209, 2, "c1006209_OnEnterCombat")



--[[怪物死亡喊话
function c1006209_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

             cre.MonsterSay(MapID, InstanceID, TargetID, 30011)
	local a = map.GetMapScriptData(MapID,InstanceID,1,6) --副本Data索引6
	map.SetMapScriptData(MapID,InstanceID,1,6,a-1)
end

aux.RegisterCreatureEvent(1006209, 4, "c1006209_OnDie")]]

