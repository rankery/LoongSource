--好意相送：10068
--需求：完成任务时传送至玄天龙城[m02 3281,4653,1656]

--任务提交时函数
function q10068_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298127, 3281, 4653, 1656)
end

--注册
aux.RegisterQuestEvent(10068, 1, "q10068_OnComplete")

