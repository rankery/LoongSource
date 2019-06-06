--兑换蛮族军粮任务
function x20004_Oncomplete(MapID, InstanceID, QuestID, OwnerID, NPCID)
	if Act4_Forage < 50 then
		--累加收集的军粮数量
		Act4_Forage = Act4_Forage + 1
		--“当前收集的蛮族军粮为##（当前收集数量）/50，若达成条件全体玩家可获得奖励”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 12)
		msg.AddMsgEvent(MsgID, 12, Act4_Forage)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		if Act4_Forage == 50 then
		    --全服通告：“今日烽火连天活动中，成功从敌方抢夺大量物资，全体成员可获得额外奖励！”
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 100, 13)
		    msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		    --自动给所有在线的玩家添加一个“凯旋”的状态
		    act.AddAllRoleBuff(2011901)
	    end
	end
end

--aux.RegisterQuestEvent(20004, 1, "x20004_Oncomplete")
