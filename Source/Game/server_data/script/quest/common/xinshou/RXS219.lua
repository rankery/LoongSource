
--任务完成时
function x219_OnComplete(MapID, InstanceID, QuestID, OwnerID, EnderID)
	role.OpenTrain(OwnerID, 81)
	role.OpenFramePage(OwnerID, 1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 3102)
	msg.DispatchRoleMsgEvent(OwnerID, MsgID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 3102)
	msg.DispatchRoleMsgEvent(OwnerID, MsgID)
end

--注册
aux.RegisterQuestEvent(219, 1, "x219_OnComplete")

function x219_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
    local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"]) + 100
	if k < 200 then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], k)
	end
end

aux.RegisterQuestEvent(219, 0, "x219_OnAccept")
