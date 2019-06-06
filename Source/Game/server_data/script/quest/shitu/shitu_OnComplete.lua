function shitu_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断玩家是否有师父
	local MasterID = role.GetMasterID(MapID, InstanceID, RoleID)
	if MasterID == 4294967295 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2306)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)

	--判断玩家是否已与师父组队
	if TeamID==4294967295 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2303)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

	local IsMasterInTeam =0

	for i=1, 6 do
	    if Role[i]== MasterID then
	        IsMasterInTeam=1
	    end
	end

	if IsMasterInTeam==0 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2303)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断师父是否在身边
	local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, MasterID)
	local x2, y2, z2 = unit.GetPosition(MapID, InstanceID, RoleID)

	if x1== nil then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2304)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	if x1-x2>30 or x2-x1>30 or z1-z2>30 or z2-z1>30 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2304)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1
end

function shitu_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local AddExp = role.GetRoleLevelUpExp(MapID, InstanceID, RoleID)

	AddExp = AddExp * 600/(rolelevel+25)^2
	role.AddRoleExp(MapID, InstanceID, RoleID, AddExp)

	if QuestID==8013 or QuestID==8014 then
		local MasterID = role.GetMasterID(MapID, InstanceID, RoleID)


		--如果服务器已开启工资功能则奖励一个传道者的证明
		if Salary_NPC_On==1 then
		    role.AddRoleItem(MapID, InstanceID, MasterID, 3303501, 1, 1, 8, 420)
		end

		local stringid = 0

		if JingMai_On == 1 then
			local r = math.random(5)

			if r==1 then
				role.AddRoleItem(MapID, InstanceID, MasterID, 3200701, 1, 1, 8, 420)
				stringid = 2307
			elseif r==2 then
				role.AddRoleItem(MapID, InstanceID, MasterID, 3200701, 1, 1, 8, 420)
				stringid = 2316
				role.AddRoleItem(MapID, InstanceID, MasterID, 1360131, 3, 1, 8, 420)
			elseif r==3 then
				role.AddRoleItem(MapID, InstanceID, MasterID, 3200701, 1, 1, 8, 420)
				stringid = 2316
				role.AddRoleItem(MapID, InstanceID, MasterID, 1360131, 3, 1, 8, 420)
			elseif r==4 then
				role.AddRoleItem(MapID, InstanceID, MasterID, 1360131, 5, 1, 8, 420)
				stringid = 2317
			elseif r==5 then
				role.AddRoleItem(MapID, InstanceID, MasterID, 1360131, 8, 1, 8, 420)
				stringid = 2317
			end

		else
			role.AddRoleItem(MapID, InstanceID, MasterID, 3200701, 1, 1, 8, 420)
			stringid = 2307
		end


		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, stringid)
		msg.AddMsgEvent(MsgID, 2, MasterID)
		msg.DispatchWorldMsgEvent(MsgID)
	end

end

--注册
aux.RegisterQuestEvent(8002, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8003, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8004, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8005, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8006, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8007, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8008, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8009, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8010, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8011, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8012, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8013, 5, "shitu_OnCheckComplete")
aux.RegisterQuestEvent(8014, 5, "shitu_OnCheckComplete")

--注册
aux.RegisterQuestEvent(8002, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8003, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8004, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8005, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8006, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8007, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8008, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8009, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8010, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8011, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8012, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8013, 1, "shitu_OnComplete")
aux.RegisterQuestEvent(8014, 1, "shitu_OnComplete")
