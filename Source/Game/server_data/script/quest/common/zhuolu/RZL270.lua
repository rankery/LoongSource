--狼吞虎咽：12270
--需求1：调查毒石[3030141]后删除纯净泉水[2612144]，得空泉水瓶[2612143]

--地物火堆被调查函数
function q12270_OnInvest(MapID, InstanceID, QuestID, RoleID, NPCID)

	role.RemoveFromRole(MapID, InstanceID, RoleID, 2612144, 1, 102)
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 12270, 2612143, 1, -1, 101)
end
aux.RegisterQuestEvent(12270, 9, "q12270_OnInvest")
