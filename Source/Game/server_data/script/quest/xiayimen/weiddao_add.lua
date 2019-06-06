function x20158_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	--玩家是否已接取卫道任务
	local Acc_Weidao = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20082)

	if Acc_Weidao == false then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 135)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--玩家本环卫道任务是否已经接受过指点
	local cur_exp = role.GetRoleScriptData(RoleID, 1, RoleDataType["WeiDao_Exp"])

	if cur_exp~=0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 136)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1
end

aux.RegisterQuestEvent(20158, 4, "x20158_OnCheckAccept")

function x20159_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	--玩家是否已接取卫道任务
	local Acc_Weidao = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20082)

	if Acc_Weidao == false then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 135)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1
end

aux.RegisterQuestEvent(20159, 4, "x20159_OnCheckAccept")


function x20158_OnComplete(MapID, InstanceID, QuestID, OwnerID, NPCID)

	local rate=math.random(100)
	local cur_exp=0

	if rate<=2 then
		cur_exp=6

		unit.AddBuff(MapID, InstanceID, OwnerID, 2021201, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 257)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 257)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=10 then
		cur_exp=3

		unit.AddBuff(MapID, InstanceID, OwnerID, 2021101, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 258)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 258)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=30 then
		cur_exp=2

		unit.AddBuff(MapID, InstanceID, OwnerID, 2021001, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 259)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 259)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=100 then
		cur_exp=1

		unit.AddBuff(MapID, InstanceID, OwnerID, 2020901, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 260)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 260)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	end

	role.SetRoleScriptData(OwnerID, 1, RoleDataType["WeiDao_Exp"], cur_exp)
end

aux.RegisterQuestEvent(20158, 1, "x20158_OnComplete")

function x20159_OnComplete(MapID, InstanceID, QuestID, OwnerID, NPCID)

	local rate=math.random(100)
	local cur_exp=0

	if rate<=10 then
		cur_exp=6

		unit.AddBuff(MapID, InstanceID, OwnerID, 2021201, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 257)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 257)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=30 then
		cur_exp=3
		unit.AddBuff(MapID, InstanceID, OwnerID, 2021101, OwnerID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 258)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 258)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=60 then
		cur_exp=2

		unit.AddBuff(MapID, InstanceID, OwnerID, 2021001, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 259)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 259)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	elseif rate<=100 then
		cur_exp=1

		unit.AddBuff(MapID, InstanceID, OwnerID, 2020901, OwnerID)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 260)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 26, 260)
		msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	end

	role.SetRoleScriptData(OwnerID, 1, RoleDataType["WeiDao_Exp"], cur_exp)
end

aux.RegisterQuestEvent(20159, 1, "x20159_OnComplete")
