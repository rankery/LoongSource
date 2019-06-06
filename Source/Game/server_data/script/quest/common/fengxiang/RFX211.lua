--寻找货物：11211
--需求1：接受任务时添加BUFF龙腾[2011201]

--任务接取时
function x11211_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011201, ownerID)
end

--注册
aux.RegisterQuestEvent(11211, 0, "x11211_OnAccept")
