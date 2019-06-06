--精力值瞬回20道具（1000801）

--使用效果
function i1000801_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 77, 20)
end

--注册
aux.RegisterItemEvent(1000801, 1, "i1000801_QuestUsable")

--精力值瞬回60道具（1000802）

--使用效果
function i1000802_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 77, 60)
end

--注册
aux.RegisterItemEvent(1000802, 1, "i1000802_QuestUsable")

--精力值瞬回120道具（1000803）
--使用效果
function i1000803_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 77, 120)
end
--注册
aux.RegisterItemEvent(1000803, 1, "i1000803_QuestUsable")


--精力值停耗2小时道具（1000804）
--使用效果
function i1000804_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    unit.AddBuff(MapID, InstanceID, TargetID, 3312401)
	role.RoleFixSpirit(MapID, InstanceID, TargetID, 36000)
end

--注册
aux.RegisterItemEvent(1000804, 1, "i1000804_QuestUsable")


--精力值停耗4小时道具（1000805）
--使用效果
function i1000805_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    unit.AddBuff(MapID, InstanceID, TargetID, 3312402)
	role.RoleFixSpirit(MapID, InstanceID, TargetID, 72000)
end
--注册
aux.RegisterItemEvent(1000805, 1, "i1000805_QuestUsable")


--精力值停耗8小时道具（1000806）
--使用效果
function i1000806_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    unit.AddBuff(MapID, InstanceID, TargetID, 3312403)
	role.RoleFixSpirit(MapID, InstanceID, TargetID, 144000)
end
--注册
aux.RegisterItemEvent(1000806, 1, "i1000806_QuestUsable")

