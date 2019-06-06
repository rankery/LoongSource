XinChunDengMi_Attend = {}				-- 是否在此活动时间内参加过
XinChunDengMi_Questions = {}				-- 问题的各种信息
XinChunDengMi_RewardsRecevied = {}	-- 是否已经领取奖品(1是没有领)
XinChunDengMi_Role_Point = {}			-- 玩家获得的分数，对应角色id
XinChunDengMi_Question_Num = 439	-- 灯谜的题目数
XinChunDengMi_Time_Counter = 30			-- 答题的倒计时
XinChunDengMi_Time = {}					-- 存放答题开始时的系统时间，对应角色id
XinChunDengMi_RightAnswer = {}			-- 正确答案在四个选项中的索引
XinChunDengMi_CreatureID = {}			-- 存放创建新春灯谜考官后返回的id，用于活动结束后删除npc
DengMi_Question_Table = {}

dengmi_levelexp = {}
dengmi_levelexp[10] = 4100
dengmi_levelexp[11] = 4800
dengmi_levelexp[12] = 5400
dengmi_levelexp[13] = 6100
dengmi_levelexp[14] = 6900
dengmi_levelexp[15] = 7600
dengmi_levelexp[16] = 8500
dengmi_levelexp[17] = 9300
dengmi_levelexp[18] = 10200
dengmi_levelexp[19] = 11200
dengmi_levelexp[20] = 13000
dengmi_levelexp[21] = 15000
dengmi_levelexp[22] = 17300
dengmi_levelexp[23] = 19800
dengmi_levelexp[24] = 22400
dengmi_levelexp[25] = 25200
dengmi_levelexp[26] = 28100
dengmi_levelexp[27] = 31200
dengmi_levelexp[28] = 34400
dengmi_levelexp[29] = 37800
dengmi_levelexp[30] = 41300
dengmi_levelexp[31] = 48600
dengmi_levelexp[32] = 56200
dengmi_levelexp[33] = 64300
dengmi_levelexp[34] = 72700
dengmi_levelexp[35] = 81600
dengmi_levelexp[36] = 90800
dengmi_levelexp[37] = 100400
dengmi_levelexp[38] = 110400
dengmi_levelexp[39] = 120800
dengmi_levelexp[40] = 131600
dengmi_levelexp[41] = 154800
dengmi_levelexp[42] = 181800
dengmi_levelexp[43] = 210600
dengmi_levelexp[44] = 240900
dengmi_levelexp[45] = 273000
dengmi_levelexp[46] = 306600
dengmi_levelexp[47] = 342000
dengmi_levelexp[48] = 378900
dengmi_levelexp[49] = 417600
dengmi_levelexp[50] = 470200
dengmi_levelexp[51] = 525300
dengmi_levelexp[52] = 582700
dengmi_levelexp[53] = 642600
dengmi_levelexp[54] = 704800
dengmi_levelexp[55] = 769500
dengmi_levelexp[56] = 836500
dengmi_levelexp[57] = 906000
dengmi_levelexp[58] = 977800
dengmi_levelexp[59] = 1052100
dengmi_levelexp[60] = 1144500
dengmi_levelexp[61] = 1204500
dengmi_levelexp[62] = 1265900
dengmi_levelexp[63] = 1328500
dengmi_levelexp[64] = 1392500
dengmi_levelexp[65] = 1457700
dengmi_levelexp[66] = 1524100
dengmi_levelexp[67] = 1591900
dengmi_levelexp[68] = 1660900
dengmi_levelexp[69] = 1731300
dengmi_levelexp[70] = 1802900
dengmi_levelexp[71] = 1875700
dengmi_levelexp[72] = 1949900
dengmi_levelexp[73] = 2025300
dengmi_levelexp[74] = 2102100
dengmi_levelexp[75] = 2180100
dengmi_levelexp[76] = 2259300
dengmi_levelexp[77] = 2339900
dengmi_levelexp[78] = 2421700
dengmi_levelexp[79] = 2504900
dengmi_levelexp[80] = 2616700
dengmi_levelexp[81] = 3500000
dengmi_levelexp[82] = 5200000
dengmi_levelexp[83] = 7600000
dengmi_levelexp[84] = 9300000
dengmi_levelexp[85] = 10600000
dengmi_levelexp[86] = 13440000
dengmi_levelexp[87] = 15618000
dengmi_levelexp[88] = 16820000
dengmi_levelexp[89] = 17700000
dengmi_levelexp[90] = 19320000
dengmi_levelexp[91] = 23719606
dengmi_levelexp[92] = 28520000
dengmi_levelexp[93] = 31679550
dengmi_levelexp[94] = 33379200
dengmi_levelexp[95] = 34908250
dengmi_levelexp[96] = 36748800
dengmi_levelexp[97] = 38803050
dengmi_levelexp[98] = 44934400
dengmi_levelexp[99] = 51359805
dengmi_levelexp[100] = 58044000
dengmi_levelexp[101] = 64000000
dengmi_levelexp[102] = 66563455
dengmi_levelexp[103] = 69941705
dengmi_levelexp[104] = 73441712
dengmi_levelexp[105] = 77554046
dengmi_levelexp[106] = 81320031
dengmi_levelexp[107] = 85216560
dengmi_levelexp[108] = 89246539
dengmi_levelexp[109] = 93959179
dengmi_levelexp[110] = 98280000

-- npc对话，整个答题过程
function x4900401_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then                                                --开始对话
	    XinChunDengMi_InitNullTable(RoleID)
		if XinChunDengMi_RewardsRecevied[RoleID] == 1 then                    --如果问题答完，奖励还没有给
			local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local x = XinChunDengMi_Role_Point[RoleID]/10
			local reward
			if x <= 0 or x == nil then                                        --答对问题的数量
				reward = 0
			else
				reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
			end
			if x < 10 and x >=0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2109)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题，获得###点经验，请再接再厉！
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif x >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2110)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题。恭喜您超过10题，除获得###点经验外，还将额外获得新春小红包1枚，请确定您的行囊中有足够空位以领取奖励，若背包已满，请清空背包再来领奖！\n   [小提示：奖励未领取的情况下不能参加新一轮的灯谜喜乐会。]
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 2111)	-- "领取奖品"
				msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 2112)	-- "稍后领取"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		else
		    if DengMi_Time() then                        --如果没有奖励判断位不为1，则判断是否在活动中
			    if XinChunDengMi_Questions[RoleID].NUM < 20 and XinChunDengMi_Questions[RoleID].NUM > 0 then
					XinChunDengMi_GenerateQuestion(RoleID, TargetID)
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2101)	-- "新春灯谜考官：\n    本次“新春灯谜喜乐会”已经开始，所有10级以上玩家均可参与答题并赚取经验，答对10题以上还可额外获得新春小红包，请问您是否要参与活动？"
					msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 2102)	-- "参加[灯谜喜乐会]"
					msg.AddMsgEvent(MsgID, 21, 9)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2103)	-- "不参加"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else                                      --如果不在活动时间
			    if XinChunDengMi_Role_Point[RoleID] > 0 then           --如果玩家上次没答完，则给奖励
				    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
					local x = XinChunDengMi_Role_Point[RoleID]/10
					local reward
					if x <= 0 or x == nil then
						reward = 0
					else
						reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
					end
					if x < 10 and x >=0 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 2109)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题，获得###点经验，请再接再厉！
						msg.AddMsgEvent(MsgID, 9, x)
						msg.AddMsgEvent(MsgID, 9, reward)
						msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					elseif x >= 10 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 2110)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题。恭喜您超过10题，除获得###点经验外，还将额外获得新春小红包1枚，请确定您的行囊中有足够空位以领取奖励，若背包已满，请清空背包再来领奖！\n   [小提示：奖励未领取的情况下不能参加新一轮的灯谜喜乐会。]
						msg.AddMsgEvent(MsgID, 9, x)
						msg.AddMsgEvent(MsgID, 9, reward)
						msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 2111)	-- "领取奖品"
						msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 2112)	-- "稍后领取"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				elseif XinChunDengMi_Role_Point[RoleID] == 0 then             --如果玩家没有能拿的奖励，则提示下次再来
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2107)	-- "新春灯谜考官：\n    答题赚经验，灯谜乐新春！\n    “新春灯谜喜乐会”于每天的[11:00 – 12:00]、[19:00 – 20:00]各开启一次，所有10级以上玩家均可参与活动并获得经验奖励。在一次活动中答对10道以上问题的玩家还可额外获得“新春小红包”一个！"
					msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
	    end
	elseif TalkIndex == 4 then                             -- 如果玩家在活动时间没有奖励可拿，想答题则……
	    local Player_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	    if Player_level < 10 then	-- 等级低于10级的话弹出提示
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2104)		-- "新春灯谜考官：\n    您的等级不足10级，请先将等级提升至10级再来参加本活动。"
			msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    elseif XinChunDengMi_Attend[RoleID] == 1 then
	        local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2105)		-- "您已参加过本次新春灯谜活动。“新春灯谜喜乐会”于每天的[11:00 – 12:00]、[19:00 – 20:00]各开启一次，请下个时间段再来。"
			msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    else
			-- “答题活动现在开始”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2106)      -- “灯谜活动现在开始”
			msg.AddMsgEvent(MsgID, 21, 6)
			msg.AddMsgEvent(MsgID, 1, 206)
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    end
	elseif TalkIndex == 6 then                       --灯谜开始后，自动出题
		XinChunDengMi_GenerateQuestion(RoleID, TargetID)
	elseif TalkIndex == 0 or TalkIndex == 1 or TalkIndex == 2 or TalkIndex == 3 then    --玩家选择选项后会有不同的结果
	    if XinChunDengMi_Time[RoleID] == nil then
		    -- 答题结束
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 40, -1)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)                            --答题结束，给予奖励
			local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local x = XinChunDengMi_Role_Point[RoleID]/10
			local reward
			if x <= 0 or x == nil then
				reward = 0
			else
				reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
			end
			if x < 10 and x >=0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2109)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题，获得###点经验，请再接再厉！
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif x >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2110)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题。恭喜您超过10题，除获得###点经验外，还将额外获得新春小红包1枚，请确定您的行囊中有足够空位以领取奖励，若背包已满，请清空背包再来领奖！\n   [小提示：奖励未领取的情况下不能参加新一轮的灯谜喜乐会。]
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 2111)	-- "领取奖品"
				msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 2112)	-- "稍后领取"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		else
			-- 计算答题花费的时间
			local time1 = XinChunDengMi_Time[RoleID]
			local dtime = os.difftime(os.time(), time1)
			-- 判断答案是否正确
			if XinChunDengMi_RightAnswer[RoleID] == TalkIndex then
				-- 回答所花的时间不同，得分不同
				if dtime <= 30 then				-- 30秒内回答出正确答案，获得题目全部分数
					-- 灯谜得分
					XinChunDengMi_Role_Point[RoleID] = XinChunDengMi_Role_Point[RoleID] + 10
				end
			end
			-- 产生题目
			if XinChunDengMi_Questions[RoleID].NUM < 20 then    -- 答题数小于20题时继续产生题目
				XinChunDengMi_GenerateQuestion(RoleID, TargetID)
			else
				-- 答题结束
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 40, -1)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)                            --答题结束，给予奖励
				local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
				local x = XinChunDengMi_Role_Point[RoleID]/10
				local reward
				if x <= 0 or x == nil then
					reward = 0
				else
					reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
				end
				if x < 10 and x >=0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2109)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题，获得###点经验，请再接再厉！
					msg.AddMsgEvent(MsgID, 9, x)
					msg.AddMsgEvent(MsgID, 9, reward)
					msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif x >= 10 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2110)	-- "新春灯谜考官：\n    本次灯谜喜乐会您共答对##题。恭喜您超过10题，除获得###点经验外，还将额外获得新春小红包1枚，请确定您的行囊中有足够空位以领取奖励，若背包已满，请清空背包再来领奖！\n   [小提示：奖励未领取的情况下不能参加新一轮的灯谜喜乐会。]
					msg.AddMsgEvent(MsgID, 9, x)
					msg.AddMsgEvent(MsgID, 9, reward)
					msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 2111)	-- "领取奖品"
					msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 2112)	-- "稍后领取"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			end
		end
	elseif TalkIndex == 10 then                         --玩家在点击确定要领取奖励之后，具体的发奖过程
	    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local x = XinChunDengMi_Role_Point[RoleID]/10
		local reward
		if x <= 0 or x == nil or x > 20 then
		    reward = 0
		else
		    reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
		end
		if x < 10 and x >=0 then
			role.AddRoleExp(MapID, InstanceID, RoleID, reward)
			XinChunDengMi_Role_Point[RoleID] = 0                             --清空分数和奖励标志位
			XinChunDengMi_RewardsRecevied[RoleID] = 0
		elseif x >= 10 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if FreeSize < 2 then
                local MsgID = msg.BeginMsgEvent()
			    msg.AddMsgEvent(MsgID, 20, 2108)	-- "新春灯谜考官：\n    您行囊的空间不足，请清理以后再来！
			    msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
			    msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
			    msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    if x == 20 then
				    role.SigTitleEvent(RoleID, 190)--获得称号 才高八斗
					-- 广播新春灯谜考官活动开始
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 100, 2124)    --恭喜XXX答对了新春灯谜的所有20道题目，获得“才高八斗”称号！
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.DispatchWorldMsgEvent(MsgID)
				end
				role.AddRoleExp(MapID, InstanceID, RoleID, reward)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303004, 1, -1, 8, 420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3303001, 1, -1, 8, 420)
				XinChunDengMi_Role_Point[RoleID] = 0
				XinChunDengMi_RewardsRecevied[RoleID] = 0
	        end
		end
	end
end




-- 活动开始
function XinChun_OnTimerMin(actID)
    local curmon = tonumber(os.date("%m"))
	local curday = tonumber(os.date("%d"))
	if (curmon == 2 and curday >= 20) or (curmon == 3 and curday <= 7)then
	    local TypeID = cre.GetCreatureTypeID(3017299663, -1, XinChunDengMi_CreatureID[15])
	    if TypeID ~= 4900406 then
		    map.MapDeleteCreature(3017299663, -1, XinChunDengMi_CreatureID[15])
	        map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[16])
		    XinChunDengMi_CreatureID[15] = map.MapCreateCreature(3017299663, -1, 4900406, 2139, 18345, 2424)--黄帝城食神
	        XinChunDengMi_CreatureID[16] = map.MapCreateCreature(3017298127, -1, 4900406, 2171, 6646, 2224)--龙城食神
		end
	elseif curmon >= 2 and curday >= 5 then
	    local TypeID = cre.GetCreatureTypeID(3017299663, -1, XinChunDengMi_CreatureID[15])
	    if TypeID ~= 4900405 then
		    map.MapDeleteCreature(3017299663, -1, XinChunDengMi_CreatureID[15])
	        map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[16])
		    XinChunDengMi_CreatureID[15] = map.MapCreateCreature(3017299663, -1, 4900405, 2139, 18345, 2424)--黄帝城食神
	        XinChunDengMi_CreatureID[16] = map.MapCreateCreature(3017298127, -1, 4900405, 2171, 6646, 2224)--龙城食神
		end
	end
    local curhour = os.date("%H")
	local curmin = os.date("%M")
	if curhour ~= "00" and curmin ~= "00" then
	    curhour = tonumber(os.date("%H"))
	    curmin = tonumber(os.date("%M"))
	else
	    if curhour == "00" then
		    curhour = 0
		end
		if curmin == "00" then
		    curmin = 0
		end
	end
	if (curhour == 11 or curhour == 19) and curmin == 28 then
        XinChunDengMi_RewardsRecevied = {}	-- 是否已经领取奖品(1是没有领)
        XinChunDengMi_Role_Point = {}			-- 玩家获得的分数，对应角色id
	elseif (curhour == 11 or curhour == 19) and curmin == 29 then
		-- 设置随机种子
		math.randomseed(os.time())
	    -- 初始化题库
		XinChunDengMi_InitQuestionTable()
		XinChunDengMi_RewardsRecevied = {}	-- 是否已经领取奖品(1是没有领)
        XinChunDengMi_Role_Point = {}			-- 玩家获得的分数，对应角色id
    elseif (curhour == 11 or curhour == 19) and curmin == 30 then
		-- 设置随机种子
		math.randomseed(os.time())
		-- 初始化题库
		XinChunDengMi_InitQuestionTable()

		-- 广播新春灯谜考官活动开始
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 2113)    --答题赚经验，灯谜乐新春！新春灯谜喜乐会已经开始，所有10级以上玩家均可前往各大主城的新春灯谜考官处参与本次活动，表现优异者还有机会获得新春小红包，请各位快快行动！
		msg.DispatchWorldMsgEvent(MsgID)

    elseif (curhour ~= 11 and curhour ~= 19) or curmin < 30 then
		-- 清空表中内容
		XinChunDengMi_Attend = {}				-- 是否在此活动时间内参加过
		XinChunDengMi_Questions = {}				-- 问题数
		XinChunDengMi_Time = {}					-- 存放答题开始时的系统时间，对应角色id
		XinChunDengMi_RightAnswer = {}			-- 正确答案在四个选项中的索引
		DengMi_Question_Table = {}
	end
end

-- 活动开始
function XinChun_OnStart(actID)
	-- 在各城市中刷出npc新春灯谜考官,大农丞
    -- 初始化题库
	XinChunDengMi_InitQuestionTable()
	XinChunDengMi_CreatureID[1] = map.MapCreateCreature(3017298127, -1, 4900401, 2171, 6646, 2219)   --灯谜NPC
	XinChunDengMi_CreatureID[2] = map.MapCreateCreature(3017298383, -1, 4900401, 928, 10000, 2007)
	XinChunDengMi_CreatureID[3] = map.MapCreateCreature(3017299663, -1, 4900401, 2139, 18345, 2419)
	XinChunDengMi_CreatureID[4] = map.MapCreateCreature(3017299919, -1, 4900401, 1021, 20140, 975)
	XinChunDengMi_CreatureID[5] = map.MapCreateCreature(3017299151, -1, 4900401, 2038, 6365, 1799)
	XinChunDengMi_CreatureID[6] = map.MapCreateCreature(3017299407, -1, 4900401, 1822, 14024, 2280)
	XinChunDengMi_CreatureID[7] = map.MapCreateCreature(3017298127, -1, 4900417, 2166, 6646, 2219)--庚寅新春使者
	XinChunDengMi_CreatureID[8] = map.MapCreateCreature(3017298383, -1, 4900417, 923, 10000, 2007)
	XinChunDengMi_CreatureID[9] = map.MapCreateCreature(3017299663, -1, 4900417, 2134, 18345, 2419)
	XinChunDengMi_CreatureID[10] = map.MapCreateCreature(3017299919, -1, 4900417, 1016, 20140, 975)
	XinChunDengMi_CreatureID[11] = map.MapCreateCreature(3017299151, -1, 4900417, 2033, 6365, 1799)
	XinChunDengMi_CreatureID[12] = map.MapCreateCreature(3017299407, -1, 4900417, 1817, 14024, 2280)
	XinChunDengMi_CreatureID[13] = map.MapCreateCreature(3017298127, -1, 4900402, 2166, 6646, 2224)--大农成
	XinChunDengMi_CreatureID[14] = map.MapCreateCreature(3017298383, -1, 4900402, 923, 10000, 2012)
    XinChunDengMi_CreatureID[15] = map.MapCreateCreature(3017299663, -1, 4900404, 2139, 18345, 2424)--黄帝城食神
	XinChunDengMi_CreatureID[16] = map.MapCreateCreature(3017298127, -1, 4900404, 2171, 6646, 2224)--龙城食神
end

-- npc生存时间到，活动结束，在各城市中删除此npc
function XinChun_OnEnd(actID)
	map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[1])
	map.MapDeleteCreature(3017298383, -1, XinChunDengMi_CreatureID[2])
	map.MapDeleteCreature(3017299663, -1, XinChunDengMi_CreatureID[3])
	map.MapDeleteCreature(3017299919, -1, XinChunDengMi_CreatureID[4])
	map.MapDeleteCreature(3017299151, -1, XinChunDengMi_CreatureID[5])
	map.MapDeleteCreature(3017299407, -1, XinChunDengMi_CreatureID[6])
	map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[7])
	map.MapDeleteCreature(3017298383, -1, XinChunDengMi_CreatureID[8])
	map.MapDeleteCreature(3017299663, -1, XinChunDengMi_CreatureID[9])
	map.MapDeleteCreature(3017299919, -1, XinChunDengMi_CreatureID[10])
	map.MapDeleteCreature(3017299151, -1, XinChunDengMi_CreatureID[11])
	map.MapDeleteCreature(3017299407, -1, XinChunDengMi_CreatureID[12])
	map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[13])
	map.MapDeleteCreature(3017298383, -1, XinChunDengMi_CreatureID[14])
	map.MapDeleteCreature(3017299663, -1, XinChunDengMi_CreatureID[15])
	map.MapDeleteCreature(3017298127, -1, XinChunDengMi_CreatureID[16])

end



-- 活动开始
function LaBa_OnStart(actID)
	-- 广播新春灯谜考官活动开始
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 2114)    --虎啸一声山海动，龙腾三界吉祥来！庚寅年新春系列活动已经正式拉开帷幕，现在正在进行的是腊八活动与小年活动，包含了“灯谜喜乐会”、“年货大采购”、“珍材大耕种”、“新春美食大烹饪”等丰富内容及各种优厚奖励，活动详情可至各主城新春使者处咨询，欢迎您踊跃参与，祝您新春愉快！
	msg.DispatchWorldMsgEvent(MsgID)
end


function LaBa_OnTimerMin(actID)
    local curhour = os.date("%H")
	local curmin = os.date("%M")
	if curhour ~= "00" and curmin ~= "00" then
	    curhour = tonumber(os.date("%H"))
	    curmin = tonumber(os.date("%M"))
	else
	    if curhour == "00" then
		    curhour = 0
		end
		if curmin == "00" then
		    curmin = 0
		end
	end
	if curhour % 3 == 0 and curmin == 0 then
	    -- 广播新春灯谜考官活动开始
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 100, 2114)    --虎啸一声山海动，龙腾三界吉祥来！庚寅年新春系列活动已经正式拉开帷幕，现在正在进行的是腊八活动与小年活动，包含了“灯谜喜乐会”、“年货大采购”、“珍材大耕种”、“新春美食大烹饪”等丰富内容及各种优厚奖励，活动详情可至各主城新春使者处咨询，欢迎您踊跃参与，祝您新春愉快！
	    msg.DispatchWorldMsgEvent(MsgID)
	end
end

-- 注册相应事件
aux.RegisterCreatureEvent(4900401, 7, "x4900401_OnTalk")
aux.RegisterActEvent(71, 4, "XinChun_OnTimerMin")
aux.RegisterActEvent(71, 2, "XinChun_OnStart")
aux.RegisterActEvent(71, 3, "XinChun_OnEnd")
aux.RegisterActEvent(72, 4, "LaBa_OnTimerMin")
aux.RegisterActEvent(72, 2, "LaBa_OnStart")

