--悬赏除贼：11233
--需求1：使用物品悬赏布告-壹[2611122]后接取任务悬赏除贼[11233]

--物品悬赏布告使用效果函数
function i2611122_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 11233)
end

--注册
aux.RegisterItemEvent(2611122, 1, "i2611122_QuestGiver")
