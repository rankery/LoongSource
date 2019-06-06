--RWY025 搜集证据-叁
--需求1：与地物NPC神秘箱子对话完成后，获得道具魔族的证据

function x14025_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3050022 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2614016, 1, 0, 102)
	end
end

aux.RegisterQuestEvent(14025, 6, "x14025_OnNPCTalk")

