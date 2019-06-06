
-- 初始化空表的默认值
function XJ_XiaoLongNV_InitNullTable(RoleID)
	-- 设置一些空表的默认值
	if XJ_XiaoLongNV_Attend[RoleID] == nil then
		XJ_XiaoLongNV_Attend[RoleID] = 0
	end
	if XJ_XiaoLongNV_Questions[RoleID] == nil then
		XJ_XiaoLongNV_Questions[RoleID] = {NUM = 0, ID = 0, Quest = {} }  --NUM为将要答的题数（一共20），id为将要答的题号，Quest为表，记录所有答过的题号
	end
    if XJ_XiaoLongNV_Role_Point[RoleID] == nil then
        XJ_XiaoLongNV_Role_Point[RoleID] = 0
    end
	if XJ_XiaoLongNV_RewardsRecevied[RoleID] == nil then
	    XJ_XiaoLongNV_RewardsRecevied[RoleID] = 0
	end
end


-- 初始化题库
function XJ_XiaoLongNV_InitQuestionTable()
	-- 将id_msg中灯谜答题内容放入表中
	for i=1, XJ_XiaoLongNV_Question_Num do
		local id = 150001 + (i-1)*5
		XJ_DengMi_Question_Table[i] = {Content = id, RightAnswer = id+1, WrongAnswer1 = id+2, WrongAnswer2 = id+3, WrongAnswer3 = id+4}
	end

end

--[[活动结束给与玩家奖励
function XJ_XiaoLongNV_RewardRole(MapID, InstanceID, RoleID, TargetID, TalkIndex)
    if TalkIndex == 10 then
	    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
		local reward
		if x <= 0 or x == nil or x > 20 then
		    reward = 0
		else
		    reward = (300*x*level*level+x*XJ_DengMi_levelexp[level]+30000*level)/100/level
		end
		if x < 10 and x >=0 then
			role.AddRoleExp(MapID, InstanceID, RoleID, reward)
			XJ_XiaoLongNV_Role_Point[RoleID] = 0
			XJ_XiaoLongNV_RewardsRecevied[RoleID] = 0
		elseif x > 10 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if FreeSize < 1 then
                local MsgID = msg.BeginMsgEvent()
			    msg.AddMsgEvent(MsgID, 20, 2108)	-- "新春灯谜考官：\n    您行囊的空间不足，请清理以后再来！
			    msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
			    msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
			    msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    role.AddRoleExp(MapID, InstanceID, RoleID, reward)
			    role.AddRoleItem(MapID, InstanceID, TargetID, 4900401, 3, -1, 8, 420)
	            XJ_XiaoLongNV_Role_Point[RoleID] = 0
			    XJ_XiaoLongNV_RewardsRecevied[RoleID] = 0
	        end
		end
	elseif TalkIndex ~=10 then
		local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
		local reward
		if x <= 0 or x == nil then
		    reward = 0
		else
		    reward = (300*x*level*level+x*XJ_DengMi_levelexp[level]+30000*level)/100/level
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
		elseif x > 10 then
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
end]]




-- 返回不重复的题目
function XJ_DM_GetQuestion(RoleID, XJ_DM_CallDeep)
	XJ_DM_CallDeep = XJ_DM_CallDeep + 1		-- 递归深度+1
	local question
	local index
	index = math.random(6, XJ_XiaoLongNV_Question_Num)
	question = XJ_DengMi_Question_Table[index]

	local flag = 0
	for k, v  in pairs(XJ_XiaoLongNV_Questions[RoleID].Quest) do
		if v == question.Content then
			flag = 1
			break
		end
	end

	if flag == 1 then
		if XJ_DM_CallDeep >= 5 then			-- 递归深度超过5的话就直接返回重复的题
			return question
		else
			return XJ_DM_GetQuestion(RoleID, XJ_DM_CallDeep)
		end
	else
		return question
	end
end

-- 产生题目和选项
function XJ_XiaoLongNV_GenerateQuestion(RoleID, TargetID)
	local XJ_DM_CallDeep = 0
	local question = XJ_DM_GetQuestion(RoleID, XJ_DM_CallDeep)
	XJ_XiaoLongNV_Questions[RoleID].NUM = XJ_XiaoLongNV_Questions[RoleID].NUM + 1
	XJ_XiaoLongNV_Questions[RoleID].ID = question.Content
	local k = XJ_XiaoLongNV_Questions[RoleID].NUM
    XJ_XiaoLongNV_Questions[RoleID].Quest[k] = question.Content
	if k == 20 then                                           --这一句很关键，如果玩家在第二十道题目没有选，则可能会无法获得奖励。
	    XJ_XiaoLongNV_RewardsRecevied[RoleID] = 1
		XJ_XiaoLongNV_Attend[RoleID] = 1		               -- 已参加过本次活动
	    local CurTime = tonumber(os.date("%j"))
		role.SetRoleScriptData(RoleID, 1, RoleDataType["XJ_Dati_Date"], CurTime)
	end


	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 40, question.Content)	-- 题目内容
	msg.AddMsgEvent(MsgID, 1, 217)
	msg.AddMsgEvent(MsgID, 12, XJ_XiaoLongNV_Role_Point[RoleID])		-- 得分

	-- 正确答案在四个选项中的随机位置
	XJ_XiaoLongNV_RightAnswer[RoleID] = math.random(0, 3)
	local index = 0
	for i=0, 3 do
		if XJ_XiaoLongNV_RightAnswer[RoleID] == i then
			msg.AddMsgEvent(MsgID, 21, i)
			msg.AddMsgEvent(MsgID, 1, question.RightAnswer)
		else
			msg.AddMsgEvent(MsgID, 21, i)
			msg.AddMsgEvent(MsgID, 1, question.WrongAnswer1+index)
			index = index + 1
		end
	end
	-- 将当前时间放入table中
	XJ_XiaoLongNV_Time[RoleID] = os.time()

	-- 倒计时30秒
	msg.AddMsgEvent(MsgID, 9, XJ_XiaoLongNV_Time_Counter)

	-- 已回答题目数
	msg.AddMsgEvent(MsgID, 12, XJ_XiaoLongNV_Questions[RoleID].NUM - 1)
	msg.AddMsgEvent(MsgID, 24, TargetID)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

end

function XJ_DengMi_Time(RoleID)
    local CurTime = tonumber(os.date("%j"))
	local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["XJ_Dati_Date"])
	if CurTime ~= UpdateTime then
	    return true
	else
	    return false
	end
end

