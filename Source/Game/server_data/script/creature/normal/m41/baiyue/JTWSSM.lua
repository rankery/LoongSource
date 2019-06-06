-- 怪物进入战斗触发示警
function c1008089_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为5个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008089, 2, "c1008089_OnEnterCombat")

--怪物死亡
function c1008089_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")
	--玩家击杀九天无上魔神[1008089]播放广播
	local MsgID = msg.BeginMsgEvent();					--注册广播消息句柄
	local r = math.random(1,100);	--取随机数

	msg.AddMsgEvent(MsgID,102,100024);
	msg.AddMsgEvent(MsgID,2,RoleID);
	if ( r<11 ) then
		msg.DispatchBroadcast(MsgID,-1,-1,-1);
	elseif ( r<61 ) then
		msg.DispatchBroadcast(MsgID,3017299151,-1,-1);--武夷
		msg.DispatchBroadcast(MsgID,3308687566,-1,-1);--盘龙谷
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
	else
		msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
	end
end
aux.RegisterCreatureEvent(1008089, 4, "c1008089_OnDie")

-- 怪物进入战斗喊话
function c1008081_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40049)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008081, 2, "c1008081_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008081 刷出 干戍奇煞狂魔君 1008082 坐标：x 2840 y 26387 z 354
function c1008081_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008082, 2840, 26387, 354)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1770)	--干戍奇煞狂魔君已重新出现于坐标[2840, 354]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008081, 4, "c1008081_OnDie")

-- 怪物进入战斗喊话
function c1008082_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40050)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008082, 2, "c1008082_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008082 刷出 干戍奇煞狂魔君 1008083 坐标：x 2791 y 26387 z 734
function c1008082_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008083, 2791, 26387, 734)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1771)	--干戍奇煞狂魔君已重新出现于坐标[2791, 734]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008082, 4, "c1008082_OnDie")

-- 怪物进入战斗喊话
function c1008083_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40051)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008083, 2, "c1008083_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008083 刷出 干戍奇煞狂魔君 1008084 坐标：x 2379 y 26387 z 737
function c1008083_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008084, 2379, 26387, 737)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1772)	--干戍奇煞狂魔君已重新出现于坐标[2379, 737]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008083, 4, "c1008083_OnDie")

-- 怪物进入战斗喊话
function c1008084_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40052)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008084, 2, "c1008084_OnEnterCombat")


--·击杀 干戍奇煞狂魔君 1008084 刷出 干戍奇煞狂魔君 1008085 坐标：x 2820 y 26387 z 302
function c1008084_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008085, 2820, 26387, 302)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1773)	--干戍奇煞狂魔君已重新出现于坐标[2820, 302]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008084, 4, "c1008084_OnDie")

-- 怪物进入战斗喊话
function c1008085_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40053)
	end
	    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1008085, 2, "c1008085_OnEnterCombat")

--·击杀 干戍奇煞狂魔君 1008085 刷出 九天无上神魔 1008089 坐标：x 2600 y 25797 z 447
function c1008085_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1008089, 2600, 25797, 447)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1774)	--九天无上神魔已出现于坐标[2600, 447]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1008085, 4, "c1008085_OnDie")
