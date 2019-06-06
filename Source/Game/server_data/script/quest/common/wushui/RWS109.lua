--完成RWS109解除BUFF五内翻腾（2018901）

--任务完成时
function q13109_OnComplete(mapID, instanceID, questID, ownerID, enderID)
	unit.CancelBuff(mapID, instanceID, ownerID, 2018901)
end

--注册
aux.RegisterQuestEvent(13109, 1, "q13109_OnComplete")

