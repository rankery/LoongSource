--RKL124
--需求1：使用小鬼密函2615036获得任务15124
--使用效果
function i2615036_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddQuest(TargetID, 15124)
end

--注册
aux.RegisterItemEvent(2615036, 1, "i2615036_QuestUsable")
