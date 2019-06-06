
--15242 任务过程中与NPC李月娘[3070128]对话获得物品大荒宝镜[2615127]
function x15242_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, Npcid, Npctypeid)
	if Npctypeid == 3070128 then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 2615127, 1, -1, 8, 420)
	end
end
--娉ㄥ唽
aux.RegisterQuestEvent(15242, 6, "x15242_OnNPCTalk")

