--蹊跷阵图：11623
--需求1：完成任务后获得BUFF坚甲[2011701]

--任务完成时函数
function x11623_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011701, ownerID)
end

--注册
aux.RegisterQuestEvent(11623, 1, "x11623_OnComplete")
