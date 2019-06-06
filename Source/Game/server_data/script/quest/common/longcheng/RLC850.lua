--秘密武器：10850
--需求1：与NPC陶略[3010186]对话后获得圆滑的石子[2610176]

--NPC对话函数
function x10850_OnNPCTalk(MapID, InstanceID, QuestID, RoleID, NpcID, NpctypeID)
	if NpctypeID == 3010186 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2610176, 1, -1, 8, 420)
	end
end

--注册
aux.RegisterQuestEvent(10850, 6, "x10850_OnNPCTalk")


--10850:秘密武器
--如果有结实的树杈[2610174],坚韧皮筋[2610175],圆滑石子[2610176]这三种任务物品，使用圆滑石子[2610176]后，删除以上三个物品，获得 绝世武器[2610177]。
function i2610176_QuestUsable(MapID, InstanceID, TypeID, RoleID)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2610174, 1, 101)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2610175, 1, 101)
	role.RemoveFromRole(MapID, InstanceID, RoleID, 2610176, 1, 101)
	role.AddRoleItem(MapID, InstanceID, RoleID, 2610177, 1, -1, 8, 420)
end


function i2610176_CanUse(MapID, InstanceID, TypeID, RoleID)
	local num1 = role.GetRoleItemNum(RoleID, 2610174)
	local num2 = role.GetRoleItemNum(RoleID, 2610175)


	if num1 < 1 or num2 < 1 then
	return 32,false
	else
	return 0, false
	end
end

--注册

aux.RegisterItemEvent(2610176, 1, "i2610176_QuestUsable")
aux.RegisterItemEvent(2610176, 0, "i2610176_CanUse")
