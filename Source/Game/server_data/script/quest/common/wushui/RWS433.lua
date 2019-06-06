--慰问勇士：13433
--需求1：与苗族勇士甲，乙，丙，丁[3040315, 3040316, 3040317, 3040318]对话后分别删除一个五色饭[2613222]

--NPC对话函数
function x13433_OnNPCTalk(mapID, instanceID, questID, ownerID, npcid, npctypeid)
	if npctypeid == 3040315 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613222, 1, 102)
	elseif npctypeid == 3040316 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613222, 1, 102)
	elseif npctypeid == 3040317 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613222, 1, 102)
	elseif npctypeid == 3040318 then
		role.RemoveFromRole(mapID, instanceID, ownerID, 2613222, 1, 102)
	end
end

--注册
aux.RegisterQuestEvent(13433, 6, "x13433_OnNPCTalk")

