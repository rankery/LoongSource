--神奇的龙鳞：94
--需求1：使用物品神器的龙鳞[1330403]后接到任务神破娴牧踇31]
--需求2：玩家完成任务后获得体力真气恢复全满BUFF[2013201]

--物品无暇的小龙鳞使用效果函数
function i1330403_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 94)
end

--注册
aux.RegisterItemEvent(1330403, 1, "i1330403_QuestGiver")

--任务完成时
function x094_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.AddBuff(mapID, instanceID, ownerID, 2013201, ownerID)
end

--注册
aux.RegisterQuestEvent(94, 1, "x094_OnComplete")
