

Tongmingguiyu_wuzixiang = 0
-- 怪物进入战斗触发示警
function c1006722_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为3个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1006722, 2, "c1006722_OnEnterCombat")

--需求4  击杀 1006722 后，在坐标 x=780 y=3923 z=757 刷出 通冥灵石 3040534

function c1006722_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 3040534, 780, 3923, 757)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
			if Target_MapID == MapID then
			    role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 3305003, 2, -1, 8, 420)
			    if act.GetActIsStart(151) then
				    HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
				end
			end
	    end
	end

	if Guoqingjie == 1 then
		local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
		Tongmingguiyu_wuzixiang = map.MapCreateColCreature(MapID, InstanceID, 3610223, x+1, y, z+1, 1)  -- 刷新物资箱
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 4021)  --地图公告：“炼狱修罗掠夺的物资箱已经刷新在他死亡地点附近”
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end


	--刷新祭魔台
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531102, x, y, z+4)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")

	map.MapCreateCreature(MapID, InstanceID, 3075221, x, y, z)
end
--注册
aux.RegisterCreatureEvent(1006722,4,"c1006722_OnDie")
