--鱼人鳍
--秘境鱼人掉落，拾取唯一，使用后获得游泳加速BUFF2016601

function I2614601_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	unit.AddBuff(MapID, InstanceID, TargetID, 2016601, TargetID)
end

aux.RegisterItemEvent(2614601, 1, "I2614601_QuestUsable")
