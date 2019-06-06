function shitu_OnTalk(MapID, InstanceID, QuestID, RoleID, NPCID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)

	if QuestID==8002 then
		map.MapCreateColCreature(MapID, -1, 1531301, x, y, z+3, 1, "")
	end

	if QuestID==8003 or QuestID==8004 then
		map.MapCreateCreature(MapID, -1, 1531302, x, y, z+3)
	end

	if QuestID==8005 or QuestID==8006 or QuestID==8007 or QuestID==8008 or QuestID==8009 then
		map.MapCreateCreature(MapID, -1, 1531303, x, y, z+3)
	end

	if QuestID==8010 or QuestID==8011 or QuestID==8012 then
		map.MapCreateCreature(MapID, -1, 1531304, x, y, z+3)
	end

	if QuestID==8013 or QuestID==8014 then
		map.MapCreateCreature(MapID, -1, 1531305, x, y, z+3)
	end

end

--зЂВс
aux.RegisterQuestEvent(8002, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8003, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8004, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8005, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8006, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8007, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8008, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8009, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8010, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8011, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8012, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8013, 6, "shitu_OnTalk")
aux.RegisterQuestEvent(8014, 6, "shitu_OnTalk")
