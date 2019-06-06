function m47_boss_OnDie(MapID, InstanceID, CreatureID)

	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 1)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 1, Boss_num)

	if Boss_num==2 then
		map.OpenCloseDoor(MapID, InstanceID, 441, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1821)	--枭吴队长吴裂已经出现于坐标[158, 482]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1011017, 4, "m47_boss_OnDie")
aux.RegisterCreatureEvent(1011018, 4, "m47_boss_OnDie")

function c1011019_OnDie(MapID, InstanceID, CreatureID, TargetTypeID, RoleID)

	map.SetMapScriptData(MapID, InstanceID, 1, 8, 200)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1843)	--枭吴队长吴裂已经出现于坐标[158, 482]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	--如果没有小队
	if 	MapID and MapID == 3084408271 then
		if TeamID == nil or TeamID == 4294967295 then
			local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
			local k = b % 10
			if k == 0 or k == 2 then
				role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], b + 1)
			end
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for k in pairs(Role) do
				if Role[k] and Role[k] ~= 4294967295 then
					local b = role.GetRoleScriptData(Role[k], 1, RoleDataType["FeiSheng_shilian"])
					local kk = b % 10
					if kk == 0 or kk == 2 then
						role.SetRoleScriptData(Role[k], 1, RoleDataType["FeiSheng_shilian"], b + 1)
					end
				end
			end
		end
	end
end

aux.RegisterCreatureEvent(1011019, 4, "c1011019_OnDie")
aux.RegisterCreatureEvent(1011020, 4, "c1011019_OnDie")
aux.RegisterCreatureEvent(1011021, 4, "c1011019_OnDie")
