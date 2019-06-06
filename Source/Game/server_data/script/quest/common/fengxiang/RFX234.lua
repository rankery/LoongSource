--炸毁水寨：11234
--需求1：使用物品悬赏布告-贰[2611123]后接取任务炸毁水寨[11234]

--物品悬赏布告使用效果函数
function i2611123_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 11234)
end

--注册
aux.RegisterItemEvent(2611123, 1, "i2611123_QuestGiver")
