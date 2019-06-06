--困境救援：10219
--需求1：接受任务后传送至副本q01[q01：154, 5, 35]

--任务接取时函数
function x10219_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826990631, 154, 5, 35)
end

--注册
aux.RegisterQuestEvent(10219, 0, "x10219_OnAccept")
