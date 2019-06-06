--瓶中信(二)：13235
--需求1：使用物品青色瓶[2613122]后接到任务瓶中信(二)[13235]
--如果已完成任务 13235 ，则直接给予道具 2613125*1，判断背包是否有1个空格

--物品青色瓶使用效果函数
function i2613122_QuestGiver(MapID, InstanceID, TypeID, TargetID)
	local IsCom = role.IsRoleCompleteQuest(MapID, InstanceID, TargetID, 13235)
		if IsCom == true then
			local bget = role.AddRoleItem(MapID, InstanceID, TargetID, 2613125, 1, -1, 8, 420)
			if bget == 0 then
			role.RemoveFromRole(MapID, InstanceID, TargetID, 2613122, 1, 1000)
			end
		else
			role.AddQuest(TargetID, 13235)
		end
end

--注册
aux.RegisterItemEvent(2613122, 1, "i2613122_QuestGiver")

function I2613122_CanAdd(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	local IsCom = role.IsRoleCompleteQuest(MapID, InstanceID, TargetID, 13235)

	if IsCom == true then
	--判断背包空闲空间是否足够
		local FreeSize = role.GetBagFreeSize(TargetID)
		if(FreeSize < 1) then
			--提示玩家背包空间不足
			bRet = 40
		end
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2613122, 0, "I2613122_CanAdd")
