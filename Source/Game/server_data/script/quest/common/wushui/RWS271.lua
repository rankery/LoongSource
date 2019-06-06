--奇兵之计：13271
--需求1：与三个地物树桩[3040233, 3040234, 3040235]对话后分别删除一个绳索[2613141]

--任务NPC对话函数
function x13271_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3040233 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613141, 1, 102)
	elseif npctypeid == 3040234 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613141, 1, 102)
	elseif npctypeid == 3040235 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613141, 1, 102)
	end
end

--注册
aux.RegisterQuestEvent(13271, 6, "x13271_OnNPCTalk")




