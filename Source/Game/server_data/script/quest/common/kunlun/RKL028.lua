--RKL028
--任务采集坚韧草[3070704],给玩家添加BUFF寒毒[2019701]

function c3070704_OnCheck(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    unit.AddBuff(MapID, InstanceID, RoleID, 2019701, RoleID)
end

--注册
aux.RegisterCreatureEvent(3070704, 6, "c3070704_OnCheck")
