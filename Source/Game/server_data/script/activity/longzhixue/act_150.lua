longzhixue_monsterid = 0
longzhixue_timermin = 0

function longzhixue_OnStart(actID)

	local bossindex = act.GetActScriptData(99, 1, 17)%4+1

	act.SetActScriptData(99, 1, 17, bossindex)
	act.SaveActScriptData(99)

	longzhixue_timermin = 0

	if bossindex==1 then

		longzhixue_monsterid = map.MapCreateCreature(3017298383, -1, 4901001, 2054, 10964, 2544)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3610)		--神龙已在凤翔出现
		msg.DispatchWorldMsgEvent(MsgID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3626)		--神龙已在凤翔出现
		msg.DispatchWorldMsgEvent(MsgID)

	elseif bossindex==2 then

		longzhixue_monsterid = map.MapCreateCreature(3017299663, -1, 4901002, 1166, 13219, 1873)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3611)		--神龙已在涿鹿出现
		msg.DispatchWorldMsgEvent(MsgID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3627)		--神龙已在涿鹿出现
		msg.DispatchWorldMsgEvent(MsgID)

	elseif bossindex==3 then

		longzhixue_monsterid = map.MapCreateCreature(3017299919, -1, 4901003, 1803, 24427, 2568)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3612)		--神龙已在巫水出现
		msg.DispatchWorldMsgEvent(MsgID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3628)		--神龙已在巫水出现
		msg.DispatchWorldMsgEvent(MsgID)

	elseif bossindex==4 then

		longzhixue_monsterid = map.MapCreateCreature(3017299151, -1, 4901004, 1915, 3100, 761)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3613)		--神龙已在武夷出现
		msg.DispatchWorldMsgEvent(MsgID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 3629)		--神龙已在武夷出现
		msg.DispatchWorldMsgEvent(MsgID)

	end

end

aux.RegisterActEvent(150, 2, "longzhixue_OnStart")

function longzhixue_OnTimerMin(actID)

	longzhixue_timermin = longzhixue_timermin+1

	if longzhixue_timermin==1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3619)		--击杀神龙能获得至尊奖励
		msg.DispatchWorldMsgEvent(MsgID)
	end

end

aux.RegisterActEvent(150, 4, "longzhixue_OnTimerMin")

function longzhixue_OnEnd(actID)

	local bossindex = act.GetActScriptData(99, 1, 17)

	if bossindex==1 then
		map.MapDeleteCreature(3017298383, -1, longzhixue_monsterid)
	elseif bossindex==2 then
		map.MapDeleteCreature(3017299663, -1, longzhixue_monsterid)
	elseif bossindex==3 then
		map.MapDeleteCreature(3017299919, -1, longzhixue_monsterid)
	elseif bossindex==4 then
		map.MapDeleteCreature(3017299151, -1, longzhixue_monsterid)
	end

end

aux.RegisterActEvent(150, 3, "longzhixue_OnEnd")
