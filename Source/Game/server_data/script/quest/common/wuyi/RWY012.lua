--RWY012 断魂毒草
--需求1：接取任务后，每次调查地物离愁断魂草，并获得道具离愁断魂草之后，获得一个瞬间BUFF断魂 之毒2015901，瞬间损失10%的生命值

function x3050031_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 2015901, RoleID)
end

aux.RegisterCreatureEvent(3050031, 6, "x3050031_OnInvest")

