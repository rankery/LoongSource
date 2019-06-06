--RPL239
--需求1：使用情报上部2614247获得任务15239
--使用效果
function i2614247_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.AddQuest(TargetID, 14539)
end

--注册
aux.RegisterItemEvent(2614247, 1, "i2614247_QuestUsable")
