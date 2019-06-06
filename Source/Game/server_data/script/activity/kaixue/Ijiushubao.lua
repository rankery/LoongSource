--书包升级(30天)
--可否使用函数
function i3303202_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    local i = role.GetRoleItemNum(TargetID, 3303201)
	if i < 1 then
           bRet = 32
	end
	--返回
	return bRet, bIgnore
end

--使用效果
function i3303202_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 3303201, 1, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303221, 1, -1, 8, 420)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 2308)
	msg.AddMsgEvent(MsgID, 2, TargetID)
	msg.DispatchWorldMsgEvent(MsgID)
end



--书包升级(90天)
--可否使用函数
function i3303203_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    local i = role.GetRoleItemNum(TargetID, 3303201)
    local k = role.GetRoleItemNum(TargetID, 3303221)
	if i < 1 and k < 1 then
           bRet = 32
	end
	--返回
	return bRet, bIgnore
end

--使用效果
function i3303203_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 3303201, 1, 420)
	role.RemoveFromRole(MapID, InstanceID, TargetID, 3303221, 1, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 3303222, 1, -1, 8, 420)
	--通知全服神器铸成
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 2309)
	msg.AddMsgEvent(MsgID, 2, TargetID)
	msg.DispatchWorldMsgEvent(MsgID)
end

--注册
aux.RegisterItemEvent(3303202, 1, "i3303202_QuestUsable")
aux.RegisterItemEvent(3303202, 0, "i3303202_CanUse")
aux.RegisterItemEvent(3303203, 1, "i3303203_QuestUsable")
aux.RegisterItemEvent(3303203, 0, "i3303203_CanUse")

