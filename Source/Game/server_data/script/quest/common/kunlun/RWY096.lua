--完成RWY096传送至昆仑密道口1798, 9460, 2894

--任务完成时
function q15096_OnComplete(MapID, InstanceID, questID, ownerID, enderID)
	role.RoleGotoNewMap(MapID, InstanceID, ownerID, 3017299407, 1798, 189, 2894)
end

--注册
aux.RegisterQuestEvent(15096, 1, "q15096_OnComplete")

