--怪物喊话脚本

-- 怪物进入战斗喊话
function c1004113_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 10009)
       end	
end

aux.RegisterCreatureEvent(1004113, 2, "c1004113_OnEnterCombat")

--怪物死亡喊话
function c1004113_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, TargetID, 10010)
       end	

end

aux.RegisterCreatureEvent(1004113, 4, "c1004113_OnDie")

