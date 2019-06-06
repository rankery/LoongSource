--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010529_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 50073)

end

aux.RegisterCreatureEvent(1010529, 2, "c1010529_OnEnterCombat")

function c1010529_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--刷新祭魔台
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1531107, x, y, z)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

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
