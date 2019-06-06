
--使用一次消耗一个姻缘结

function tongxin_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)
	if role.GetRoleItemNum(OwnerID, 3305003) == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10008)	--使用该技能需要一个姻缘结
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)

	if TeamID==4294967295 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10010)	--组队后才可使用此技能！
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	if MapID==2108960820 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10009)	--该场景中不可使用此技能！
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	return 0

end

function tongxin_Cast(MapID, InstanceID, SkillID, OwnerID)

	role.RemoveFromRole(MapID, InstanceID, OwnerID, 3305003, 1, 420)

	local LearnedSkill = role.IsLearnedSkill(OwnerID, 11601)

	local temp = math.random(4)
	unit.AddBuff(MapID, InstanceID, OwnerID, (11213+temp)*100+LearnedSkill, OwnerID)

	return 0
end


--注册

aux.RegisterSkillEvent(1160101, 1, "tongxin_Cast")
aux.RegisterSkillEvent(1160102, 1, "tongxin_Cast")
aux.RegisterSkillEvent(1160103, 1, "tongxin_Cast")
aux.RegisterSkillEvent(1160104, 1, "tongxin_Cast")
aux.RegisterSkillEvent(1160105, 1, "tongxin_Cast")

aux.RegisterSkillEvent(1160101, 0, "tongxin_CanCast")
aux.RegisterSkillEvent(1160102, 0, "tongxin_CanCast")
aux.RegisterSkillEvent(1160103, 0, "tongxin_CanCast")
aux.RegisterSkillEvent(1160104, 0, "tongxin_CanCast")
aux.RegisterSkillEvent(1160105, 0, "tongxin_CanCast")

function tongxin_2_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)

	if TeamID==4294967295 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10010)	--组队后才可使用此技能！
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	return 0

end

function tongxin_2_Cast(MapID, InstanceID, SkillID, OwnerID)

	local Team_Num = 0
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)

	if TeamID~=4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 and Role[i]~= OwnerID then

				if role.GetFriendShipValue(OwnerID, Role[i])~=-1 and role.GetFriendShipValue(Role[i], OwnerID)~=-1 then
					Team_Num = Team_Num+1
				end
			end
		end
	end

	if Team_Num==0 then

		unit.AddBuff(MapID, InstanceID, OwnerID, 1121801, OwnerID)

	else

		local LearnedSkill = role.IsLearnedSkill(OwnerID, 11601)
		unit.AddBuff(MapID, InstanceID, OwnerID, 1121801+(LearnedSkill-1)*5+Team_Num, OwnerID)

	end

	return 0
end

function tongxin_4_Cast(MapID, InstanceID, SkillID, OwnerID)

	local Team_Num = 0
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)
	local GuildID = guild.GetRoleGuildID(OwnerID)

	if TeamID~=4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 and Role[i]~= OwnerID then

				local GuildID2=guild.GetRoleGuildID(Role[i])

				if GuildID2~=4294967295 and GuildID2==GuildID then
					Team_Num = Team_Num+1
				end
			end
		end
	end

	if Team_Num==0 then

		unit.AddBuff(MapID, InstanceID, OwnerID, 1121901, OwnerID)

	else

		local LearnedSkill = role.IsLearnedSkill(OwnerID, 11601)
		unit.AddBuff(MapID, InstanceID, OwnerID, 1121901+(LearnedSkill-1)*5+Team_Num, OwnerID)

	end

	return 0
end


--注册

aux.RegisterSkillEvent(1160301, 1, "tongxin_2_Cast")
aux.RegisterSkillEvent(1160302, 1, "tongxin_2_Cast")
aux.RegisterSkillEvent(1160303, 1, "tongxin_2_Cast")
aux.RegisterSkillEvent(1160304, 1, "tongxin_2_Cast")
aux.RegisterSkillEvent(1160305, 1, "tongxin_2_Cast")
aux.RegisterSkillEvent(1160401, 1, "tongxin_4_Cast")
aux.RegisterSkillEvent(1160402, 1, "tongxin_4_Cast")
aux.RegisterSkillEvent(1160403, 1, "tongxin_4_Cast")
aux.RegisterSkillEvent(1160404, 1, "tongxin_4_Cast")
aux.RegisterSkillEvent(1160405, 1, "tongxin_4_Cast")

aux.RegisterSkillEvent(1160301, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160302, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160303, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160304, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160305, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160401, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160402, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160403, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160404, 0, "tongxin_2_CanCast")
aux.RegisterSkillEvent(1160405, 0, "tongxin_2_CanCast")

function tongxin_3_Cast(MapID, InstanceID, SkillID, OwnerID, TargetID)

	local restore_val = {}

	restore_val[1]=25
	restore_val[2]=30
	restore_val[3]=35
	restore_val[4]=40
	restore_val[5]=50

	local LearnedSkill = role.IsLearnedSkill(OwnerID, 11601)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)

	if TeamID~=4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 and Role[i]~= OwnerID then

				local temp = role.AreThereSocialRelations(2, Role[i], OwnerID)

				if temp==true then

					local MapID2, InstanceID2 = role.GetRoleMapID(Role[i])

					role.ModRoleAttValue(MapID2, InstanceID2, Role[i], 81, -restore_val[LearnedSkill])
					role.SetRoleScriptData(OwnerID, 1, RoleDataType["Tongxin_last"], tonumber(os.date("%j")))

					return 0
				end
			end
		end

	end
end


--注册

aux.RegisterSkillEvent(1160501, 1, "tongxin_3_Cast")
aux.RegisterSkillEvent(1160502, 1, "tongxin_3_Cast")
aux.RegisterSkillEvent(1160503, 1, "tongxin_3_Cast")
aux.RegisterSkillEvent(1160504, 1, "tongxin_3_Cast")
aux.RegisterSkillEvent(1160505, 1, "tongxin_3_Cast")

function tongxin_3_CanCast(MapID, InstanceID, SkillID, OwnerID, Dummy)

	local UpdateTime = role.GetRoleScriptData(OwnerID, 1, RoleDataType["Tongxin_last"])

	local CurTime = tonumber(os.date("%j"))

	if CurTime==UpdateTime then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10012)	--每天只可使用一次
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, OwnerID)

	if TeamID==4294967295 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10011)	--组队后才可使用此技能！
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999

	else

		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 and Role[i]~= OwnerID then

				local temp = role.AreThereSocialRelations(2, Role[i], OwnerID)

				if temp==true then
					return 0
				end
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 10011)	--组队后才可使用此技能！
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999

	end

	return 0

end

aux.RegisterSkillEvent(1160501, 0, "tongxin_3_CanCast")
aux.RegisterSkillEvent(1160502, 0, "tongxin_3_CanCast")
aux.RegisterSkillEvent(1160503, 0, "tongxin_3_CanCast")
aux.RegisterSkillEvent(1160504, 0, "tongxin_3_CanCast")
aux.RegisterSkillEvent(1160505, 0, "tongxin_3_CanCast")
