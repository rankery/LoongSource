--良心考验：13012
--需求1：接受任务后玩家被传送至三峡幻境副本中[q05: 492, 6781, 725]
--需求2：接受任务后玩家获得BUFF良心蛊[2612501]

--任务接取时函数
function x13012_OnAccept(mapID, instanceID, questID, ownerID, accepterID)
	unit.AddBuff(mapID, instanceID, unitID, 2012501, ownerID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 1826991655, 492, 6781, 725)
end

--注册
aux.RegisterQuestEvent(13012, 0, "x13012_OnAccept")

