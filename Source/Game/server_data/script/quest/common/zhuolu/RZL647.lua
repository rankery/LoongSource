--五里牌坊：12647
--需求1：接受任务后玩家被传送到第一道牌坊顶端[m04: 1803, 14602, 1816]
--需求2：接受任务后玩家获得BUFF身轻如燕[2012001]

--任务接取函数
function x12647_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, ownerID, 2012001, ownerID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017299663, 1803, 14602, 1816)
end

aux.RegisterQuestEvent(12647, 0, "x12647_OnAccept")

