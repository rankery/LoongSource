--怪物喊话脚本/示警脚本

-- 怪物进入战斗喊话
function c1004530_OnEnterCombat(MapID, InstanceID, CreatureID)

        cre.MonsterSay(MapID, InstanceID, CreatureID, 10067)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end

aux.RegisterCreatureEvent(1004530, 2, "c1004530_OnEnterCombat")


--怪物死亡喊话
function c1004530_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    cre.MonsterSay(MapID, InstanceID, TargetID, 10069)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,10)+10
	map.SetMapScriptData(MapID,InstanceID,1,10,Boss_num)

	if Boss_num==1111 then
		local HuanJiao_ID=map.GetMapScriptData(MapID,InstanceID,1,11)
		unit.CancelBuff(MapID, InstanceID, HuanJiao_ID, 9422601)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1737)	--妖邪祭祀幻角的无敌状态已解除！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

	if GM_fubenhuodong2_on == 1 then

		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)

		if TeamID~=4294967295 then
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])

					if Target_MapID==MapID then
						local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, Role[i])
						role.AddRoleExp(MapID, InstanceID, Role[i], math.min(LevelUpNeedExp, 200000))
					end
				end
			end
		end
	end

end

aux.RegisterCreatureEvent(1004530, 4, "c1004530_OnDie")


