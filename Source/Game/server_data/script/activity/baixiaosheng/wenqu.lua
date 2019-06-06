WenQu_Attend = {}					-- 是否在此活动时间内参加过
WenQu_Questions = {}				-- 问题数
WenQu_RightAnswer = {}				-- 正确答案在四个选项中的索引
WenQu_Normal_Question_Point = 10	-- 文曲普通题目的分数
WenQu_Hard_Question_Point = 20 		-- 文曲困难题目的分数
WenQu_Time = {}						-- 存放答题开始时的系统时间，对应角色id
WenQu_Role_Point = {}				-- 玩家获得的分数，对应角色id
WenQu_Stepindex = {}				-- 每个玩家的活动步骤
WenQu_Question_Type = {}			-- 当前题目类型，文曲困难或文曲普通
WenQu_Role_Level = {}				-- 玩家当前等级
WenQu_Time_Counter = 30				-- 答题的倒计时
BaiXiaoYin_TypeID = 2612616			-- 百晓印的typeid
WenQu_Current_Question = {}      	-- 玩家当前答题

BaiXiaoSheng_Hard_Question_Num = 212		-- 百晓困难的题目数
BaiXiaoSheng_Normal_Question_Num = 259	-- 百晓普通的题目数
WenQu_Hard_Question_Num = 126			-- 文曲困难的题目数
WenQu_Normal_Question_Num = 195			-- 文曲普通的题目数

WQ_CallDeep = 0							-- 递归调用深度

WenQu_Hard_Question_Table = {}
WenQu_Normal_Question_Table = {}

WenQu_Npc_Accept	= 	0			-- 是否可以参加活动

g_WQ_RoleNum = 0						-- 参加文曲答题活动的玩家个数
tbl_WQ_RoleID = {}						-- 以g_WQ_RoleNum为索引对应玩家的RoleID
tbl_WQ_Index = {}						-- 以RoleID为索引对应玩家的答题Index
tbl_WQ_Questions = {}					-- 玩家已经回答过的题目

-- npc对话，整个答题过程
function x3020751_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if WenQu_Npc_Accept == 1 then

		if TalkIndex == -1 then
			-- 初始化空表
			WenQu_InitNullTable(RoleID)
		end

		-- 用到的属性
		WenQu_Role_Level[tbl_WQ_Index[RoleID]] = role.GetRoleLevel(MapID, InstanceID, RoleID)

		if WenQu_Stepindex[tbl_WQ_Index[RoleID]] == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 213)	-- "您是否决定参加华夏文曲夺星活动？"
			msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"

			msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"

			msg.AddMsgEvent(MsgID, 24, TargetID)
			WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 2
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

		elseif WenQu_Stepindex[tbl_WQ_Index[RoleID]] == 2 then
			if TalkIndex == 4 then			-- 选择了确定参加按钮后
				if WenQu_Role_Level[tbl_WQ_Index[RoleID]] <= 15 then	-- 等级低于15级的话弹出提示
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 214)		-- "文曲星君活动必须在15级以上才可参与"
					msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
					WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

				elseif role.GetRoleItemNum(RoleID, BaiXiaoYin_TypeID) < 5 then
					-- 百晓印数小于5个无法参加
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 214)		-- "需要5个百晓印"
					msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
					WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

				elseif WenQu_Attend[tbl_WQ_Index[RoleID]] == 1 then	-- 在此活动时间内已参加过
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 215)		-- "您已参加过本次文曲星君活动，可继续参加下个时间的活动。"
					msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
					WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

				else
					-- “答题活动现在开始”
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 205)
					msg.AddMsgEvent(MsgID, 21, 4)
					msg.AddMsgEvent(MsgID, 1, 206)
					msg.AddMsgEvent(MsgID, 24, TargetID)
					WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 3
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

					-- 从背包中减去5个百晓印
					role.RemoveFromRole(MapID, InstanceID, RoleID, BaiXiaoYin_TypeID, 5, 300)
					-- 初始化玩家得分
					role.SetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"], 0)
					WenQu_Attend[tbl_WQ_Index[RoleID]] = 1				-- 已参加过本次活动
					WenQu_Current_Question[tbl_WQ_Index[RoleID]] = {question, index}
					-- 将MapID和InstanceID存入scriptdata
					role.SetRoleScriptData(RoleID, 1, RoleDataType["RoleMapID"], MapID)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["RoleInstanceID"], MapID)

				end
			elseif TalkIndex == 5 then
				WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
			else
				WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
			end


		elseif WenQu_Stepindex[tbl_WQ_Index[RoleID]] == 3 then				-- 答题开始

			-- 产生题目
			WenQu_GenerateQuestion(RoleID, TargetID)

			WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 4

		elseif WenQu_Stepindex[tbl_WQ_Index[RoleID]] == 4 then	-- 计算答题所用时间
			if TalkIndex == 5 then						-- 放弃活动则奖励玩家当前得分应得的奖励
				WenQu_RewardRole(MapID, InstanceID, RoleID)
			else
				-- 计算答题花费的时间
				local time1 = WenQu_Time[tbl_WQ_Index[RoleID]]
				local dtime = os.difftime(os.time(), time1)

				-- 判断答案是否正确
				if WenQu_RightAnswer[tbl_WQ_Index[RoleID]] == TalkIndex then
					local pt = role.GetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"])
					if pt == nil then
						pt = 0
					end

					-- 回答所花的时间不同，得分不同
					if dtime <= 10 then				-- 10秒内回答出正确答案，获得题目全部分数
						if WenQu_Question_Type[tbl_WQ_Index[RoleID]] == 1 then  	-- 文曲困难得分
							pt = pt + WenQu_Hard_Question_Point

						else							-- 文曲普通得分
							pt = pt + WenQu_Normal_Question_Point

						end

					elseif dtime <= 20 then			-- 20秒内回答出正确答案，获得题目一般分数
						if WenQu_Question_Type[tbl_WQ_Index[RoleID]] == 1 then  	-- 文曲困难得分
							pt = pt + WenQu_Hard_Question_Point*0.5

						else							-- 文曲普通得分
							pt = pt + WenQu_Normal_Question_Point*0.5

						end
					else								-- 30秒内回答出正确答案，获得20%的题目分数
						if WenQu_Question_Type[tbl_WQ_Index[RoleID]] == 1 then  	-- 文曲困难得分
							pt = pt + WenQu_Hard_Question_Point*0.2

						else							-- 文曲普通得分
							pt = pt + WenQu_Normal_Question_Point*0.2
						end
					end

					role.SetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"], pt)
				end

				-- 产生题目
				if WenQu_Questions[tbl_WQ_Index[RoleID]] <= 50 then    -- 答题数小于50题时继续产生题目
					WenQu_GenerateQuestion(RoleID, TargetID)
				else
					-- 答题结束
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 40, -1)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

					WenQu_RewardRole(MapID, InstanceID, RoleID)
					WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
				end
				-- 玩家离线
				if (role.IsRoleOnLine(MapID, InstanceID, RoleID) == 0) then
					-- 如果已经参加过活动，则获得当前分数对应的奖励，并且活动自动终止
					if (WenQu_Attend[tbl_WQ_Index[RoleID]] == 1) then
						WenQu_RewardRole(MapID, InstanceID, RoleID)
						WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
					end
				end
			end
		end
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 218)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end


-- 初始化空表的默认值
function WenQu_InitNullTable(RoleID)
	-- 设置一些空表的默认值
	if tbl_WQ_Index[RoleID] == nil then
		g_WQ_RoleNum = g_WQ_RoleNum + 1
		tbl_WQ_RoleID[g_WQ_RoleNum] = RoleID
		tbl_WQ_Index[RoleID] = g_WQ_RoleNum
		tbl_WQ_Questions[g_WQ_RoleNum] = {}
	end
	if WenQu_Stepindex[g_WQ_RoleNum] == nil then
		WenQu_Stepindex[g_WQ_RoleNum] = 1
	end
	if WenQu_Attend[g_WQ_RoleNum] == nil then
	    WenQu_Attend[g_WQ_RoleNum] = 0
	end
	if WenQu_Questions[g_WQ_RoleNum] == nil then
		WenQu_Questions[g_WQ_RoleNum] = 1
	end
	if WenQu_Question_Type[g_WQ_RoleNum] == nil then
	    WenQu_Question_Type[g_WQ_RoleNum] = 0
	end

end

-- 初始化题库
function WenQu_InitQuestionTable()
	-- 将id_msg中文曲困难和百晓困难答题内容放入表中
	local x = 1
	for j=0, BaiXiaoSheng_Hard_Question_Num-1 do
		local id = 100001 + j*5
		WenQu_Hard_Question_Table[x] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
		x = x + 1
	end

	for j=0, WenQu_Hard_Question_Num-1 do
		local id = 110001 + j*5
		WenQu_Hard_Question_Table[x] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
		x = x + 1
	end

	-- 将id_msg中文曲普通和百晓普通答题内容放入表中
	local y = 1
	for j=0, BaiXiaoSheng_Normal_Question_Num-1 do
		local id = 120001 + j*5
		WenQu_Normal_Question_Table[y] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
		y = y + 1
	end

	for j=0, WenQu_Normal_Question_Num-1 do
		local id = 130001 + j*5
		WenQu_Normal_Question_Table[y] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
		y = y + 1
	end

end

-- 返回不重复的题目  参数0为困难题目 1为普通题目
function WQ_GetQuestion(RoleIndex, n)
	WQ_CallDeep = WQ_CallDeep + 1		-- 递归深度+1
	local question
	local index
	if n == 0 then
		if table.maxn(WenQu_Hard_Question_Table) == 0 then	--判断之前的活动进行过程中，是否因重载脚本或服务器重启等原因造成题库表被置空
			WenQu_InitQuestionTable()
		end
		index = math.random(1, table.maxn(WenQu_Hard_Question_Table))
		question = WenQu_Hard_Question_Table[index]
	else
		if table.maxn(WenQu_Normal_Question_Table) == 0 then	--判断之前的活动进行过程中，是否因重载脚本或服务器重启等原因造成题库表被置空
			WenQu_InitQuestionTable()
		end
		index = math.random(1, table.maxn(WenQu_Normal_Question_Table))
		question = WenQu_Normal_Question_Table[index]
	end

	if tbl_WQ_Questions[RoleIndex] == nil then
		local question_table = {}
		tbl_WQ_Questions[RoleIndex] = {question_table}
		table.insert(tbl_WQ_Questions[RoleIndex], question.Content)
		return question
	end

	local flag = 0
	for k, v in pairs(tbl_WQ_Questions[RoleIndex]) do
		if v == question.Content then
			flag = 1
			break
		end
	end

	if flag == 1 then
		if WQ_CallDeep >= 30 then			-- 递归深度超过10的话就直接返回重复的题
			return question
		else
			return WQ_GetQuestion(RoleIndex, n)
		end
	else
		table.insert(tbl_WQ_Questions[RoleIndex], question.Content)
		return question
	end
end

-- 产生题目和选项
function WenQu_GenerateQuestion(RoleID, TargetID)
	local RoleIndex = tbl_WQ_Index[RoleID]
	if WenQu_Questions[RoleIndex] > 30 then  -- 答题数大于30题时抽取百晓困难和文曲困难类的题目

		-- 在困难表中随机抽取出一道题
		WQ_CallDeep = 0
		WenQu_Current_Question[RoleIndex].question = WQ_GetQuestion(RoleIndex, 0)

		local MsgID = msg.BeginMsgEvent()

		msg.AddMsgEvent(MsgID, 40, WenQu_Current_Question[RoleIndex].question.Content)	-- 题目内容
		msg.AddMsgEvent(MsgID, 1, 217)
		local pt = role.GetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"])
		msg.AddMsgEvent(MsgID, 12, pt)		-- 得分

		-- 正确答案在四个选项中的随机位置
		WenQu_RightAnswer[RoleIndex] = math.random(0, 3)
		local index = 0
		for i=0, 3 do									-- 四个选项
			if WenQu_RightAnswer[RoleIndex] == i then
				msg.AddMsgEvent(MsgID, 21, i)			-- 选项
				msg.AddMsgEvent(MsgID, 1, WenQu_Current_Question[RoleIndex].question.RightAnswer)	-- 选项内容
			else
				msg.AddMsgEvent(MsgID, 21, i)
				msg.AddMsgEvent(MsgID, 1, WenQu_Current_Question[RoleIndex].question.WrongAnswer1+index)
				index = index + 1
			end
		end

		WenQu_Question_Type[RoleIndex] = 1		-- 当前题目类型
		-- 将当前时间放入table中
		WenQu_Time[RoleIndex] = os.time()

		-- 倒计时
		msg.AddMsgEvent(MsgID, 9, WenQu_Time_Counter)
		-- 已回答题目数
		msg.AddMsgEvent(MsgID, 12, WenQu_Questions[RoleIndex]-1)


		msg.AddMsgEvent(MsgID, 24, TargetID)
		WenQu_Questions[RoleIndex] = WenQu_Questions[RoleIndex] + 1 	-- 已答题数+1
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	-- 产生普通题目
	else
		WQ_CallDeep = 0
		WenQu_Current_Question[RoleIndex].question = WQ_GetQuestion(RoleIndex, 1)

		local MsgID = msg.BeginMsgEvent()

		msg.AddMsgEvent(MsgID, 40, WenQu_Current_Question[RoleIndex].question.Content)	-- 题目内容
		msg.AddMsgEvent(MsgID, 1, 217)
		local pt = role.GetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"])
		msg.AddMsgEvent(MsgID, 12, pt)		-- 得分

		-- 正确答案在四个选项中的随机位置
		WenQu_RightAnswer[RoleIndex] = math.random(0, 3)
		local index = 0
		for i=0, 3 do
			if WenQu_RightAnswer[RoleIndex] == i then
				msg.AddMsgEvent(MsgID, 21, i)
				msg.AddMsgEvent(MsgID, 1, WenQu_Current_Question[RoleIndex].question.RightAnswer)
			else
				msg.AddMsgEvent(MsgID, 21, i)
				msg.AddMsgEvent(MsgID, 1, WenQu_Current_Question[RoleIndex].question.WrongAnswer1+index)
				index = index + 1
			end
		end

		WenQu_Question_Type[RoleIndex] = 0		-- 当前题目类型
		-- 将当前时间放入table中
		WenQu_Time[RoleIndex] = os.time()

		-- 倒计时30秒
		msg.AddMsgEvent(MsgID, 9, WenQu_Time_Counter)
		-- 已回答题目数
		msg.AddMsgEvent(MsgID, 12, WenQu_Questions[RoleIndex]-1)

		msg.AddMsgEvent(MsgID, 24, TargetID)
		WenQu_Questions[RoleIndex] = WenQu_Questions[RoleIndex] + 1 	-- 已答题数+1
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

end

-- 活动结束或者玩家掉线，给与玩家奖励
function WenQu_RewardRole(MapID, InstanceID, RoleID)

	-- 金钱
	local pt = role.GetRoleScriptData(RoleID, 1, RoleDataType["RolePoint"])
	local addmoney = 20000 * pt / 360
	role.AddRoleSilver(MapID, InstanceID, RoleID, addmoney, 0)

	-- 经验
	local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, RoleID)
	local addexp = LevelUpNeedExp * 150 / (WenQu_Role_Level[tbl_WQ_Index[RoleID]]+10)^2 * pt / 360 * 2
	role.AddRoleExp(MapID, InstanceID, RoleID, addexp)

	-- 将玩家id和得分存入WenQu_Role_Point
	table.insert(WenQu_Role_Point, {id = RoleID, point = pt})

	-- 增加物品(未完成)


	--增加进阶实力值

	Increase_shili(MapID, InstanceID, RoleID, 0, pt/10, "jinjieshili_A")

	WenQu_Stepindex[tbl_WQ_Index[RoleID]] = 1
end


-- 活动开始
function WenQu_OnStart(actID)
	g_WQ_RoleNum = 0
	tbl_WQ_RoleID = {}
	tbl_WQ_Index = {}
	tbl_WQ_Questions = {}

	-- 设置随机种子
	math.randomseed(os.time())
	-- 初始化题库
	WenQu_InitQuestionTable()
	-- 可以从npc接取答题活动
	WenQu_Npc_Accept = 1

	-- 广播文曲星君活动开始
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 212)
	msg.DispatchWorldMsgEvent(MsgID)

end


-- 活动结束。
function WenQu_OnEnd(actID)

	WenQu_Npc_Accept = 0	-- 活动结束

	-- 将WenQu_Role_Point表中内容按得分值降序排列
	table.sort(WenQu_Role_Point, function(a,b) return a.point > b.point end)

	-- 奖励活动前10名
	local rolenum = table.getn(WenQu_Role_Point)

	if rolenum > 0 then
		local n = 0
		if rolenum >= 10 then
			n = 10
		else
			n = rolenum
		end

		-- 保存状元和前10名RoleID
		local roleid = {}
		if n ~= 0 then
			local i = 0
			for k, v in pairs(WenQu_Role_Point) do
				-- 将RoleID存入roleid中，奖励前10名时用到
				roleid[i] = v.id
				i = i + 1
				if i >= n then
					break
				end
			end
		end

		-- 广播状元玩家名字
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 216)
		msg.AddMsgEvent(MsgID, 2, roleid[0])
		msg.DispatchWorldMsgEvent(MsgID)

		-- 奖励状元和前10名
		-- 状元
		local mapid = role.GetRoleScriptData(roleid[0], 1, RoleDataType["RoleMapID"])
		local instanceid = role.GetRoleScriptData(roleid[0], 1, RoleDataType["RoleInstanceID"])
		if mapid ~= nil then
			role.AddRoleSilver(mapid, instanceid, roleid[0], 100000, 104)
			-- 奖励道具，Buff ，韬略（待定）
			role.AddRoleItem(mapid, instanceid, roleid[0], 2612630, 1, -1, 3, 104)
		end

		-- 榜眼
		mapid = role.GetRoleScriptData(roleid[1], 1, RoleDataType["RoleMapID"])
		instanceid = role.GetRoleScriptData(roleid[1], 1, RoleDataType["RoleInstanceID"])
		if mapid ~= nil then
			role.AddRoleSilver(mapid, instanceid, roleid[1], 80000, 104)
			-- 奖励道具，Buff ，韬略（待定）
			role.AddRoleItem(mapid, instanceid, roleid[1], 2612631, 1, -1, 3, 104)
		end

		-- 探花
		mapid = role.GetRoleScriptData(roleid[2], 1, RoleDataType["RoleMapID"])
		instanceid = role.GetRoleScriptData(roleid[2], 1, RoleDataType["RoleInstanceID"])
		if mapid ~= nil then
			role.AddRoleSilver(mapid, instanceid, roleid[2], 50000, 104)
			-- 奖励道具，Buff ，韬略（待定）
			role.AddRoleItem(mapid, instanceid, roleid[2], 2612632, 1, -1, 3, 104)
		end

		-- 其余剩下的
		for i=3, n do
			mapid = role.GetRoleScriptData(roleid[i], 1, RoleDataType["RoleMapID"])
			instanceid = role.GetRoleScriptData(roleid[i], 1, RoleDataType["RoleInstanceID"])
			if mapid ~= nil then
				role.AddRoleSilver(mapid, instanceid, roleid[i], 20000, 104)
				-- 奖励道具，Buff ，韬略（待定）
			end
			i = i + 1
		end
	end

	-- 清空表中内容
	WenQu_Attend = {}
	WenQu_Questions = {}
	WenQu_RightAnswer = {}
	WenQu_Time = {}
	WenQu_Role_Point = {}
	WenQu_Stepindex = {}
	WenQu_Question_Type = {}
	WenQu_Hard_Question_Table = {}
	WenQu_Normal_Question_Table = {}
	WenQu_Role_Level = {}
	WenQu_Current_Question = {}
	tbl_WQ_RoleID = {}
	tbl_WQ_Index = {}
	tbl_WQ_Questions = {}
	g_WQ_RoleNum = 0
end

-- 注册相应事件
aux.RegisterCreatureEvent(3020751, 7, "x3020751_OnTalk")
aux.RegisterActEvent(42, 3, "WenQu_OnEnd")
aux.RegisterActEvent(42, 2, "WenQu_OnStart")
aux.RegisterActEvent(43, 3, "WenQu_OnEnd")
aux.RegisterActEvent(43, 2, "WenQu_OnStart")
