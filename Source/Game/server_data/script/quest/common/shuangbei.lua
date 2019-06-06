--双倍经验：10848
--需求1：完成任务时添加BUFF双倍经验[2010401]

--任务提交时接口
function x10848_OnComplete(mapID, instanceID, questID, ownerID, accepterID)
    --给予双倍经验
	unit.AddBuff(mapID, instanceID, ownerID, 2010401, ownerID)
end

--注册
aux.RegisterQuestEvent(10848, 1, "x10848_OnComplete")
