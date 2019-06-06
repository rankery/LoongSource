--无瑕的龙鳞：31
--需求1：使用物品无瑕的小龙鳞[1330401]后接到任务无瑕的龙鳞[31]
--需求2：玩家完成任务后获得体力真气恢复全满BUFF[2013201]

--物品无暇的小龙鳞使用效果函数
function i1330401_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 31)
end

--注册
aux.RegisterItemEvent(1330401, 1, "i1330401_QuestGiver")

--任务完成时
function x031_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.AddBuff(mapID, instanceID, ownerID, 2013201, ownerID)
end

--注册
aux.RegisterQuestEvent(31, 1, "x031_OnComplete")
