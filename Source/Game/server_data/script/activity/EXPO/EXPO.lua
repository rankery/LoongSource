
EXPO_Creature = {}
function EXPO_OnStart(actID)
	if EXPO_trigger == 1 then
		EXPO_Creature[1] = map.MapCreateColCreature(3017299663, -1, 4900460, 2103, 18345, 2670,1) --迎宾使者
		EXPO_Creature[2] = map.MapCreateColCreature(3017298127, -1, 4900460, 2222, 6646, 2222, 1) --迎宾使者
		EXPO_Creature[3] = map.MapCreateColCreature(3017299663, -1, 4900461, 2100, 18345, 2670,1) --世博兑换人
		EXPO_Creature[4] = map.MapCreateColCreature(3017298127, -1, 4900461, 2220, 6646, 2220, 1) --世博兑换人
	end
	for k = 5,8 do
		EXPO_Creature[k] = 0
	end
end

function EXPO_OnEnd(actID)
	map.MapDeleteCreature(3017299663, -1, EXPO_Creature[1])
	map.MapDeleteCreature(3017298127, -1, EXPO_Creature[2])
	map.MapDeleteCreature(3017299663, -1, EXPO_Creature[3])
	map.MapDeleteCreature(3017298127, -1, EXPO_Creature[4])
end


function EXPO_OnTimerMin(actID)
	if EXPO_trigger == 1 then
		local curday = tonumber(os.date("%w"))
		local CanSummon = 0
		if (curday == 0 or curday == 6) then--周日或者周六
			local curhour = tonumber(os.date("%H"))
			local curmin = tonumber(os.date("%M"))
			local EXPO_Time = {}
			EXPO_Time[1] = {hour=15,mine=0}
			EXPO_Time[2] = {hour=16,mine=0}
			EXPO_Time[3] = {hour=21,mine=0}
			EXPO_Time[4] = {hour=22,mine=0}
			EXPO_Time[5] = {hour=14,mine=55}
			EXPO_Time[6] = {hour=15,mine=55}
			EXPO_Time[7] = {hour=20,mine=55}
			EXPO_Time[8] = {hour=21,mine=55}
			for k = 1,4 do
				if EXPO_Time[k].hour == curhour and EXPO_Time[k].mine == curmin then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 102, 100032)--"世博金蛋已在龙城[2714,2254]和黄帝城[2161,1910]出现。"
					msg.DispatchBroadcast(MsgID,-1,-1,-1)
					CanSummon = 1
				elseif EXPO_Time[k].hour == curhour and EXPO_Time[k].mine == curmin+1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 102, 100033)--"全球友人已在龙城[2150,2234]和黄帝城[2095,2670]出现。"
					msg.DispatchBroadcast(MsgI,-1,-1,-1)
					CanSummon = 1
				end
			end
			for k = 5,8 do --即将刷怪公告""
				if EXPO_Time[k].hour == curhour and EXPO_Time[k].mine == curmin then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 102, 100034)--"迎世博会,世博金蛋5分钟后即将出现在龙城[2714,2254]和黄帝城[2161,1910]"
					msg.DispatchBroadcast(MsgID,-1,-1,-1)
				elseif EXPO_Time[k].hour == curhour and EXPO_Time[k].mine == curmin+1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 102, 100035)--"迎世博会,全球友人5分钟后即将出现在龙城[2150,2234]和黄帝城[2095,2670]"
					msg.DispatchBroadcast(MsgID,-1,-1,-1)
				end
			end
		end
		if CanSummon == 1 then
			local EXPO_Mon= {}
			EXPO_Mon[1] = {mapid=3017299663,monid=1534140,x=2161,y=13089,z=1910}--黄帝金蛋2161 13089 1910
			EXPO_Mon[2] = {mapid=3017298127,monid=1534140,x=2714,y=6515,z=2254}--龙城金蛋2714 6515 2254
			EXPO_Mon[3] = {mapid=3017299663,monid=4900463,x=2095,y=18345,z=2670}--黄帝友人
			EXPO_Mon[4] = {mapid=3017298127,monid=4900463,x=2150,y=6646,z=2234}--龙城友人
			for k = 1,4 do
				EXPO_Creature[k+4] = map.MapCreateColCreature(EXPO_Mon[k].mapid, -1, EXPO_Mon[k].monid, EXPO_Mon[k].x, EXPO_Mon[k].y, EXPO_Mon[k].z, 1)
			end
		end
	end
end

--注册该活动的相关事件
aux.RegisterActEvent(133, 4, "EXPO_OnTimerMin")	--活动ID
aux.RegisterActEvent(133, 2, "EXPO_OnStart")	--活动ID
aux.RegisterActEvent(133, 3, "EXPO_OnEnd")		--活动ID


--与世博兑换人对话
function ShiBoDuiHuan_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"]) --使用清明节的计数

	local exp_award = {}
	exp_award[20]= 780
	exp_award[25]= 1512
	exp_award[30]= 2253
	exp_award[35]= 4083
	exp_award[40]= 5994
	exp_award[45]= 8454
	exp_award[50]= 11064
	exp_award[55]= 13824
	exp_award[60]= 16734
	exp_award[65]= 19794
	exp_award[70]= 23004
	exp_award[75]= 26364
	exp_award[80]= 29874
	exp_award[85]= 33534
	exp_award[90]= 37344
	exp_award[95]= 41304
	exp_award[100]= 45414
	exp_award[105]= 50414
	exp_award[110]= 58414

	local level = math.floor(rolelevel/5)*5
	local mark = 0 -- 记录是否有足够物品
	local freeb = 0 --记录背包空间是否足够
	if TalkIndex == -1 then
		if rolelevel >= 20 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1501)--"世博兑换人：在《龙》游戏世界里发放的世博字帖都被怪物都抢走了，麻烦勇士帮我们抢回来吧。所有场景的怪物身上都可能会有字帖，一些稀有的字帖听说在一些头目手上，勇士要小心哦。抢回数量越多的字帖，我会给越好的奖励哦。"
			msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1502)	-- "【兑换字帖】"
			msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1503)	-- "【大奖兑换券】"
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 3012)
			msg.AddMsgEvent(MsgID, 9, 20)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	elseif TalkIndex == 4 and rolelevel >= 20 then

	--[[“欢迎您”*10，“世博会”*3，“上海”*1，“EXPO”*1 可换\n
	★世博请柬*1、大奖兑换券*1，经验金钱\n
“欢迎您”*10，“世博会”*3，“上海”*1\n
	★世博请柬*1，经验金钱\n
“欢迎您”*10，“世博会”*3\n
	★经验金钱\n
“欢迎您”*10\n
	★经验金钱
]]
		local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"]) --使用清明节的更新时间
		--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
		local CurTime = tonumber(os.date("%j"))
		if CurTime~=UpdateTime then
			role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], 0)
			role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"], CurTime)
		end
		if TotalNum >= 10 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 147)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1504)--------------------------------------------------------
			msg.AddMsgEvent(MsgID, 21, 6)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1505)	-- "上交“欢迎您”*10，“世博会”*3，“上海”*1，“EXPO”*1 "
			msg.AddMsgEvent(MsgID, 21, 7)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1506)	-- "上交“欢迎您”*10，“世博会”*3，“上海”*1"
			msg.AddMsgEvent(MsgID, 21, 8)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1507)	-- "上交“欢迎您”*10，“世博会”*3"
			msg.AddMsgEvent(MsgID, 21, 9)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 1508)	-- "上交“欢迎您”*10"
			msg.AddMsgEvent(MsgID, 21, 13)	-- 选项一
			msg.AddMsgEvent(MsgID, 1, 59)	-- 取消
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	elseif TalkIndex == 5 and rolelevel >= 20 then
--[[bbbb
您已上交xx个大奖兑换券\n\n
至尊超级大奖：\n 截止5月31日24点电信大区和网通大区收集大奖兑换券最多的角色可以获得超绝神器一把；\n
世博活动特等奖：\n 截止5月31日24点电信大区和网通大区收集大奖兑换券第二名，且数量超过100张的玩家，可以获得上海世博会门票（7日指定票）一张，并由我们承担上海2日住宿费用，商城赠点：100000点；\n
世博活动一等奖：\n 每个服务器收集满75张的玩家，可以获得上海世博会门票（3日指定票）一张，每个服务器限量1张；\n
世博活动二等奖：\n每个服务器收集满50张的玩家，可以获得上海世博会门票（1日指定票）一张，每个服务器限量3张；
]]		local num = role.GetRoleRankData(RoleID,1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1509)--------------------------------------------------------bbbb
		msg.AddMsgEvent(MsgID, 9, num)
		msg.AddMsgEvent(MsgID, 21, 10)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 1510)	-- "【查询本服前三名】 "
		msg.AddMsgEvent(MsgID, 21, 11)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 1511)	-- "【上交大奖兑换券】"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 6 and TotalNum < 10 then
		local x = role.GetRoleItemNum(RoleID, 3303801)
		local y = role.GetRoleItemNum(RoleID, 3303802)
		local z = role.GetRoleItemNum(RoleID, 3303803)
		local m = role.GetRoleItemNum(RoleID, 3303804)
		if x >= 10 and y >= 3 and z >= 1 and m >= 1 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if(FreeSize < 3) then--根据奖励物品而定
				freeb = 1
			else
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303801, 10, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303802, 3, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303803, 1, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303804, 1, 420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303805, 1, -1, 8, 420)--大奖
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303806, 1, -1, 8, 420)--纪念章
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303809, 1, -1, 8, 420)--请柬
				role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[level]*2)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100036)--"【江湖传闻】XXXX完成世博字帖兑换，获得一个世博请柬和大奖兑换券。"
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			end
		else
			mark = 1
		end
	elseif TalkIndex == 7 and TotalNum < 10 then
		local x = role.GetRoleItemNum(RoleID, 3303801)
		local y = role.GetRoleItemNum(RoleID, 3303802)
		local z = role.GetRoleItemNum(RoleID, 3303803)
		if x >= 10 and y >= 3 and z >= 1 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if(FreeSize < 2) then--根据奖励物品而定
				freeb = 1
			else
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303801, 10, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303802, 3, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303803, 1, 420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303806, 1, -1, 8, 420)--纪念章
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303809, 1, -1, 8, 420)--请柬
				role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[level]*1.8)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100037)--"【江湖传闻】XXXX完成世博字帖兑换，获得一个世博请柬。"
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			end
		else
			mark = 1
		end
	elseif TalkIndex == 8 and TotalNum < 10 then
		local x = role.GetRoleItemNum(RoleID, 3303801)
		local y = role.GetRoleItemNum(RoleID, 3303802)
		if x >= 10 and y >= 3 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if(FreeSize < 1) then--根据奖励物品而定
				freeb = 1
			else
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303801, 10, 420)
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303802, 3, 420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303806, 1, -1, 8, 420)--纪念章
				role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[level]*1.5)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
			end
		else
			mark = 1
		end
	elseif TalkIndex == 9 and TotalNum < 10 then
		local x = role.GetRoleItemNum(RoleID, 3303801)
		if x >= 10 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if(FreeSize < 1) then--根据奖励物品而定
				freeb = 1
			else
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303801, 10, 420)
				role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[level])
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
			end
		else
			mark = 1
		end
	elseif TalkIndex == 10 then
	role.GetDisplayRank(RoleID,1,1,5)
	-------------------------------------------------弹出排名接口
	elseif TalkIndex == 11 then
		local a = role.GetRoleItemNum(RoleID, 3303805)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1512)--您行囊内共有a个大奖兑换券，点击确定全部上交。如果想上交固定数量，请将想保留的道具存至银行。
		msg.AddMsgEvent(MsgID, 9, a)
		msg.AddMsgEvent(MsgID, 21, 12)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 1513)	-- "确认全部上交 "
		msg.AddMsgEvent(MsgID, 21, 13)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 59)	-- 取消
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 12 then
		local x = role.GetRoleItemNum(RoleID, 3303805)
		if x == 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1517)--您没有兑换券
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			role.RemoveFromRole(MapID, InstanceID, RoleID, 3303805, x, 420)
			--向数据库表中写入积分变化。
			role.ModRankData(RoleID,1,x)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1518)--您上交了x个大奖兑换券。
			msg.AddMsgEvent(MsgID, 9, x)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end
	if freeb == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2235)--您行囊空间不足，请清理行囊空间后再来兑换！
		msg.AddMsgEvent(MsgID, 21, 13)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 59)	-- 取消
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
	if mark == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1514)--要求物品不足，请确认物品数量或者选择其他兑换项。
		msg.AddMsgEvent(MsgID, 21, 13)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 59)	-- 取消
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end
aux.RegisterCreatureEvent(4900461, 7, "ShiBoDuiHuan_OnTalk")




--鲜花使用
function I3303807_OnUse(MapID, InstanceID, TypeID, RoleID)
	local TargetID = 0
	if MapID == 3017299663 then
		TargetID = EXPO_Creature[7]
	elseif MapID == 3017298127 then
		TargetID = EXPO_Creature[8]
	end

	local Num = cre.GetCreatureScriptData(MapID, InstanceID, TargetID,1,1)
	cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1, Num + 1)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1515)--你成功的献上鲜花。全球友人已接受XX朵鲜花。
	msg.AddMsgEvent(MsgID, 9, Num + 1)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
	if Num + 1 == 1000 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100038)--"XX(场景)的全球友人已收到1000朵鲜花,为在场的玩家带来丰厚好礼。"
		msg.AddMsgEvent(MsgID, 6, MapID)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)

		map.MapDeleteCreature(MapID, -1, TargetID)
		if MapID == 3017299663 then
			EXPO_Creature[7] = 0
			map.MapCreateColCreature(3017299663, -1, 4900464, 2103, 18345, 2665,1)
		elseif MapID == 3017298127 then
			EXPO_Creature[8] = 0
			map.MapCreateColCreature(3017298127, -1, 4900464, 2160, 6646, 2230,1)
		end

	elseif Num + 1 == 500 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100039)--"XX(场景)的全球友人已收到500朵鲜花,接受总数到1000时会为在场玩家送上丰厚好礼,请再接再厉。"
		msg.AddMsgEvent(MsgID, 6, MapID)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end
	local exp_award = {}
	exp_award[20]=	260
	exp_award[25]=	504
	exp_award[30]=	751
	exp_award[35]=	1361.2
	exp_award[40]=	1998
	exp_award[45]=	2818
	exp_award[50]=	3688
	exp_award[55]=	4608
	exp_award[60]=	5578
	exp_award[65]=	6598
	exp_award[70]=	7668
	exp_award[75]=	8788
	exp_award[80]=	9958
	exp_award[85]=	11178
	exp_award[90]=	12448
	exp_award[95]=	13768
	exp_award[100]=	15138
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local level = math.floor(rolelevel/5)*5
	if level < 100 then
		role.AddRoleExp(MapID, InstanceID, RoleID, exp_award[level])
	elseif level == 100 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, 4000, 102)
	end

end

--鲜花能否使用
function I3303807_CanUse(MapID, InstanceID, TypeID, RoleID)
	local a = EXPO_Creature[7] --黄帝友人
	local b = EXPO_Creature[8] --龙城友人

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)

	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 2) then-----------------------------------------根据奖励物品而定
		return 40, false
	end

	if MapID == 3017298127 then
		if math.abs(x-2150)>25 or math.abs(z-2234)>25 then
			return 43 ,false
		else
			if b == 0 or b == nil then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1516)--"周围没有全球友人,无法使用鲜花"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				return 32 ,false
			else
				return 0 ,false
			end
		end
	elseif MapID == 3017299663 then
		if math.abs(x-2095)>25 or math.abs(z-2670)>25 then
			return 43 ,false
		else
			if a == 0 or a == nil then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1516)--"周围没有全球友人,无法使用鲜花"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				return 32 ,false
			else
				return 0 ,false
			end
		end
	else
		return 43,false
	end
end

aux.RegisterItemEvent(3303807, 1, "I3303807_OnUse")
aux.RegisterItemEvent(3303807, 0, "I3303807_CanUse")





--世博请柬使用效果
function I3303809_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local i = math.random(1,1000)
	local x = math.random(1,100)
	local Temp = 0

	role.AddRoleItem(MapID, InstanceID, RoleID, 3303806, 1, -1, 8, 420) ---世博纪念章
	if i >= 10 and i <= 290 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303801, 4, -1, 8, 420) ---欢迎您*4,--29%
	elseif i >= 291 and i <= 310 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303805, 1, -1, 8, 420) ---大奖兑换券*1,--2%
		Temp = 3303805
	elseif i >= 311 and i <= 350 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303808, 4, -1, 8, 420) ---称号：世博活动达人*1,--4%
		Temp = 3303808
	elseif i >= 351 and i <= 400 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303007, 1, -1, 8, 420) ---珍肴灵种*1,--5%
		Temp = 3303007
	elseif i >= 401 and i <= 475 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303010, 1, -1, 8, 420) ---生命灵水*1----7.5%
		Temp = 3303010
	elseif i >= 476 and i <= 550 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303012, 1, -1, 8, 420) ---培土灵肥*1--7.5%
		Temp = 3303012
	elseif i >= 550 and i <= 610 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303802, 1, -1, 8, 420) ---“世博会”*1---6%
	elseif i >= 611 and i <= 650 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303803, 1, -1, 8, 420) ---“上海”*1---4%
	elseif i >= 651 and i <= 670 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3303804, 1, -1, 8, 420) ---“EXPO”*1---2%
		Temp = 3303804
	elseif i >= 671 and i <= 1000 then
		role.AddRoleSilver(MapID, InstanceID, RoleID, math.random(20000,35000), 102) ---2j-3.5j ----33%
	end

	local Role_MapID, Role_InstanceID = role.GetRoleMapID(TargetID)
	if x >= 15 then
		role.AddRoleItem(Role_MapID, Role_InstanceID, TargetID, 3303801, 4, -1, 8, 420) ---欢迎您*4,--85%
	elseif x >= 1 then
		role.AddRoleItem(Role_MapID, Role_InstanceID, TargetID, 3303802, 1, -1, 8, 420) ---世博会”*1--15%
	end

	if Temp ~= 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 102, 100040)--"【江湖传闻】XXX(玩家A)对xxx(玩家B)使用了世博请柬，获得了世博纪念章和xxx(上面需广播道具)。"
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 4, Temp)
		msg.DispatchBroadcast(MsgID,-1,-1,-1)
	end
end

--世博请柬使用条件
function I3303809_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 2402)   --目标玩家行囊的空间不足
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		return 32, false
	end

	local FreeSize1 = role.GetBagFreeSize(RoleID)
	if FreeSize1 < 2 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 168)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 40, false
	end
end

aux.RegisterItemEvent(3303809, 1, "I3303809_OnUse")
aux.RegisterItemEvent(3303809, 0, "I3303809_CanUse")
