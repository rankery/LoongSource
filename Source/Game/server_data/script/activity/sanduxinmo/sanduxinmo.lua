SanDuXinMo_Boss_Num = 0				-- boss被击杀的个数
SanDuXinMo_Boss_ID = {}				-- 存放三魔的ID，一小时后若未处于交战状态则删除boss

-- boss死亡
function x1523001_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	SanDuXinMo_Boss_Num = SanDuXinMo_Boss_Num + 1

	-- 获得心魔位置，死亡后在其附近刷新出一个三魔宝箱
	local x, y, z = unit.GetPosition(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 3020753, x+2, y, z+2)

	-- 如果三魔全部被击杀，全场景玩家得到一个buff
	if SanDuXinMo_Boss_Num >= 3 then
		SanDuXinMo_Boss_AllDie(MapID, InstanceID, 2016401)
	end
	Increase_shili(MapID, InstanceID, KillerID, 1, 20,"jinjieshili_A")

end

-- boss死亡
function x1523002_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	SanDuXinMo_Boss_Num = SanDuXinMo_Boss_Num + 1

	-- 获得心魔位置，死亡后在其附近刷新出一个三魔宝箱
	local x, y, z = unit.GetPosition(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 3020753, x+2, y, z+2)

	-- 如果三魔全部被击杀，全场景玩家得到一个buff
	if SanDuXinMo_Boss_Num >= 3 then
		SanDuXinMo_Boss_AllDie(MapID, InstanceID, 2016401)
	end
	Increase_shili(MapID, InstanceID, KillerID, 1, 20,"jinjieshili_A")


end

-- boss死亡
function x1523003_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	SanDuXinMo_Boss_Num = SanDuXinMo_Boss_Num + 1

	-- 获得心魔位置，死亡后在其附近刷新出一个三魔宝箱
	local x, y, z = unit.GetPosition(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 3020753, x+2, y, z+2)

	-- 如果三魔全部被击杀，全场景玩家得到一个buff
	if SanDuXinMo_Boss_Num >= 3 then
		SanDuXinMo_Boss_AllDie(MapID, InstanceID, 2016401)
	end
	Increase_shili(MapID, InstanceID, KillerID, 1, 20,"jinjieshili_A")

end

-- 三魔全部死亡
function SanDuXinMo_Boss_AllDie(MapID, InstanceID, BuffID)
	-- 置0
	SanDuXinMo_Boss_Num = 0

	-- 广播玩家得到buff
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 232)
	msg.DispatchMapMsgEvent(MsgID, 3017299151, -1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 232)
	msg.DispatchMapMsgEvent(MsgID, 1146339967, -1)

	map.AddMapRoleBuff(MapID, InstanceID, BuffID)

	-- 广播净禅佛界开启
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 233)
	msg.DispatchMapMsgEvent(MsgID, 3017299151, -1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 233)
	msg.DispatchMapMsgEvent(MsgID, 1146339967, -1)

	-- 刷出npc
	map.MapCreateCreature(3017299151, -1, 3020752, 602, 7220, 409)
end

-- 活动开始
function SanDuXinMo_OnStart(actID)
	-- 刷出三毒心魔
	SanDuXinMo_Boss_ID[0] = map.MapCreateCreature(3017299151, -1, 1523001, 2538, 5000, 541)
	SanDuXinMo_Boss_ID[1] = map.MapCreateCreature(3017299151, -1, 1523002, 2646, 8130, 1227)
	SanDuXinMo_Boss_ID[2] = map.MapCreateCreature(3017299151, -1, 1523003, 1927, 3500, 2645)

	SanDuXinMo_Boss_Num = 0

	-- 广播三毒心魔活动开始
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 231)
	msg.DispatchMapMsgEvent(MsgID, 3017299151, -1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 231)
	msg.DispatchMapMsgEvent(MsgID, 1146339967, -1)

end

-- 活动结束
function SanDuXinMo_OnEnd(actID)
    local CanDelete = 1
	for i=0, 2 do
		if unit.IsAttack(3017299151, -1, SanDuXinMo_Boss_ID[i]) == 1 then
			CanDelete = 0
		end
	end

	-- 如果boss不在交战状态中，则删除
	--[[if CanDelete == 1 then
		-- 删除boss
		map.MapDeleteCreature(3017299151, -1, SanDuXinMo_Boss_ID[0])
		map.MapDeleteCreature(3017299151, -1, SanDuXinMo_Boss_ID[1])
		map.MapDeleteCreature(3017299151, -1, SanDuXinMo_Boss_ID[2])
	end]]
end

-- 法相罗汉
function q14043_OnAccept(MapID, InstanceID, QuestID, OwnerID, NPCID)
	role.RoleGotoNewMap(MapID, InstanceID, OwnerID, 1826988583, 224, 4780, 857)
end
aux.RegisterQuestEvent(14043, 0,"q14043_OnAccept")

-- 净禅菩萨
function q14043_OnComplete(MapID, InstanceID, QuestID, OwnerID, NPCID)
	role.RoleGotoNewMap(MapID, InstanceID, OwnerID, 3017299151, 602, 7220, 409)
end
aux.RegisterQuestEvent(14043, 1,"q14043_OnComplete")

-- 净禅使者对话后传送进入副本
function x3020752_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == 4 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 1826988583, 438, 7389, 308)
		--如果玩家在小队中，则向整个队伍发送进入副本的请求
		role.InstanceNotify(RoleID, true)

	elseif TalkIndex == 5 then

	else

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 235)	-- "您是否要进入净禅佛界？"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	end
end


aux.RegisterCreatureEvent(3020752, 7, "x3020752_OnTalk")

aux.RegisterCreatureEvent(1523001, 4, "x1523001_OnDie")
aux.RegisterCreatureEvent(1523002, 4, "x1523002_OnDie")
aux.RegisterCreatureEvent(1523003, 4, "x1523003_OnDie")

aux.RegisterActEvent(44, 2, "SanDuXinMo_OnStart")
aux.RegisterActEvent(45, 2, "SanDuXinMo_OnStart")
aux.RegisterActEvent(46, 2, "SanDuXinMo_OnStart")
aux.RegisterActEvent(47, 2, "SanDuXinMo_OnStart")
aux.RegisterActEvent(44, 3, "SanDuXinMo_OnEnd")
aux.RegisterActEvent(45, 3, "SanDuXinMo_OnEnd")
aux.RegisterActEvent(46, 3, "SanDuXinMo_OnEnd")
aux.RegisterActEvent(47, 3, "SanDuXinMo_OnEnd")
