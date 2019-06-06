

function q14560_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050194 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1007259, x+2, y, z+2)
	end
end

--зЂВс
aux.RegisterQuestEvent(14560, 6, "q14560_OnNPCTalk")
