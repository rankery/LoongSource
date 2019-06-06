--狼吞虎咽：13275
--需求1：调查火堆[3040050]后删除鲜嫩的肉[2613030]，得到嫩汁烤肉[2613031]

--地物火堆被调查函数
function q13275_OnInvest(MapID, InstanceID, QuestID, RoleID, NPCID)

	role.RemoveFromRole(MapID, InstanceID, RoleID, 2613030, 1, 101)
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, QuestID, 2613031, 1, -1, 101)
end

aux.RegisterQuestEvent(13275, 9, "q13275_OnInvest")
