

function q14415_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050195 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 3050168, x+2, y, z+2)
	elseif Npctypeid == 3050196 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 3050169, x+2, y, z+2)
	elseif Npctypeid == 3050197 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 3050170, x+2, y, z+2)
	end
end

--зЂВс
aux.RegisterQuestEvent(14415, 6, "q14415_OnNPCTalk")
