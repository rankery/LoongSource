--疯癫强盗：10654
--需求1：接取任务10654后获得BUFF2015501

function x10654_OnAccept(mapID, instanceID, questID, ownerID, accepterID)

	unit.AddBuff(mapID, instanceID, ownerID, 2015501, ownerID)
end

--注册
aux.RegisterQuestEvent(10654, 0, "x10654_OnAccept")
