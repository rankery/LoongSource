--RWYMR08：14308伏兵何惧
--需求1：调查地物[火药箱][3060023]后，有50%几率刷新怪物[天绝伏兵][1007109]1个

function q14308_OnInvest(MapID, InstanceID, QuestID, RoleID, NPCID)
	local Rate = math.random(1, 100)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	--每次调查都减少一个物品[药粉盒子][2614036]
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2614036, 1, 101)
	if Rate < 51 then
	map.MapCreateCreature(MapID, InstanceID, 1007109, x+2, y, z+2)
	end
end

aux.RegisterQuestEvent(14308, 9, "q14308_OnInvest")
