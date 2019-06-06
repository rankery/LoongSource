

function q14419_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050210 then
		role.RemoveFromRole(MapID, InstanceID, RoleID, 2614205, 1, 300)
		unit.AddBuff(MapID, InstanceID, RoleID, 2011101, RoleID)
	end
end

--зЂВс
aux.RegisterQuestEvent(14419, 6, "q14419_OnNPCTalk")
