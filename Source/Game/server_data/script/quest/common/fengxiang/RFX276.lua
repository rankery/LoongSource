--恶魂枭吴：11276
--需求1：接取任务时传送至副本q03[q03: 190, 3460, 165]

--任务接取时函数
function x11276_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826990119, 190, 3460, 165)
end

--注册
aux.RegisterQuestEvent(11276, 0, "x11276_OnAccept")
