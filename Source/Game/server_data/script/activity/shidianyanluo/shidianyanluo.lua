ShiDianYanLuo_Questions = {}				-- 已答题数
ShiDianYanLuo_RightAnswer = {}			    -- 正确答案在四个选项中的索引
ShiDianYanLuo_Stepindex1 = {}		        -- 每个玩家的活动接取步骤
ShiDianYanLuo_RightPoint = {}	            -- 答对题目的个数
ShiDianYanLuo_Question_Step = {}            -- 每个玩家答题的步骤
ShiDianYanLuo_Question_Table = {}
ShiDianYanLuo_Role_Level = {}               -- 每个玩家的等级
ShiDianYanLuo_Role_Quest = {}               -- 每个玩家的任务
ShiDianYanLuo_Role_Attend = {}              -- 每个玩家是否参加了活动
ShiDianYanLuo_Role_Done = {}                -- 玩家是否完成了答题任务
ShiDianYanLuo_Question_Num = 105		    --题库中题目数
ShiDianYanLuo_Current_Question = {}			-- 玩家当前的题目

ShiDianYanLuo_Accept_Time = {3600}  	    -- npc存在时间为30分钟
ShiDianYanLuo_CanAccept = 0                 -- 是否能从npc处接取任务

g_SDYL_RoleNum = 0							-- 参加十殿阎罗答题活动的玩家个数
tbl_SDYL_RoleID = {}						-- 以g_SDYL_RoleNum为索引对应玩家的RoleID
tbl_SDYL_Index = {}							-- 以RoleID为索引对应玩家的答题Index
tbl_SDYL_Questions = {}						-- 玩家已经回答过的题目

SDYL_CallDeep = 0							-- 递归调用深度

--地图m02的随机点坐标
map_m05 = {}
--5个NPC阴间小鬼
map_m05[1]={x=1055,y=24286,z=1828}
map_m05[2]={x=1070,y=25207,z=1717}
map_m05[3]={x=1185,y=25026,z=1981}
map_m05[4]={x=1343,y=22712,z=2128}
map_m05[5]={x=1150,y=26532,z=1893}

--十王NPC

NPC = {}
NPC[1]={TypeID=3020801,x=936,y=19115,z=1184}
NPC[2]={TypeID=3020802,x=927,y=18887,z=1499}
NPC[3]={TypeID=3020803,x=630,y=24876,z=1648}
NPC[4]={TypeID=3020804,x=1175,y=26666,z=1893}
NPC[5]={TypeID=3020805,x=1639,y=22837,z=1913}
NPC[6]={TypeID=3020806,x=2328,y=16290,z=2658}
NPC[7]={TypeID=3020807,x=2700,y=28540,z=1825}
NPC[8]={TypeID=3020808,x=2312,y=20087,z=1333}
NPC[9]={TypeID=3020809,x=1910,y=22690,z=389}
NPC[10]={TypeID=3020810,x=1708,y=19826,z=495}

--刷出1个NPC阴间小鬼，刷出10王NPC
function ShiDianYanLuo_CreateCreature()
		local randPoint = math.random(1, 5)
		local x = map_m05[randPoint].x
		local y = map_m05[randPoint].y
		local z = map_m05[randPoint].z
		map.MapCreateCreature(3017299919, -1, 3020811, x, y, z)

		for i = 1,10 do
			local x1 = NPC[i].x
			local y1 = NPC[i].y
			local z1 = NPC[i].z
			local TypeID = NPC[i].TypeID
			map.MapCreateCreature(3017299919, -1, TypeID, x1, y1, z1)
		end
end

--活动开始事件
function ShiDianYanLuo_OnStart(actID)
	tbl_SDYL_RoleID = {}
	tbl_SDYL_Index = {}
	tbl_SDYL_Questions = {}
	g_SDYL_RoleNum = 0
	-- 设置随机种子
	math.randomseed(os.time())
	-- 初始化题库
	ShiDianYanLuo_InitQuestionTable()
	--刷出5个NPC阴间小鬼
	ShiDianYanLuo_CreateCreature()
	--全场景广播活动开始
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 220)
	msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)

    -- 开启鬼王使者任务接取
    ShiDianYanLuo_CanAccept = 1

end

-- 初始化Npc接取任务时间
function ShiDianYanLuo_OnInit(actID)
	act.InitEventTime("ShiDianYanLuo_Accept_Time", 1, actID)
end

-- 到了1个小时后鬼王使者关闭任务接取
function ShiDianYanLuo_OnTimer(actID)
    ShiDianYanLuo_CanAccept = 0
end

-- 初始化空表的默认值
function ShiDianYanLuo_InitNullTable(RoleID)
	-- 设置一些空表的默认值
	if tbl_SDYL_Index[RoleID] == nil then
		g_SDYL_RoleNum = g_SDYL_RoleNum + 1
		tbl_SDYL_RoleID[g_SDYL_RoleNum] = RoleID
		tbl_SDYL_Index[RoleID] = g_SDYL_RoleNum
		tbl_SDYL_Questions[g_SDYL_RoleNum] = {}
	end

	if ShiDianYanLuo_Stepindex1[g_SDYL_RoleNum] == nil then
		ShiDianYanLuo_Stepindex1[g_SDYL_RoleNum] = 1
	end
	if ShiDianYanLuo_Questions[g_SDYL_RoleNum] == nil then
		ShiDianYanLuo_Questions[g_SDYL_RoleNum] = 0
	end
	if ShiDianYanLuo_RightPoint[g_SDYL_RoleNum] == nil then
		ShiDianYanLuo_RightPoint[g_SDYL_RoleNum] = 0
	end
	if ShiDianYanLuo_Question_Step[g_SDYL_RoleNum] == nil then
		ShiDianYanLuo_Question_Step[g_SDYL_RoleNum] = 0
	end
    if ShiDianYanLuo_Role_Quest[g_SDYL_RoleNum] == nil then
        ShiDianYanLuo_Role_Quest[g_SDYL_RoleNum] = 0
    end
    if ShiDianYanLuo_Role_Attend[g_SDYL_RoleNum] == nil then
        ShiDianYanLuo_Role_Attend[g_SDYL_RoleNum] = 0
    end
    if ShiDianYanLuo_Role_Done[g_SDYL_RoleNum] == nil then
        ShiDianYanLuo_Role_Done[g_SDYL_RoleNum] = 0
    end

end

-- 鬼王使者npc对话
function n3600014_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    -- 任务接取已被关闭
    if ShiDianYanLuo_CanAccept ~= 1 then
		-- 过了接取任务时间，若玩家完成任务依然可以领取到奖励
		if role.GetRoleItemNum(RoleID, 2612661) > 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 242)     -- "十殿阎罗试炼通过"
			msg.AddMsgEvent(MsgID, 21, 4)       -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)      -- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			-- 删除轮转王令牌
			role.RemoveFromRole(MapID, InstanceID, RoleID, 2612661, 1, 101)
			-- 给与玩家奖励（未完成）


			return 0
		end

        -- npc的普通对话
        local MsgID = msg.BeginMsgEvent()
        msg.AddMsgEvent(MsgID, 20, 243)     -- "鬼王使者的普通对话"
        msg.AddMsgEvent(MsgID, 21, 4)       -- 确定按钮
        msg.AddMsgEvent(MsgID, 1, 206)      -- "确定"
        msg.DispatchRoleMsgEvent(RoleID, MsgID)
        return 0
    end

    -- 玩家背包里有轮转王令牌的话，给与奖励，并重新开始
	if role.GetRoleScriptData(RoleID, 1, RoleDataType["SDYL_Done"]) == 1 then
		if role.GetRoleItemNum(RoleID, 2612661) > 0 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 242)     -- "十殿阎罗试炼通过"
			msg.AddMsgEvent(MsgID, 21, 4)       -- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)      -- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			-- 删除轮转王令牌
			role.RemoveFromRole(MapID, InstanceID, RoleID, 2612661, 1, 101)
			-- 给与玩家奖励（未完成）
			role.AddRoleExp(MapID, InstanceID, RoleID, 16800)

			--增加实力值
			Increase_shili(MapID, InstanceID, RoleID, 0, 25,"jinjieshili_A")

			return 0
		end
	end

	if TalkIndex == -1 then
		--初始化空表
		ShiDianYanLuo_InitNullTable(RoleID)
		-- 用到的属性
		ShiDianYanLuo_Role_Level[tbl_SDYL_Index[RoleID]] = role.GetRoleLevel(MapID, InstanceID, RoleID)
	end

	if ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 221)	-- "您是否决定参加十殿阎罗活动？"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"

		msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"

		msg.AddMsgEvent(MsgID, 24, TargetID)
		ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] = 2
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] == 2 then
		if TalkIndex == 4 then			-- 选择了确定参加按钮后
			if ShiDianYanLuo_Role_Level[tbl_SDYL_Index[RoleID]] < 55 then	-- 等级低于55级的话弹出提示
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 222)		-- "十殿阎罗活动必须在55级以上才可参与"
				msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
				ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] = 1
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			elseif ShiDianYanLuo_Role_Attend[tbl_SDYL_Index[RoleID]] == 1 then	-- 在此活动时间内已参加过
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 223)		-- "您已参加了十殿阎罗活动。"
				msg.AddMsgEvent(MsgID, 21, 4)		-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
				ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] = 1
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			else
				-- “活动现在开始”
				ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] = 1
				ShiDianYanLuo_Question_Step[tbl_SDYL_Index[RoleID]] = 1
				ShiDianYanLuo_Role_Attend[tbl_SDYL_Index[RoleID]] = 1       -- 已参加本次活动
				-- 初始化ScriptData
				role.SetRoleScriptData(RoleID, 1, RoleDataType["SDYL_Done"], 0)
				--给予玩家道具[鬼王令牌]
				role.QuestAddRoleItem(MapID, InstanceID, RoleID, 20061, 2612651, 1, -1, 102)
				ShiDianYanLuo_Current_Question[tbl_SDYL_Index[RoleID]] = {question, index}
			end
		else
			ShiDianYanLuo_Stepindex1[tbl_SDYL_Index[RoleID]] = 1
		end
	end
end

-- 初始化题库
function ShiDianYanLuo_InitQuestionTable()
	-- 将id_msg中十殿阎罗答题内容放入表中
	for i=1, ShiDianYanLuo_Question_Num do
		local id = 140001 + (i-1)*5
		ShiDianYanLuo_Question_Table[i] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
	end
end

-- 返回不重复的题目
function SDYL_GetQuestion(RoleIndex)
	SDYL_CallDeep = SDYL_CallDeep + 1		-- 递归深度+1
	local question
	local index
	index = math.random(1, table.maxn(ShiDianYanLuo_Question_Table))
	question = ShiDianYanLuo_Question_Table[index]

	if tbl_SDYL_Questions[RoleIndex] == nil then
		local question_table = {}
		tbl_SDYL_Questions[RoleIndex] = {question_table}
		table.insert(tbl_SDYL_Questions[RoleIndex], question.Content)
		return question
	end

	local flag = 0
	for k, v in pairs(tbl_SDYL_Questions[RoleIndex]) do
		if v == question.Content then
			flag = 1
			break
		end
	end

	if flag == 1 then
		if SDYL_CallDeep >= 10 then			-- 递归深度超过10的话就直接返回重复的题
			return question
		else
			return SDYL_GetQuestion(RoleIndex)
		end
	else
		table.insert(tbl_SDYL_Questions[RoleIndex], question.Content)
		return question
	end
end

-- 产生题目和选项
function ShiDianYanLuo_GenerateQuestion(RoleID, TargetID)
		local RoleIndex = tbl_SDYL_Index[RoleID]
		SDYL_CallDeep = 0
		ShiDianYanLuo_Current_Question[RoleIndex].question = SDYL_GetQuestion(RoleIndex)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 40, ShiDianYanLuo_Current_Question[RoleIndex].question.Content)	-- 题目内容
		msg.AddMsgEvent(MsgID, 1, 217)
		msg.AddMsgEvent(MsgID, 12, ShiDianYanLuo_RightPoint[RoleIndex])		-- 得分
		-- 正确答案在四个选项中的随机位置
		ShiDianYanLuo_RightAnswer[RoleIndex] = math.random(0, 3)
		local index = 0
		for i=0, 3 do
			if ShiDianYanLuo_RightAnswer[RoleIndex] == i then
				msg.AddMsgEvent(MsgID, 21, i)
				msg.AddMsgEvent(MsgID, 1, ShiDianYanLuo_Current_Question[RoleIndex].question.RightAnswer)
			else
				msg.AddMsgEvent(MsgID, 21, i)
				msg.AddMsgEvent(MsgID, 1, ShiDianYanLuo_Current_Question[RoleIndex].question.WrongAnswer1+index)
				index = index + 1
			end
		end
        -- 倒计时
		msg.AddMsgEvent(MsgID, 9, 30)
		-- 已回答题目数
		msg.AddMsgEvent(MsgID, 12, ShiDianYanLuo_Questions[RoleIndex])
		msg.AddMsgEvent(MsgID, 24, TargetID)
        msg.DispatchRoleMsgEvent(RoleID, MsgID)

		ShiDianYanLuo_Questions[RoleIndex] = ShiDianYanLuo_Questions[RoleIndex] + 1 	-- 已答题数+1
end

-- 接受对话任务
function ShiDianYanLuo_OnAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	local RoleIndex = tbl_SDYL_Index[RoleID]
    --根据携带物品，产生QuestID
    local num1 = role.GetRoleItemNum(RoleID, 2612651)
    local num2 = role.GetRoleItemNum(RoleID, 2612655)
    local num3 = role.GetRoleItemNum(RoleID, 2612658)
    if num1 >0 then
        ShiDianYanLuo_Role_Quest[RoleIndex] = 20062
    elseif num2 >0 then
        ShiDianYanLuo_Role_Quest[RoleIndex] = 20069
    elseif num3 >0 then
        ShiDianYanLuo_Role_Quest[RoleIndex] = 20072
    end

	nQuestion_OnTalk(MapID, InstanceID, NPCID, -1, RoleID, 1)
end



--秦广王，阎罗王，都市王答题对话
function nQuestion_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	--判断是否有任务
	if(role.IsRoleHaveQuest(MapID, InstanceID, RoleID, ShiDianYanLuo_Role_Quest[tbl_SDYL_Index[RoleID]]) == true) then

		if ShiDianYanLuo_Question_Step[tbl_SDYL_Index[RoleID]] == 1 then
			ShiDianYanLuo_GenerateQuestion(RoleID, TargetID)
			ShiDianYanLuo_Question_Step[tbl_SDYL_Index[RoleID]] = 2
		else
			if TalkIndex == 5 then      -- 放弃答题
			    -- 删除此任务  需重新接取
			    role.DeleteQuest(RoleID, ShiDianYanLuo_Role_Quest[tbl_SDYL_Index[RoleID]])
				-- 玩家已答题置空
				tbl_SDYL_Questions[tbl_SDYL_Index[RoleID]] = {}
				--答题得分清空
				ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] = 0
				ShiDianYanLuo_Questions[tbl_SDYL_Index[RoleID]] = 0
			else
				-- 如果答案正确，正确计数加1
				if ShiDianYanLuo_RightAnswer[tbl_SDYL_Index[RoleID]] == TalkIndex then
					ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] = ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] + 1
				end

				--产生题目 有三个答题任务，如果玩家答题任务未能在所有题库数量五分之一中正确回答五道题时
				--默认允许其过关
				if ShiDianYanLuo_Questions[tbl_SDYL_Index[RoleID]] < ShiDianYanLuo_Question_Num/5 and ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] < 5 then --答题数小于10题时继续产生题目
					ShiDianYanLuo_GenerateQuestion(RoleID, TargetID)

				else
					-- 答题结束
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 40, -1)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

					-- 如果答对5道题以上或者答完所有题，ShiDianYanLuo_RightPoint置为1
					ShiDianYanLuo_Role_Done[tbl_SDYL_Index[RoleID]] = 1
					ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] = 0 --重置答对数
					ShiDianYanLuo_Questions[tbl_SDYL_Index[RoleID]] = 0 --重置答题数
				end
			end
        end
    end
end

--接取任务
function n3020802_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	--任务目标随机数
	local rand = math.random(1,4)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 20, 226)	-- "近期恶鬼肆虐，请到鬼门关找人要他们加强警戒。"
	msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
	msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"

	msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
	msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"

	msg.AddMsgEvent(MsgID, 24, TargetID)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)
	if TalkIndex == 4 then
		if rand == 1 then
			role.AddQuest(RoleID,20063)
		elseif rand == 2 then
			role.AddQuest(RoleID,20064)
		elseif rand == 3 then
			role.AddQuest(RoleID,20065)
		elseif rand == 4 then
			role.AddQuest(RoleID,20066)
		end
	end
end


function question_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	--判断是否已答对5道题
	if ShiDianYanLuo_Role_Done[tbl_SDYL_Index[RoleID]] ~= 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 224)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断背包空闲空间是否足够
	if(role.GetBagFreeSize(RoleID) < 1) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 142)
		msg.DispatchRoleMsgEvent(MsgID)
		return 0
	end

	-- 重置
	ShiDianYanLuo_Role_Done[tbl_SDYL_Index[RoleID]] = 0

	return 1
end

-- 放弃任务
function Quest_OnCancel(MapID, InstanceID, QuestID, RoleID)
	-- 活动结束不对表进行操作
	if g_SDYL_RoleNum == 0 then
		-- 玩家已答题置空
		tbl_SDYL_Questions[tbl_SDYL_Index[RoleID]] = {}

		--中途放弃任务视为未参加活动
		ShiDianYanLuo_Role_Attend[tbl_SDYL_Index[RoleID]] = 0

		--答题得分清空
		ShiDianYanLuo_RightPoint[tbl_SDYL_Index[RoleID]] = 0
		ShiDianYanLuo_Questions[tbl_SDYL_Index[RoleID]] = 0

	end
	-- 除去所有令牌
	for i=2612651, 2612661 do
		ShiDianYanLuo_RemoveBar(MapID, InstanceID, RoleID, i)
	end
end

-- 除去玩家身上拥有的令牌
function ShiDianYanLuo_RemoveBar(MapID, InstanceID, RoleID, ItemID)
    if role.GetRoleItemNum(RoleID, ItemID) > 0 then
        role.RemoveFromRole(MapID, InstanceID, RoleID, ItemID, 1, 103)
    end
end

--完成任务十殿阎罗后，标记为未参加活动，并将玩家传送至鬼王使者
function Quest_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	-- 玩家已答题置空
	tbl_SDYL_Questions[tbl_SDYL_Index[RoleID]] = {}
	-- 置SDYL_Done为1
	role.SetRoleScriptData(RoleID, 1, RoleDataType["SDYL_Done"], 1)
	ShiDianYanLuo_Role_Attend[tbl_SDYL_Index[RoleID]] = 0
	role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017299919, 957, 20140, 848)
end

--20071 七殿阎罗 接取任务给予BUFF卞城神力[2016201]
function n20071_OnAccept(MapID, InstanceID, QuestID, OwnerID, AccepterID)
	unit.AddBuff(MapID, InstanceID, OwnerID, 2016201, OwnerID)
end

--20073 九殿阎罗 完成任务给予BUFF平等神力[2016301]
function n20073_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	unit.AddBuff(MapID, InstanceID, RoleID, 2016301, RoleID)
end

-- 活动结束
function ShiDianYanLuo_OnEnd(actID)
    ShiDianYanLuo_Stepindex1 = {}
    ShiDianYanLuo_Questions = {}
    ShiDianYanLuo_RightPoint = {}
    ShiDianYanLuo_Question_Step = {}
    ShiDianYanLuo_Role_Quest = {}
    ShiDianYanLuo_Role_Attend = {}
    ShiDianYanLuo_Role_Done = {}
	ShiDianYanLuo_Current_Question = {}
	tbl_SDYL_RoleID = {}
	tbl_SDYL_Index = {}
	tbl_SDYL_Questions = {}
	g_SDYL_RoleNum = 0
end
--aux.RegisterQuestEvent(20062, 0,"ShiDianYanLuo_OnAccept")
--aux.RegisterQuestEvent(20069, 0,"ShiDianYanLuo_OnAccept")
--aux.RegisterQuestEvent(20072, 0,"ShiDianYanLuo_OnAccept")
--[[ 注册相应事件
aux.RegisterCreatureEvent(3600014, 7, "n3600014_OnTalk")
aux.RegisterCreatureEvent(3020801, 7, "nQuestion_OnTalk")
aux.RegisterCreatureEvent(3020805, 7, "nQuestion_OnTalk")
aux.RegisterCreatureEvent(3020808, 7, "nQuestion_OnTalk")
aux.RegisterQuestEvent(20062, 5, "question_OnCheckComplete")
aux.RegisterQuestEvent(20069, 5, "question_OnCheckComplete")
aux.RegisterQuestEvent(20072, 5, "question_OnCheckComplete")

aux.RegisterActEvent(48, 0, "ShiDianYanLuo_OnInit")
aux.RegisterActEvent(48, 1, "ShiDianYanLuo_OnTimer")
aux.RegisterActEvent(48, 2, "ShiDianYanLuo_OnStart")
aux.RegisterActEvent(48, 3, "ShiDianYanLuo_OnEnd")
aux.RegisterActEvent(49, 0, "ShiDianYanLuo_OnInit")
aux.RegisterActEvent(49, 1, "ShiDianYanLuo_OnTimer")
aux.RegisterActEvent(49, 2, "ShiDianYanLuo_OnStart")
aux.RegisterActEvent(49, 3, "ShiDianYanLuo_OnEnd")
aux.RegisterActEvent(50, 0, "ShiDianYanLuo_OnInit")
aux.RegisterActEvent(50, 1, "ShiDianYanLuo_OnTimer")
aux.RegisterActEvent(50, 2, "ShiDianYanLuo_OnStart")
aux.RegisterActEvent(50, 3, "ShiDianYanLuo_OnEnd")
aux.RegisterActEvent(51, 0, "ShiDianYanLuo_OnInit")
aux.RegisterActEvent(51, 1, "ShiDianYanLuo_OnTimer")
aux.RegisterActEvent(51, 2, "ShiDianYanLuo_OnStart")
aux.RegisterActEvent(51, 3, "ShiDianYanLuo_OnEnd")



aux.RegisterQuestEvent(20061, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20062, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20063, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20064, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20065, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20066, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20067, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20068, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20069, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20070, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20071, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20072, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20073, 2, "Quest_OnCancel")
aux.RegisterQuestEvent(20074, 2, "Quest_OnCancel")

aux.RegisterQuestEvent(20074,1,"Quest_OnComplete")
aux.RegisterQuestEvent(20073,1,"n20073_OnComplete")

aux.RegisterCreatureEvent(3020802, 7, "n3020802_OnTalk")
aux.RegisterQuestEvent(20071, 0, "n20071_OnAccept")]]


