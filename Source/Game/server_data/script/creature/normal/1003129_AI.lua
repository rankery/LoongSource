--怪物喊话脚本

-- 怪物进入战斗喊话
function c1003129_OnEnterCombat(MapID, InstanceID, CreatureID)
         
       cre.MonsterSay(MapID, InstanceID, CreatureID, 23)
       	
end

aux.RegisterCreatureEvent(1003129, 2, "c1003129_OnEnterCombat")

--怪物死亡喊话
function c1003129_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, TargetID, 24)
       end	

end

aux.RegisterCreatureEvent(1003129, 4, "c1003129_OnDie")
