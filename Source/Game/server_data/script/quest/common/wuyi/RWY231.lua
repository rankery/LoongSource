--RWY231前往凤凰阁
--交任务后，获得增加跳跃能力的BUFF2016501

function q14231_OnComplete(MapID, InstanceID, QuestID, OwnerID, NpcID)
	unit.AddBuff(MapID, InstanceID, OwnerID, 2016501, OwnerID)
end
aux.RegisterQuestEvent(14231, 1, "q14231_OnComplete")



