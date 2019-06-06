

function q14409_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050159 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 3050146, x+2, y, z+2)
	end
end

--зЂВс
aux.RegisterQuestEvent(14409, 6, "q14409_OnNPCTalk")
