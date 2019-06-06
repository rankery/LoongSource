
Jidou = {Singlewaitinglist = {},Teamwaitinglist = {},NPCID={},RoomIndex=0,CreBuff={},RoomInfo={}}   --Singlewaitinglist单独排队的玩家列表，Teamwaitinglist队伍排队的玩家列表，NPCID活动用到的NPCID，RoomIndex已经开过的房间总量，RoomInfo每个房间的信息,CreBuff是触发器上怪物与buff的对应表
Jidou.CreBuff[4902119] = 1140301
Jidou.CreBuff[4902120] = 1140401
Jidou.CreBuff[4902121] = 1140501
Jidou.CreBuff[4902122] = 1140601

function moshen_OnStart(actID)

	--Jidou.NPCid[1] = map.MapCreateCreature(3017298383, -1, 4900801, 882, 10000, 1994)--需要3个不同的坐标，现在只写了一个，其他两个需要另找
	--Jidou.NPCid[2] = map.MapCreateCreature(3017298383, -1, 4900801, , 10000, )
	--Jidou.NPCid[3] = map.MapCreateCreature(3017298383, -1, 4900801, , 10000, )

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 12928)	--魔神激斗活动已开始
	msg.DispatchWorldMsgEvent(MsgID)

end

aux.RegisterActEvent(161, 2, "moshen_OnStart")
aux.RegisterActEvent(162, 2, "moshen_OnStart")

function moshen_OnEnd(actID)

	--map.MapDeleteCreature(3017298383, -1, Jidou.NPCid[1])
	--map.MapDeleteCreature(3017298383, -1, Jidou.NPCid[2])
	--map.MapDeleteCreature(3017298383, -1, Jidou.NPCid[3])

	if #Jidou.Singlewaitinglist > 0 then
		for i=1, #Jidou.Singlewaitinglist do
            unit.CancelBuff(MapID, InstanceID,Jidou.Singlewaitinglist[i], 1140901)  --取消魔神排队状态
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3725)			--活动时间到，您刚才的报名已取消
			msg.DispatchRoleMsgEvent(Jidou.Singlewaitinglist[i], MsgID)
		end
		Jidou.Singlewaitinglist = {}
	end

	if #Jidou.Teamwaitinglist > 0 then
		for i=1, #Jidou.Teamwaitinglist do
		    unit.CancelBuff(MapID, InstanceID,Jidou.Teamwaitinglist[i], 1140901)  --取消魔神排队状态
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3725)			--活动时间到，您刚才的报名已取消
			msg.DispatchRoleMsgEvent(Jidou.Teamwaitinglist[i], MsgID)
		end
		Jidou.Teamwaitinglist = {}
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 12929)	--本次魔神激斗活动已经结束
	msg.DispatchWorldMsgEvent(MsgID)

end

aux.RegisterActEvent(161, 3, "moshen_OnEnd")
aux.RegisterActEvent(162, 3, "moshen_OnEnd")
