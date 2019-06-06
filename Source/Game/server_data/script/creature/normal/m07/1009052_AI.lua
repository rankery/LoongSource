-- 怪物进入战斗喊话
function c1009052_OnEnterCombat(MapID, InstanceID, CreatureID)
        local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
	      cre.MonsterSay(MapID, InstanceID, CreatureID, 50014)
	end
end

aux.RegisterCreatureEvent(1009052, 2, "c1009052_OnEnterCombat")


--1009052死亡时，如果有物品2615045，则刷出混沌妖1009054
function c1009052_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)	
	local i = role.GetRoleItemNum(RoleID, 2615045)
        if i == 1 then
	    local x,y,z = unit.GetPosition(MapID, InstanceID, RoleID)
	    map.MapCreateCreature(MapID, InstanceID, 1009054, x, y, z)
	end
end

aux.RegisterCreatureEvent(1009052, 4, "c1009052_OnDie")

