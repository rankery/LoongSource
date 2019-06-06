--猫口夺履：12645
--需求1：与NPC黑猫[3030603]对话后获得物品裴青云的鞋子[2612704]

--黑猫对话脚本函数
function x12645_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3030603 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2612704, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(12645, 6, "x12645_OnNPCTalk")

