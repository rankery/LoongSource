--踏足龙城少年狂：24
--需求1：完成任务后传送至玄天龙城[m02: 2256, 5265, 1869]

--任务完成时
function x024_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3017298127, 2256, 5265, 1869)
end

--注册
aux.RegisterQuestEvent(24, 1, "x024_OnComplete")

