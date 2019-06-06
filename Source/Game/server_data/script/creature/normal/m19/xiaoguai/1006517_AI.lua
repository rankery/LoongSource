--怪物喊话脚本


--怪物死亡喊话
function c1006517_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, TargetID, 30083)
       end	

end

aux.RegisterCreatureEvent(1006517, 4, "c1006517_OnDie")

