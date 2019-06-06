--分发名帖：12001
--需求1：与NPC桑干[3030002]，艾万多[3030003]，古开明[3030004]对话后分别获得一个任务物品回信[2612023]

--NPC对话函数
function x12001_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3030002 or npctypeid == 3030003 or npctypeid == 3030004 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2612023, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(12001, 6, "x12001_OnNPCTalk")

