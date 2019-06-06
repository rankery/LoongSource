--九幽残余：13610
--需求1：接取任务后传送至副本巫峡苗寨[q06: 725, 4330, 319]

--任务接取时函数
function x13610_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826991399, 725, 4330, 319)
end

--注册
aux.RegisterQuestEvent(13610, 0, "x13610_OnAccept")

