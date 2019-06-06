
--RWS107
--需求1：与客栈老板[3040412]对话后任务背包添加物品一包调料[2613624]

--NPC对话函数
function x13107_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
    --检查是否为客栈老板
	if npctypeid == 3040412 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2613624, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(13107, 6, "x13107_OnNPCTalk")

