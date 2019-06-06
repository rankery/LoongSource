--RWY274：脊骨钥匙
--需求1：完成任务后奖励时限道具"脊骨钥匙"2614118，使用后进入副本：上古魔殿

--完成任务后奖励时限道具"脊骨钥匙"2614118
function q14274_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	role.QuestAddRoleItem(MapID, InstanceID, RoleID, 14274, 2614118, 1, -1, 101)

	--增加进阶实力值
		Increase_shili(MapID, InstanceID, RoleID, 0, 2, "jinjieshili_Q")


end


aux.RegisterQuestEvent(14274, 1, "q14274_OnComplete")
