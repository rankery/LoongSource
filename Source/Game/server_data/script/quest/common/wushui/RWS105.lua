
--RWS105
--需求1：与海瑞[3040207]对话后任务背包添加物品鱼钩[2613617]

--NPC对话函数
function x13105_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
    --检查是否为海瑞
	if npctypeid == 3040207 then
		role.QuestAddRoleItem(mapID, instanceID, ownerID, questID, 2613617, 1, 0, 102)
	end
end

--注册
aux.RegisterQuestEvent(13105, 6, "x13105_OnNPCTalk")

