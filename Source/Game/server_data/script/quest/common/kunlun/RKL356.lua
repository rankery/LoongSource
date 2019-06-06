--任务接取时
function q15356_OnComplete(mapID, instanceID, questID, ownerID, accepterID)
	role.RoleGotoNewMap(mapID, instanceID, ownerID, 3084408527, 430, 52, 556)
end
aux.RegisterQuestEvent(15356, 1, "q15356_OnComplete")
