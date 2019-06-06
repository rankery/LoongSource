--重回平静：12017
--需求1：与地物石碑[3030017]对话后删除任务物品阳符[2612012]

--NPC对话函数
function x12017_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3030017 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2612012, 1, 101)
	end
end

--注册
aux.RegisterQuestEvent(12017, 6, "x12017_OnNPCTalk")

