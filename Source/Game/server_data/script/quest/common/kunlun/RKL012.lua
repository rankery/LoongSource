--RKL012
--需求1：与紫葳真人[3070005]对话后给予BUFF聆听猫语[2020201]

--NPC对话函数
function x15012_OnNPCTalk(MapID, InstanceID, QuestID, OwnerID, NpcID, NpcTypeID)
    --检查是否为紫葳真人
	if npctypeid == 3070005 then
		unit.AddBuff(MapID, InstanceID, OwnerID, 2020001, OwnerID)
	end
end

--注册
aux.RegisterQuestEvent(15012, 6, "x15012_OnNPCTalk")

--任务完成时
function q15012_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2020001)
end

--注册
aux.RegisterQuestEvent(15012, 1, "q15012_OnComplete")
