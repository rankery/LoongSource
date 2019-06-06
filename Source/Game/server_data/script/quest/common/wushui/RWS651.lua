--《离思》：13651
--需求1：使用物品《离思三首》[2613017]后接到任务《离思》[13651]

--物品离思使用效果函数
function i2613017_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 13651)
end

--注册
aux.RegisterItemEvent(2613017, 1, "i2613017_QuestGiver")
