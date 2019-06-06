--完成RWY107给予BUFF百越王的祝福（2019201）

--任务完成时
function q14107_OnComplete(MapID, InstanceID, questID, ownerID, enderID)
	unit.AddBuff(MapID, InstanceID, ownerID, 2019201, ownerID)
end

--注册
aux.RegisterQuestEvent(14107, 1, "q14107_OnComplete")

