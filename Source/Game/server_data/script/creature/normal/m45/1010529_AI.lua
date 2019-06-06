--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010529_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 50073)

end

aux.RegisterCreatureEvent(1010529, 2, "c1010529_OnEnterCombat")

function c1010529_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

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

	--刷新祭魔台
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531107, x, y, z+4)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	map.MapCreateCreature(MapID, InstanceID, 3075221, x, y, z)

	--玩家击杀青狄怨魂[1010529]播放服务器广播
	local MsgID = msg.BeginMsgEvent();					--注册广播消息句柄
	local r = math.random(1,100);	--取随机数

	msg.AddMsgEvent(MsgID,102,100026);
	msg.AddMsgEvent(MsgID,2,RoleID);
	if ( r<11 ) then
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ( r<61 ) then
		msg.DispatchBroadcast(MsgID,3017299407,-1,-1);
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
	else
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
	end

end
aux.RegisterCreatureEvent(1010529, 4, "c1010529_OnDie")
