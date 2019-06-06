--竞技场活动
Arenaact_min = 0
Arenaact2_min = 0
Arenaact_isopen = 0
Arenaact_times = {}
guangbo_Arena = 0
function Arenaact_OnStart(actID)
    Arenaact_isopen = 1
    Arenaact_min = 0
    Arenaact_times = {}
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100094)--竞技场活动期间玩家可以获得额外的仙界渡劫令奖励，同时只要完成10场竞技场比赛就能获得大量的经验！
	msg.DispatchBroadcast(MsgID,-1,-1,-1)

end

function Arenaact_OnTimerMin(actID)

    guangbo_Arena = 0
	Arenaact_min = Arenaact_min + 1
	if Arenaact_min == 5 or Arenaact_min == 10 or Arenaact_min == 15 or Arenaact_min == 20 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100094)--竞技场活动期间玩家可以获得额外的仙界渡劫令奖励，同时只要完成10场竞技场比赛就能获得大量的经验！
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end

end

function Arenaact_OnEnd(actID)
	Arenaact_isopen = 0
	Arenaact_times = {}
end

function Arenaact2_OnStart(actID)
    Arenaact2_min = 0
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100124)--随机的奖励，随机的队友，新的竞技场模式等待你来参与，比赛的获胜者更有机会获得珍贵的时限制装备合成材料奖励！
	msg.DispatchBroadcast(MsgID,-1,-1,-1)

end

function Arenaact2_OnTimerMin(actID)

	Arenaact2_min = Arenaact2_min + 1
	if Arenaact_min == 5 or Arenaact_min == 10 or Arenaact_min == 15 or Arenaact_min == 20 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100124)--随机的奖励，随机的队友，新的竞技场模式等待你来参与，比赛的获胜者更有机会获得珍贵的时限制装备合成材料奖励！
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end

end

--注册该活动的相关事件
aux.RegisterActEvent(118, 4, "Arenaact_OnTimerMin")
aux.RegisterActEvent(118, 2, "Arenaact_OnStart")
aux.RegisterActEvent(118, 3, "Arenaact_OnEnd")
aux.RegisterActEvent(147, 2, "Arenaact2_OnStart")
aux.RegisterActEvent(147, 4, "Arenaact2_OnTimerMin")
