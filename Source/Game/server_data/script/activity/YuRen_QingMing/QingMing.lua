
QingMingCreature = {}
function QingMing_OnStart(actID)
	QingMingCreature[1] = map.MapCreateColCreature(3017299663, -1, 4900447, 2103, 18345, 2670, 1)
end

function QingMing_OnEnd(actID)
	map.MapDeleteCreature(3017299663, -1, QingMingCreature[1])
end


function QingMing_OnTimerMin(actID)

	local curhour = tonumber(os.date("%H"))
	local curmonth = tonumber(os.date("%m"))
	local curmin = tonumber(os.date("%M"))
	local CanSummon = 0
	local QingMing_Time = {}
	QingMing_Time[1] = {hour=20,mine=30}
	QingMing_Time[2] = {hour=20,mine=45}
	QingMing_Time[3] = {hour=21,mine=0}
	QingMing_Time[4] = {hour=21,mine=15}
	QingMing_Time[5] = {hour=21,mine=30}

	if (curhour == 20 or curhour == 21) then
		for k = 1,5 do
			if QingMing_Time[k].hour == curhour and QingMing_Time[k].mine - 1 == curmin then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 2650)--	1分钟后将在凤翔，涿鹿，武夷刷新无良劫匪，请按扫墓包裹上的坐标前往！每15分钟刷一次直到9:30
				msg.DispatchWorldMsgEvent(MsgID)
			elseif QingMing_Time[k].hour == curhour and QingMing_Time[k].mine == curmin then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 2651)---无良劫匪已于凤翔[2462,1488]附近刷新，每15分钟刷一次直到9:30
				msg.DispatchMapMsgEvent(MsgID, 3017298383, -1)

				local MsgID1 = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID1, 100, 2652)---无良劫匪已于涿鹿[1790,1886]附近刷新，每15分钟刷一次直到9:30
				msg.DispatchMapMsgEvent(MsgID1, 3017299663, -1)

				local MsgID2 = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID2, 100, 2653)---无良劫匪已于武夷[1809,1819]附近刷新，每15分钟刷一次直到9:30
				msg.DispatchMapMsgEvent(MsgID2, 3017299151, -1)
				CanSummon = 1
			end
		end
	end

	if CanSummon == 1 then
		local QingMing_JieFei= {}
		QingMing_JieFei[1] = {mapid=3017298383,monid=1534131,x=2411,y=12694,z=1544}
		QingMing_JieFei[2] = {mapid=3017298383,monid=1534131,x=2411,y=12896,z=1513}
		QingMing_JieFei[3] = {mapid=3017298383,monid=1534131,x=2443,y=12834,z=1537}
		QingMing_JieFei[4] = {mapid=3017298383,monid=1534131,x=2425,y=12484,z=1569}
		QingMing_JieFei[5] = {mapid=3017298383,monid=1534131,x=2390,y=12604,z=1541}
		QingMing_JieFei[6] = {mapid=3017298383,monid=1534131,x=2462,y=12604,z=1488}
		QingMing_JieFei[7] = {mapid=3017298383,monid=1534131,x=2365,y=12074,z=1561}
		QingMing_JieFei[8] = {mapid=3017298383,monid=1534131,x=2382,y=11896,z=1605}

		QingMing_JieFei[9] = {mapid=3017299663,monid=1534132,x=1790,y=12400,z=1886}
		QingMing_JieFei[10] = {mapid=3017299663,monid=1534132,x=1828,y=12312,z=1939}
		QingMing_JieFei[11] = {mapid=3017299663,monid=1534132,x=1805,y=12291,z=1897}
		QingMing_JieFei[12] = {mapid=3017299663,monid=1534132,x=1800,y=12397,z=1926}
		QingMing_JieFei[13] = {mapid=3017299663,monid=1534132,x=1735,y=12349,z=1879}
		QingMing_JieFei[14] = {mapid=3017299663,monid=1534132,x=1728,y=12442,z=1960}
		QingMing_JieFei[15] = {mapid=3017299663,monid=1534132,x=1732,y=12495,z=2000}
		QingMing_JieFei[16] = {mapid=3017299663,monid=1534132,x=1769,y=12512,z=2003}

		QingMing_JieFei[17] = {mapid=3017299151,monid=1534133,x=1809,y=5738,z=1819}
		QingMing_JieFei[18] = {mapid=3017299151,monid=1534133,x=1853,y=5990,z=1681}
		QingMing_JieFei[19] = {mapid=3017299151,monid=1534133,x=1779,y=5925,z=1714}
		QingMing_JieFei[20] = {mapid=3017299151,monid=1534133,x=1828,y=5945,z=1728}
		QingMing_JieFei[21] = {mapid=3017299151,monid=1534133,x=1840,y=5975,z=1762}
		QingMing_JieFei[22] = {mapid=3017299151,monid=1534133,x=1885,y=5983,z=1793}
		QingMing_JieFei[23] = {mapid=3017299151,monid=1534133,x=1925,y=6000,z=1833}
		QingMing_JieFei[24] = {mapid=3017299151,monid=1534133,x=1879,y=6000,z=1742}

		for k = 1,24 do
			map.MapCreateColCreature(QingMing_JieFei[k].mapid, -1, QingMing_JieFei[k].monid, QingMing_JieFei[k].x, QingMing_JieFei[k].y, QingMing_JieFei[k].z, 1)
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(80, 4, "QingMing_OnTimerMin")
aux.RegisterActEvent(80, 2, "QingMing_OnStart")-----活动ID
aux.RegisterActEvent(80, 3, "QingMing_OnEnd")-----活动ID


--完成任务
function x20199_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	local exp_award = {}
	exp_award[25]=2025
	exp_award[30]=2725
	exp_award[35]=3350
	exp_award[40]=4600
	exp_award[45]=5650
	exp_award[50]=7376
	exp_award[55]=9216
	exp_award[60]=9216
	exp_award[65]=9216
	exp_award[70]=11156
	exp_award[75]=11156
	exp_award[80]=11156
	exp_award[85]=13196
	exp_award[90]=13196
	exp_award[95]=17576
	exp_award[100]=17576
	exp_award[105]=25576
	exp_award[110]=25576
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	rolelevel = math.floor(rolelevel/5)*5
	role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[rolelevel])
end

aux.RegisterQuestEvent(20199, 1, "x20199_OnComplete")


function x20200_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	local a = role.GetRoleAttValue(MapID, InstanceID, RoleID, 74)
	if a + 15 <= 0 then
		role.ModRoleAttValue(MapID, InstanceID, RoleID, 74, 15)
	elseif a < 0 then
		role.ModRoleAttValue(MapID, InstanceID, RoleID, 74, -a)
	end
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"])
	role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
end

aux.RegisterQuestEvent(20200, 1, "x20200_OnComplete")


function x20200_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断本日任务接取次数是否已满
	--得到当前任务总任务环数
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"])
	local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"])


	--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
	local CurTime = tonumber(os.date("%j"))
	if CurTime~=UpdateTime then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], 0)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"], CurTime)
		return 1
	end
	if TotalNum >= 3 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 147)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(20200, 4, "x20200_OnCheckAccept")


--扫墓包裹
function I3303701_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	if MapID ==3017298383 then
		map.MapCreateColCreature(MapID, -1, 1534131, x, y, z, 1)
	elseif MapID ==3017299663 then
		map.MapCreateColCreature(MapID, -1, 1534132, x, y, z, 1)
	elseif MapID ==3017299151 then
		map.MapCreateColCreature(MapID, -1, 1534133, x, y, z, 1)
	end
end

function I3303701_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)

	if MapID == 3017298383 and math.abs(x-2462)<100 and math.abs(z-1488)<100 then
		return 0,false
	elseif MapID == 3017299663 and math.abs(x-1790)<100 and math.abs(z-1886)<100 then
		return 0,false
	elseif MapID == 3017299151 and math.abs(x-1809)<100 and math.abs(z-1819)<100 then
		return 0,false
	else
		return 43, false
	end
end

aux.RegisterItemEvent(3303701, 1, "I3303701_OnUse")
aux.RegisterItemEvent(3303701, 0, "I3303701_CanUse")
