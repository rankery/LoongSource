--怪物喊话脚本


--怪物死亡喊话
function c1008045_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, TargetID, 40040)
       end

end

aux.RegisterCreatureEvent(1008045, 4, "c1008045_OnDie")

