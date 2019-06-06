

--使用槟榔随机恢复30-50活力
function i3301006_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    local i = math.random(30,50)
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 25, i)
end

--注册
aux.RegisterItemEvent(3301006, 1, "i3301006_QuestUsable")

--使用威士比随机恢复10-30活力
function i3301007_QuestUsable(MapID, InstanceID, TypeID, TargetID)
    local i = math.random(10,30)
	role.ModRoleAttValue(MapID, InstanceID, TargetID, 25, i)
end

--注册
aux.RegisterItemEvent(3301007, 1, "i3301007_QuestUsable")


