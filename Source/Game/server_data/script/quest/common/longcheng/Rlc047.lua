--龙魂之威：10047
--需求1：接受任务后获得BUFF仙法·龙魂[]
--需求2：接受任务后传送至副本q02[q02: 154, 5, 35]

--任务接取时函数
function x10047_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826990375, 154, 5, 35)
end

--注册
aux.RegisterQuestEvent(10047, 0, "x10047_OnAccept")

