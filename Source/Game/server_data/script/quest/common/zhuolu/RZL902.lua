--RZL902
--需求1：与东方天元[3010082]对话后任务背包添加物品将人服饰[3200510]

--NPC对话函数
function x12902_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
    --检查是否为东方天元
	if npctypeid == 3010082 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 3200510, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(12902, 6, "x12902_OnNPCTalk")

