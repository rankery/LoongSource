--任务燃烧军粮
function x20002_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	--随机一个刷怪点
	local randPoint = math.random(101, 130)
	local x = map_list[3017298127].map[randPoint].x
	local y = map_list[3017298127].map[randPoint].y
	local z = map_list[3017298127].map[randPoint].z

	--生成一个调查地物“蛮族军粮库”
	map.MapCreateCreature(3017298127, -1, 3010225, x, y, z)

	--附近会刷新出2-4个“蛮族护粮使”
	local Num = math.random(2, 4)
	for k=1, Num do
		map.MapCreateCreature(3017298127, -1, 1003162, x+1, y, z+1)
	end

	--全场景提示：“发现一座敌军军粮库! 位于####，####(坐标)!”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 0)
	msg.AddMsgEvent(MsgID, 1, 10)
	msg.AddMsgEvent(MsgID, 9, x)
	msg.AddMsgEvent(MsgID, 9, z)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
end


function x20002_OnInvest(MapID, InstanceID, QuestID, OwnerID, NPCID)
	--全玄天龙城场景玩家通告：“###（玩家名称）成功烧毁一座蛮族军粮库!”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 11)
	msg.AddMsgEvent(MsgID, 2, OwnerID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	--25%几率获得一个特殊道具“蛮族军粮”
	local Rate = math.random(1, 100)
	if Rate <=25 then
		role.QuestAddRoleItem(MapID, InstanceID, OwnerID, QuestID, 2610161, 1, -1, 101)
	end

	--完成任务
	role.RoleCompleteQuest(OwnerID, QuestID, -1)
		--增加进阶实力值
	Increase_shili(MapID, InstanceID, OwnerID, 0, 1, "jinjieshili_A")

end

--aux.RegisterQuestEvent(20002, 0, "x20002_OnAccept")
--aux.RegisterQuestEvent(20002, 9, "x20002_OnInvest")
