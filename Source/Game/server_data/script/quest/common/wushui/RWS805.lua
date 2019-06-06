--白玉鼠：13805
--需求1：与NPC黑猫[3040611]对话后获得道具白玉鼠[2613610]

--NPC对话函数
function x13805_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3040611 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2613610, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(13805, 6, "x13805_OnNPCTalk")

    