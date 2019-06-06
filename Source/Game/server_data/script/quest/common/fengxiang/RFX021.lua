--运送肥料：11021
--需求1：接取任务时添加BUFF负重[2011101]
--需求2：删除或完成任务时删除BUFF负重[2011101]

--任务接取时
function x11021_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011101, ownerID)
end

--任务完成时
function x11021_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2011101)
end

--任务放弃时
function x11021_OnCancel(mapID, instanceID, questID, ownerID, enderID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2011101)
end

--注册
aux.RegisterQuestEvent(11021, 0, "x11021_OnAccept")
aux.RegisterQuestEvent(11021, 1, "x11021_OnComplete")
aux.RegisterQuestEvent(11021, 2, "x11021_OnCancel")
