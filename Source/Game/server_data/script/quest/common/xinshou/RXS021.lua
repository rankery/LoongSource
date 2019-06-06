

--任务完成时
function q021_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	role.OpenFramePage(ownerID, 8)

end

--注册
aux.RegisterQuestEvent(21, 1, "q021_OnComplete")

