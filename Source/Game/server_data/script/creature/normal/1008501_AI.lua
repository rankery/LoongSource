-- 怪物进入战斗触发示警
--function c1008501_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    --local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    --g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
--end

--aux.RegisterCreatureEvent(1008501, 2, "c1008501_OnEnterCombat")

--怪物死亡

function c1008501_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")
	--玩家击杀上古魔王[1008501]播放服务器广播
	local MsgID = msg.BeginMsgEvent();					--注册广播消息句柄
	local r = math.random(1,100);	--取随机数

	msg.AddMsgEvent(MsgID,102,100025);
	msg.AddMsgEvent(MsgID,2,RoleID);
	if ( r<11 ) then
		msg.DispatchBroadcast(MsgID,-1,-1,-1);--全服
	elseif ( r<61 ) then
		msg.DispatchBroadcast(MsgID,3017299151,-1,-1);--武夷
		msg.DispatchBroadcast(MsgID,3308687566,-1,-1);--盘龙谷
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);--副本
	else
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);--副本
	end	
end
aux.RegisterCreatureEvent(1008501, 4, "c1008501_OnDie")


