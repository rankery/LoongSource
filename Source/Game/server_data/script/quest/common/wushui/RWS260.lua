--埋藏火药：13260
--需求1：与三个土包[3040228, 3040229, 3040230]对话后分别删除一个一包火药[2613133]

--任务NPC对话函数
function x13260_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3040228 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613133, 1, 102)
	elseif npctypeid == 3040229 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613133, 1, 102)
	elseif npctypeid == 3040230 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613133, 1, 102)
	end
end

--注册
aux.RegisterQuestEvent(13260, 6, "x13260_OnNPCTalk")

