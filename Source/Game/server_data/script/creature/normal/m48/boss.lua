function m48_boss_OnDie(MapID, InstanceID, CreatureID)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 0)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 1)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 1, Boss_num)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1822)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, Boss_num)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(1011007, 4, "m48_boss_OnDie")
aux.RegisterCreatureEvent(1011008, 4, "m48_boss_OnDie")
aux.RegisterCreatureEvent(1011009, 4, "m48_boss_OnDie")

function m48_Bboss_OnDie(MapID, InstanceID, CreatureID, TargetTypeID, RoleID)
	if MapID and MapID == 3084409551 then
		local x, y, z = unit.GetPosition(MapID, InstanceID, CreatureID)
		map.MapCreateCreature(MapID, InstanceID, 3075221, x, y, z)
		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		--如果没有小队
		if TeamID == nil or TeamID == 4294967295 then
			local b = role.GetRoleScriptData(Role[k], 1, RoleDataType["FeiSheng_shilian"])
			local k = b % 10
			if k == 0 and k == 1 then
				role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], b + 2)
			end
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for k in pairs(Role) do
				if Role[k] and Role[k] ~= 4294967295 then
					--local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[k])
					--if Target_MapID == MapID then
						local b = role.GetRoleScriptData(Role[k], 1, RoleDataType["FeiSheng_shilian"])
						local k = b % 10
						if k == 0 and k == 1 then
							role.SetRoleScriptData(Role[k], 1, RoleDataType["FeiSheng_shilian"], b + 2)
						end
					--end
				end
			end
		end
	end
end

aux.RegisterCreatureEvent(1011010, 4, "m48_Bboss_OnDie")

