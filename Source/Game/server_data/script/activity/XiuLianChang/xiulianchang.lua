--修炼场活动
MFXiuLianChang_TimerMin = 0

function XiuLianChang_OnStart(actID)
    MFXiuLianChang_TimerMin = 0
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 102, 100030)--每日修炼场免费开放时间已到，大家可在所有修炼场内免费半个小时
	msg.DispatchBroadcast(MsgID,-1,-1,-1)

end

function XiuLianChang_OnTimerMin(actID)

	MFXiuLianChang_TimerMin = MFXiuLianChang_TimerMin + 1

	if MFXiuLianChang_TimerMin == 29 then
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 102, 100031)--本时段的免费修炼场活动已结束，请期待下一时段的开放
	    msg.DispatchBroadcast(MsgID,-1,-1,-1)
    end

end

function XiuLianChang_OnEnd(actID)

	MFXiuLianChang_TimerMin = 0

	local Trans_OnEnd = {}
	Trans_OnEnd[1] = {MapID=2291722097,ToMapID=3017298383,ToX=2485,ToY=13210,ToZ=1532,Buff=2017701}
	Trans_OnEnd[2] = {MapID=2291721329,ToMapID=3017299663,ToX=1602,ToY=15687,ToZ=1507,Buff=2017801}
	Trans_OnEnd[3] = {MapID=2291721585,ToMapID=3017299919,ToX=2512,ToY=22813,ToZ=529,Buff=2017901}
	Trans_OnEnd[4] = {MapID=2291720817,ToMapID=3017299151,ToX=593,ToY=3500,ToZ=1774,Buff=2018001}
	Trans_OnEnd[5] = {MapID=2291721073,ToMapID=3017298127,ToX=2275,ToY=6646,ToZ=2262,Buff=2021301}--m105,对应龙城
	Trans_OnEnd[6] = {MapID=2291720305,ToMapID=1146339967,ToX=553,ToY=12988,ToZ=1970,Buff=2021401}
	Trans_OnEnd[7] = {MapID=2291720561,ToMapID=3017299407,ToX=1759,ToY=302,ToZ=2693,Buff=2021501}


	for i = 1,7 do
		local RoleTbl = map.GetMapAllRoleID(Trans_OnEnd[i].MapID, -1)
		if RoleTbl ~= nil then
			for k, v in pairs(RoleTbl) do
				local bhave = role.IsRoleHaveBuff(Trans_OnEnd[i].MapID, -1, v, 20202)
				-- 随机至各进入npc
				if bhave == true then
				role.RoleGotoNewMap(Trans_OnEnd[i].MapID, -1, v, Trans_OnEnd[i].ToMapID, Trans_OnEnd[i].ToX, Trans_OnEnd[i].ToY, Trans_OnEnd[i].ToZ)
				end
			end
		end
	end
end


--完成任务的脚本在quest/xiulianchang/xiulianchang.lua


--[[
function XiuLianChang_OnTimer(actID)
	local curhour = tonumber(os.date("%H"))
	local curmin = tonumber(os.date("%M"))
	local cansummon = 0
	local Trans_OnTimer = {}
	Trans_OnTimer[1] = {MapID=2291722097,X=2485,Y=13210,Z=1532}
	Trans_OnTimer[2] = {MapID=2291721329,X=1602,Y=15687,Z=1507}
	Trans_OnTimer[3] = {MapID=2291721585,X=2512,Y=22813,Z=529}
	Trans_OnTimer[4] = {MapID=2291720817,X=593,Y=3500,Z=1774}
	Trans_OnTimer[5] = {MapID=2291721073,X=2275,Y=6646,Z=2262}--m105,对应龙城
	Trans_OnTimer[6] = {MapID=2291720305,X=553,Y=12988,Z=1970}
	Trans_OnTimer[7] = {MapID=2291720561,X=1759,Y=302,Z=2693}
	if (curhour == 0 and curmin == 45) or (curhour == 12 and curmin == 45) or (curhour == 21 and curmin == 45) then
		cansummon = 1
	end
	if cansummon == 1 then
		for k = 1,7 then
		map.MapCreateColCreature(Trans_OnTimer[k].MapID, -1, xxxxxxx, X, Y, Z, 1)----------------刷怪
		end
	end

end
]]
aux.RegisterActEvent(74, 2, "XiuLianChang_OnStart")--修炼场ID
aux.RegisterActEvent(74, 3, "XiuLianChang_OnEnd")--修炼场ID
aux.RegisterActEvent(74, 4, "XiuLianChang_OnTimerMin")--修炼场ID
aux.RegisterActEvent(87, 2, "XiuLianChang_OnStart")--修炼场ID
aux.RegisterActEvent(87, 3, "XiuLianChang_OnEnd")--修炼场ID
aux.RegisterActEvent(87, 4, "XiuLianChang_OnTimerMin")--修炼场ID
aux.RegisterActEvent(88, 2, "XiuLianChang_OnStart")--修炼场ID
aux.RegisterActEvent(88, 3, "XiuLianChang_OnEnd")--修炼场ID
aux.RegisterActEvent(88, 4, "XiuLianChang_OnTimerMin")--修炼场ID
--[[
function XiuLian_CanEnterWhenOnline(MapID, RoleID)

	for k in pairs(Trans_OnEnd) do
		if MapID == Trans_OnEnd[k].MapID then
			local bhave = role.IsRoleHaveBuff(MapID, -1, RoleID, 20247)
			if bhave == true then
				unit.CancelBuff(MapID, InstanceID, RoleID, 2024701, RoleID)
				return Trans_OnEnd[k].ToMapID, Trans_OnEnd[k].ToX, Trans_OnEnd[k].ToY, Trans_OnEnd[k].ToZ
			end
		end
	end

end
aux.RegisterMapEvent("m101", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m102", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m103", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m104", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m105", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m106", 12, "XiuLian_CanEnterWhenOnline")
aux.RegisterMapEvent("m107", 12, "XiuLian_CanEnterWhenOnline")
]]
