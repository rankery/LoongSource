--合成60级神器 伏羲翠翼琴 8200216

--可否使用函数
function i3200544_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
    local i = role.GetRoleItemNum(TargetID, 3200517)
    local k = role.GetRoleItemNum(TargetID, 3200562)
	if i ~= 1 then
           bRet = 32
	end
       if k <=29 then
	    bRet = 32
	end
	--返回
	return bRet, bIgnore
end

--使用效果
function i3200544_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       role.RemoveFromRole(MapID, InstanceID, TargetID, 3200517, 1, 420)
       role.RemoveFromRole(MapID, InstanceID, TargetID, 3200562, 30, 420)
       role.AddRoleItem(MapID, InstanceID, TargetID, 8200216, 1, -1, 8, 420)
       --通知全服神器铸成
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 327)
        msg.AddMsgEvent(MsgID, 2, TargetID)
	msg.AddMsgEvent(MsgID, 4, 8200216)
	msg.DispatchWorldMsgEvent(MsgID)
end

--注册
aux.RegisterItemEvent(3200544, 1, "i3200544_QuestUsable")
aux.RegisterItemEvent(3200544, 0, "i3200544_CanUse")

