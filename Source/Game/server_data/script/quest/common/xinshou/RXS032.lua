--完美的龙鳞：32
--需求1：使用物品完美的大龙鳞[1330402]后接到任务完美的龙鳞[32]
--需求2：玩家完成任务后获得体力真气恢复全满BUFF[2013201]

--物品完美的大龙鳞使用效果函数
function i1330402_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 32)
end

--注册
aux.RegisterItemEvent(1330402, 1, "i1330402_QuestGiver")

--任务完成时
function x032_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.AddBuff(mapID, instanceID, ownerID, 2013201, ownerID)
end

--注册
aux.RegisterQuestEvent(32, 1, "x031_OnComplete")

