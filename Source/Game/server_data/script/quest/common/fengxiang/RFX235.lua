--玉髓芝：11235
--需求1：使用物品悬赏布告-叁[2611124]后接取任务玉髓芝[11235]

--物品悬赏布告使用效果函数
function i2611124_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 11235)
end

--注册
aux.RegisterItemEvent(2611124, 1, "i2611124_QuestGiver")
