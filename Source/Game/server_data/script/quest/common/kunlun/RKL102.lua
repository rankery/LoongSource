--RKL102
--需求1：使用狼妖内丹2615028获得任务15102

--使用效果
function i2615028_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddQuest(TargetID, 15102)
end

--注册
aux.RegisterItemEvent(2615028, 1, "i2615028_QuestUsable")
