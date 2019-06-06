--物归原主：10440
--使用物品破损的头带[2610173]，获得任务物归原主[10440]

--物品破损的头带使用效果脚本
function i2610173_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	role.AddQuest(TargetID, 10440)
end

--注册
aux.RegisterItemEvent(2610173, 1, "i2610173_QuestGiver")
