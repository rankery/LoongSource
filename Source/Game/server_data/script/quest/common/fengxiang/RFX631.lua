--兽人首领：11631
--需求1：使用物品兽人阶级图[2611315]后接取任务兽人首领[11631]

--物品兽人阶级图效果脚本函数
function i2611315_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 11631)
end

--注册
aux.RegisterItemEvent(2611315, 1, "i2611315_QuestGiver")
