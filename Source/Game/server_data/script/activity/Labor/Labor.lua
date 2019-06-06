
Labor_Creature = {}
Labor_Creature[1] = { TypeID=4900468,x=2133,y=18345,z=2652,CreID=0}
Labor_Creature[2] = { TypeID=4900469,x=2142,y=18345,z=2652,CreID=0}

--[[
function Labor_OnStart(actID)
	if Labor_trigger == 1 then
		for i = 1,2 do
			Labor_Creature[i].CreID = map.MapCreateColCreature(3017299663, -1, Labor_Creature[i].TypeID, Labor_Creature[i].x, Labor_Creature[i].y, Labor_Creature[i].z,1)
		end
	end
end
]]
function Labor_OnEnd(actID)
	for i = 1,2 do
		map.MapDeleteCreature(3017299663, -1, Labor_Creature[i].CreID)
	end
end

--结束后刷新NPC时间见SystemActivity.lua内
function Labor_OnTimerMin(actID)
	if Labor_trigger == 1 then
		local curhour = tonumber(os.date("%H"))
		if (curhour >= 20 and curhour <= 21 and Labor_Creature[1].CreID == 0) then-- 晚20：-22：00
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 102, 100106)--劳动节合成商人已出现，请到黄帝城[2103,2670]附近查看。
			msg.DispatchBroadcast(MsgID,-1,-1,-1)
			for k = 1,2 do
				Labor_Creature[k].CreID = map.MapCreateColCreature(3017299663, -1, Labor_Creature[k].TypeID, Labor_Creature[k].x, Labor_Creature[k].y, Labor_Creature[k].z, 1)
			end
		elseif Labor_Creature[1].CreID ~= 0 and (curhour < 20 or curhour > 21) then
			for i = 1,2 do
				map.MapDeleteCreature(3017299663, -1, Labor_Creature[i].CreID)
				Labor_Creature[i].CreID = 0
			end
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(134, 4, "Labor_OnTimerMin")--活动ID
aux.RegisterActEvent(134, 3, "Labor_OnEnd")--活动ID
