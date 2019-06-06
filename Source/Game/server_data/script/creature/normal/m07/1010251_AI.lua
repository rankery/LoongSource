--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010251_OnEnterCombat(MapID, InstanceID, CreatureID)
        local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
	      cre.MonsterSay(MapID, InstanceID, CreatureID, 50056)
	end
end

aux.RegisterCreatureEvent(1010251, 2, "c1010251_OnEnterCombat")

--怪物死亡
function c1010251_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010251, 4, "c1010251_OnDie")

