function JiMoShi_Boss_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local SummonerID = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)

	if TargetTypeID == 1531104 then
		role.AddRoleItem(MapID, InstanceID, SummonerID, 3200562, 1, -1, 8, 420)
	elseif TargetTypeID == 1531105 then
		role.AddRoleItem(MapID, InstanceID, SummonerID, 3200563, 1, -1, 8, 420)
	elseif TargetTypeID == 1531106 then
		role.AddRoleItem(MapID, InstanceID, SummonerID, 3200564, 1, -1, 8, 420)
    elseif TargetTypeID == 1531109 then
		role.AddRoleItem(MapID, InstanceID, SummonerID, 3200636, 1, -1, 8, 420)
	elseif TargetTypeID == 1531110 then
		role.AddRoleItem(MapID, InstanceID, SummonerID, 3200637, 1, -1, 8, 420)
	end

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, SummonerID)

	if TeamID~=4294967295 and TeamID~=nil then

		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then
				local k=math.random(100)
				if k<=5 then
					if TargetTypeID == 1531104 then
						role.AddRoleItem(MapID, InstanceID, Role[i], 3200562, 1, -1, 8, 420)
					elseif TargetTypeID == 1531105 then
						role.AddRoleItem(MapID, InstanceID, Role[i], 3200563, 1, -1, 8, 420)
					elseif TargetTypeID == 1531106 then
						role.AddRoleItem(MapID, InstanceID, Role[i], 3200564, 1, -1, 8, 420)
					elseif TargetTypeID == 1531109 then
						role.AddRoleItem(MapID, InstanceID, Role[i], 3200636, 1, -1, 8, 420)
                    elseif TargetTypeID == 1531110 then
						role.AddRoleItem(MapID, InstanceID, Role[i], 3200637, 1, -1, 8, 420)
					end
				end
			end
		end
	end
end

aux.RegisterCreatureEvent(1531104, 4, "JiMoShi_Boss_OnDie")
aux.RegisterCreatureEvent(1531105, 4, "JiMoShi_Boss_OnDie")
aux.RegisterCreatureEvent(1531106, 4, "JiMoShi_Boss_OnDie")
aux.RegisterCreatureEvent(1531109, 4, "JiMoShi_Boss_OnDie")
aux.RegisterCreatureEvent(1531110, 4, "JiMoShi_Boss_OnDie")

