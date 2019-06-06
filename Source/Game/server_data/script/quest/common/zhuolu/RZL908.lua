--RZL908    3200530 分值是1

--可否使用函数
function i3200530_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	local i = role.GetBagFreeSize(TargetID)
	local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID ~= 3017299663 then
        bRet = 43
	elseif i <= 1 then
	    bRet = 40
	elseif x < 1980 or x > 2020 or z < 2500 or z > 2540 then
        bRet = 43
	end
	return bRet, bIgnore
end

--使用效果
function i3200530_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1420007, 5, -1, 8, 420)
	role.AddRoleItem(MapID, InstanceID, TargetID, 1412001, 1, -1, 8, 420)
    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 325)
		msg.DispatchRoleMsgEvent(TargetID, MsgID)
end

--注册
aux.RegisterItemEvent(3200530, 1, "i3200530_QuestUsable")
aux.RegisterItemEvent(3200530, 0, "i3200530_CanUse")

function RZL908_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
    local FristTime_GiveBag = role.GetRoleScriptData(RoleID, 1, RoleDataType["FristTime_GiveBag"])
	local i = role.GetBagFreeSize(RoleID)
	if i < 1 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 141)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	else
	    if FristTime_GiveBag == 1 or FristTime_GiveBag == 3 or FristTime_GiveBag == 5 or FristTime_GiveBag == 7 or FristTime_GiveBag > 7 then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 26, 326)
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 325)
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)
			role.AddRoleItem(MapID, InstanceID, RoleID, 3200530, 1, -1, 8, 420)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["FristTime_GiveBag"], FristTime_GiveBag+1)
		end
	end
end

aux.RegisterQuestEvent(12908, 0, "RZL908_OnAccept")
