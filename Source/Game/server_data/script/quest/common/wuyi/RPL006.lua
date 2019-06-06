

function q14406_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050155 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1007165, x+2, y, z+2)
	elseif Npctypeid == 3050156 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1007165, x+2, y, z+2)
	elseif Npctypeid == 3050157 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1007165, x+2, y, z+2)
	elseif Npctypeid == 3050158 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1007165, x+2, y, z+2)
	end
end

--зЂВс
aux.RegisterQuestEvent(14406, 6, "q14406_OnNPCTalk")
