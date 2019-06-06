--怪物喊话脚本\示警脚本

-- 怪物进入战斗喊话
function c1004426_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 10087)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1004426, 2, "c1004426_OnEnterCombat")


--怪物死亡喊话
function c1004426_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 10089)
 	Increase_shili(MapID, InstanceID, RoleID, 1, 1, "jinjieshili")

    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
			if Target_MapID == MapID then
			    role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 3305003, 5, -1, 8, 420)
			    if act.GetActIsStart(151) then
				    HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
				end
			end
	    end
	end

	if GM_fubenhuodong_on == 1 then

		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)

		if TeamID~=4294967295 then
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])

					if Target_MapID==MapID then
						local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, Role[i])
						role.AddRoleExp(MapID, InstanceID, Role[i], math.min(LevelUpNeedExp, 100000))
					end
				end
			end
		end
	end

end

aux.RegisterCreatureEvent(1004426, 4, "c1004426_OnDie")


