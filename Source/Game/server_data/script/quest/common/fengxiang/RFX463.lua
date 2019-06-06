--上古之石：11463
--需求1：接取任务时添加BUFF身轻如燕[2011501]

--任务接取时函数
function x11463_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2011501, ownerID)
end

--注册
aux.RegisterQuestEvent(11463, 0, "x11463_OnAccept")
