--地脉火种：11629
--需求1：完成任务后获得BUFF浩气长存[2011801]

--任务完成时函数
function x11629_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011801, ownerID)
end

--注册
aux.RegisterQuestEvent(11629, 1, "x11629_OnComplete")
