--怪物喊话脚本

-- 怪物进入战斗喊话/示警脚本
function c1005433_OnEnterCombat(MapID, InstanceID, CreatureID)

             cre.MonsterSay(MapID, InstanceID, CreatureID, 20067)
			     --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1005433, 2, "c1005433_OnEnterCombat")


--怪物死亡喊话
function c1005433_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    cre.MonsterSay(MapID, InstanceID, TargetID, 20069)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

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

end

aux.RegisterCreatureEvent(1005433, 4, "c1005433_OnDie")



