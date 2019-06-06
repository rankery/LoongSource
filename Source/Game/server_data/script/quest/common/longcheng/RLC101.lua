--使用华严经 2610181，获得华严经手抄卷 2610184
function I2610181_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	
      role.QuestAddRoleItem(MapID, InstanceID, TargetID, 10101, 2610184, 1, -1, 102)

end


aux.RegisterItemEvent(2610181, 1, "I2610181_QuestUsable")


--任务完成时 保证完成时的道德为0
function q10101_OnComplete(MapID, InstanceID, QuestID, OwnerID, EnderID)
	local i=role.GetRoleAttValue(MapID, InstanceID, OwnerID, 74)
	if i > -20 then
	    role.ModRoleAttValue(MapID, InstanceID, OwnerID, 74, -20-i)
	end
end

--注册
aux.RegisterQuestEvent(10101, 1, "q10101_OnComplete")