--22般若圆通遍十方 201 前往玄天龙城
--需求1：完成任务后传送至玄天龙城[m02: 2299, 5733, 2204]

--任务完成时
function q022_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298127, 2299, 5733, 2204)
end

--注册
aux.RegisterQuestEvent(22, 1, "q022_OnComplete")
aux.RegisterQuestEvent(201, 1, "q022_OnComplete")

