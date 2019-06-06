

function q14512_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3050180 then
	      role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14512, 2614227, 1, -1, 102)
	end
end

--зЂВс
aux.RegisterQuestEvent(14512, 6, "q14512_OnNPCTalk")
