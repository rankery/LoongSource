--隐藏的阴谋：11444
--需求1：使用物品万毒教密函[2611230]后接取任务隐藏的阴谋[11444]

--物品万毒教密函使用效果函数
function i2611230_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 11444)
end

--注册
aux.RegisterItemEvent(2611230, 1, "i2611230_QuestGiver")
