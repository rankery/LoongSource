--紀妍美眉的祝福 




--任务提交时接口
function x23001_OnComplete(mapID, instanceID, questID, ownerID, accepterID)
    --给予祝福BUFF
	unit.AddBuff(mapID, instanceID, ownerID, 2025001, ownerID)
end

--注册
aux.RegisterQuestEvent(23001, 1, "x23001_OnComplete")