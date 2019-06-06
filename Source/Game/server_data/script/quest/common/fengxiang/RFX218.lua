--疯狂报复：11218
--需求1：接取任务时给予BUFF仇火滔天[2011301]

--任务接取时
function x11218_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011301, ownerID)
end

--注册
aux.RegisterQuestEvent(11218, 0, "x11218_OnAccept")
