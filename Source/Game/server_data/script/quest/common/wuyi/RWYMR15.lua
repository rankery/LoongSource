

--任务完成时 [2614046]虚弱的魂魄计算。等于8个给额外金钱，大于等于5个给经验。
--[[function q14318_OnComplete(MapID, InstanceID, QuestID, RoleID, EnderID)

end

--注册
aux.RegisterQuestEvent(14318, 1, "q14318_OnComplete")]]


function q14318_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	local num = role.GetRoleItemNum(RoleID, 2614046)
	if num == 8 then
		role.AddRoleExp(MapID, InstanceID, RoleID, 10000)
		role.AddRoleSilver(MapID, InstanceID, RoleID, 3000, 101)
	elseif num >= 5 then
		role.AddRoleExp(MapID, InstanceID, RoleID, 10000)
	end

	return 1
end

aux.RegisterQuestEvent(14318, 5, "q14318_OnCheckComplete")
