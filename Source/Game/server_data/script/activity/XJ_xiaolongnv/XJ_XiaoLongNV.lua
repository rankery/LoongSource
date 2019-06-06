XJ_XiaoLongNV_Attend = {}				-- 是否在此活动时间内参加过
XJ_XiaoLongNV_Questions = {}				-- 问题的各种信息
XJ_XiaoLongNV_RewardsRecevied = {}	-- 是否已经领取奖品(1是没有领)
XJ_XiaoLongNV_Role_Point = {}			-- 玩家获得的分数，对应角色id
XJ_XiaoLongNV_Question_Num = 439	-- 灯谜的题目数
XJ_XiaoLongNV_Time_Counter = 30			-- 答题的倒计时
XJ_XiaoLongNV_Time = {}					-- 存放答题开始时的系统时间，对应角色id
XJ_XiaoLongNV_RightAnswer = {}			-- 正确答案在四个选项中的索引
--XJ_XiaoLongNV_CreatureID = {}			-- 存放创建小龙女考官后返回的id，用于活动结束后删除npc
XJ_DengMi_Question_Table = {}


-- npc对话，整个答题过程
function x4900449_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then                                                --开始对话
	    -- 设置随机种子
		math.randomseed(os.time())
		XJ_XiaoLongNV_InitNullTable(RoleID)
		if XJ_XiaoLongNV_RewardsRecevied[RoleID] == 1 then                    --如果问题答完，奖励还没有给
			local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
			local reward
			if x <= 0 or x == nil then                                        --答对问题的数量
				reward = 0
			else
				reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
			end
			if x < 10 and x >=0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2701)	-- "小龙女考官：\n    本次答题您共答对##题，获得###点经验，若想获得珍珠项链请您再接再厉！
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif x >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2702)	-- "小龙女考官：\n    本次答题您共答对##题。恭喜您超过10题，除获得###点经验外，您还将获得20个珍珠项链，请确认您的行囊拥有足够的空间！\n   [小提示：奖励未领取的情况下不能参加新一轮的百问小龙女活动。]
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
		    if XJ_DengMi_Time(RoleID) then                        --如果没有奖励判断位不为1，则判断是否在活动中
			    if XJ_XiaoLongNV_Questions[RoleID].NUM < 20 and XJ_XiaoLongNV_Questions[RoleID].NUM > 0 then
					XJ_XiaoLongNV_GenerateQuestion(RoleID, TargetID)
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2703)	-- "小龙女考官：\n    所有超过<color=0xffff0000>30<color=0xfffff7e0>级的玩家都可以在我这参加<color=0xff05ff00>【百问小龙女】<color=0xfffff7e0>活动，所有参与的玩家都可以获得丰厚的经验奖励。每当您答对10道题还会获得10个珍珠项链作为奖励，您可以在选秀投票人处将其投给您心仪的小龙女选手。（参与投票的玩家还有机会获得<color=0xffff6000>龙卫士<color=0xfffff7e0>称号奖励）"
					msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 2704)	-- "参加百问小龙女活动"
					msg.AddMsgEvent(MsgID, 21, 11)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2705)	-- "用金贝兑换珍珠项链"
					msg.AddMsgEvent(MsgID, 21, 12)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2706)	-- "关于小龙女选秀活动"
					msg.AddMsgEvent(MsgID, 21, 9)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2103)	-- "不参加"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else                                      --如果不在活动时间
			   --[[ if XJ_XiaoLongNV_Role_Point[RoleID] > 0 then           --如果玩家上次没答完，则给奖励
				    local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
					local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
					local reward
					if x <= 0 or x == nil then
						reward = 0
					else
						reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
					end
					if x < 10 and x >=0 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 2701)	-- "小龙女考官：\n    本次答题您共答对##题，获得###点经验，若想获得珍珠项链请您再接再厉！
						msg.AddMsgEvent(MsgID, 9, x)
						msg.AddMsgEvent(MsgID, 9, reward)
						msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					elseif x >= 10 then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 2702)	-- "小龙女考官：\n    本次答题您共答对##题。恭喜您超过10题，除获得###点经验外，您还将获得20个珍珠项链，请确认您的行囊拥有足够的空间！\n   [小提示：奖励未领取的情况下不能参加新一轮的百问小龙女活动。]
						msg.AddMsgEvent(MsgID, 9, x)
						msg.AddMsgEvent(MsgID, 9, reward)
						msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 2111)	-- "领取奖品"
						msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 2112)	-- "稍后领取"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				elseif XJ_XiaoLongNV_Role_Point[RoleID] == 0 then             --如果玩家没有能拿的奖励，则提示下次再来]]
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2707)	-- "小龙女考官：\n    今天您已经参加过答题活动了。\n    所有超过<color=0xffff0000>30<color=0xfffff7e0>级的玩家都可以在我这参加<color=0xff05ff00>【百问小龙女】<color=0xfffff7e0>活动，所有参与的玩家都可以获得丰厚的经验奖励。每当您答对10道题还会获得10个珍珠项链作为奖励，您可以在选秀投票人处将其投给您心仪的小龙女选手。（参与投票的玩家还有机会获得<color=0xffff6000>龙卫士<color=0xfffff7e0>称号奖励）"
					msg.AddMsgEvent(MsgID, 21, 11)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2705)	-- "用金贝兑换珍珠项链"
					msg.AddMsgEvent(MsgID, 21, 12)	-- 取消按钮
					msg.AddMsgEvent(MsgID, 1, 2706)	-- "关于小龙女选秀活动"
					msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				--end
			end
	    end
	elseif TalkIndex == 4 then                             -- 如果玩家在活动时间没有奖励可拿，想答题则……
	    local Player_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local CurTime = tonumber(os.date("%j"))
	    local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["XJ_Dati_Date"])
	    if Player_level < 30 then	-- 等级低于30级的话弹出提示
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2708)		-- "小龙女考官：\n    您的等级不足10级，请先将等级提升至10级再来参加本活动。"
			msg.AddMsgEvent(MsgID, 21, 9)		-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    elseif CurTime == UpdateTime then
	        local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2709)		-- "您今天已参加过百问小龙女活动，请明天再来。"
			msg.AddMsgEvent(MsgID, 21, 9)		-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 206)		-- "确定"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    else
			-- “答题活动现在开始”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2710)      -- “百问小龙女答题现在开始”
			msg.AddMsgEvent(MsgID, 21, 6)
			msg.AddMsgEvent(MsgID, 1, 206)
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    end
	elseif TalkIndex == 6 then                       --灯谜开始后，自动出题
		XJ_XiaoLongNV_GenerateQuestion(RoleID, TargetID)
	elseif TalkIndex == 0 or TalkIndex == 1 or TalkIndex == 2 or TalkIndex == 3 then    --玩家选择选项后会有不同的结果
	    if XJ_XiaoLongNV_Time[RoleID] == nil then
		    -- 答题结束
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 40, -1)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)                            --答题结束，给予奖励
			local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
			local reward
			if x <= 0 or x == nil then
				reward = 0
			else
				reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
			end
			if x < 10 and x >=0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2701)	-- "小龙女考官：\n    本次答题您共答对##题，获得###点经验，若想获得珍珠项链请您再接再厉！
				msg.AddMsgEvent(MsgID, 9, x)
				msg.AddMsgEvent(MsgID, 9, reward)
				msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif x >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 2702)	-- "小龙女考官：\n    本次答题您共答对##题。恭喜您超过10题，除获得###点经验外，您还将获得20个珍珠项链，请确认您的行囊拥有足够的空间！\n   [小提示：奖励未领取的情况下不能参加新一轮的百问小龙女活动。]
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
			local time1 = XJ_XiaoLongNV_Time[RoleID]
			local dtime = os.difftime(os.time(), time1)
			-- 判断答案是否正确
			if XJ_XiaoLongNV_RightAnswer[RoleID] == TalkIndex then
				-- 回答所花的时间不同，得分不同
				if dtime <= 30 then				-- 30秒内回答出正确答案，获得题目全部分数
					-- 灯谜得分
					XJ_XiaoLongNV_Role_Point[RoleID] = XJ_XiaoLongNV_Role_Point[RoleID] + 10
				end
			end
			-- 产生题目
			if XJ_XiaoLongNV_Questions[RoleID].NUM < 20 then    -- 答题数小于20题时继续产生题目
				XJ_XiaoLongNV_GenerateQuestion(RoleID, TargetID)
			else
				-- 答题结束
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 40, -1)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)                            --答题结束，给予奖励
				local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
				local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
				local reward
				if x <= 0 or x == nil then
					reward = 0
				else
					reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
				end
				if x < 10 and x >=0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2701)	-- "小龙女考官：\n    本次答题您共答对##题，获得###点经验，若想获得珍珠项链请您再接再厉！
					msg.AddMsgEvent(MsgID, 9, x)
					msg.AddMsgEvent(MsgID, 9, reward)
					msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif x >= 10 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2702)	-- "小龙女考官：\n    本次答题您共答对##题。恭喜您超过10题，除获得###点经验外，您还将获得10个珍珠项链，请确认您的行囊拥有足够的空间！\n   [小提示：奖励未领取的情况下不能参加新一轮的百问小龙女活动。]
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
		local x = XJ_XiaoLongNV_Role_Point[RoleID]/10
		local reward
		if x <= 0 or x == nil or x > 20 then
		    reward = 0
		else
		    reward = (300*x*level*level+x*dengmi_levelexp[level]+30000*level)/100/level
		end
		if x < 10 and x >=0 then
			role.AddRoleExp(MapID, InstanceID, RoleID, reward)
			XJ_XiaoLongNV_Role_Point[RoleID] = 0                             --清空分数和奖励标志位
			XJ_XiaoLongNV_RewardsRecevied[RoleID] = 0
		elseif x >= 10 then
			local FreeSize = role.GetBagFreeSize(RoleID)
			if FreeSize < 1 then
                local MsgID = msg.BeginMsgEvent()
			    msg.AddMsgEvent(MsgID, 20, 2711)	-- "小龙女考官：\n    您行囊的空间不足，请清理以后再来！
			    msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
			    msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
			    msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				role.AddRoleExp(MapID, InstanceID, RoleID, reward)
				role.AddRoleItem(MapID, InstanceID, RoleID, 3301056, 10, -1, 8, 420)
				XJ_XiaoLongNV_Role_Point[RoleID] = nil
				XJ_XiaoLongNV_RewardsRecevied[RoleID] = nil
				XJ_XiaoLongNV_Questions[RoleID] = nil
		        XJ_XiaoLongNV_Time[RoleID] = nil
		        XJ_XiaoLongNV_RightAnswer[RoleID] = nil
	        end
		end
	elseif TalkIndex == 11 then
	    local MsgID = msg.BeginMsgEvent()
        msg.AddMsgEvent(MsgID, 20, 2712)	--小龙女考官：\n    每十个金贝可以兑换一串珍珠项链，您需要兑换吗？
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 13)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 2705)	-- "兑换珍珠项链"
		msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 12 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2713)	--小龙女考官：\n    <color=0xffff6000>万元大奖<color=0xfffff7e0>及<color=0xffff6000>最终汽车豪礼<color=0xfffff7e0>究竟花落谁家？敬等您的参与！小龙女选秀活动正在举行，广大美女们均可至网站<color=0xff05ff00>xln.love0991.com<color=0xfffff7e0>报名参加，了解更多的活动信息。\n    另外，所有超过<color=0xffff0000>30<color=0xfffff7e0>级的玩家都可以在我这里用<color=0xffffae00>10个金贝来兑换1个珍珠项链<color=0xfffff7e0>，您可以在选秀投票人处将其投给您心仪的小龙女选手。
		msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
    elseif TalkIndex == 13 then
		local Player_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
        local JinBei = role.GetRoleItemNum(RoleID, 3301055)
        local FreeSize = role.GetBagFreeSize(RoleID)
        if Player_level < 30 then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 26, 2714)	--只有高于30级的玩家才可以兑换珍珠项链
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif JinBei < 10 then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 26, 2715)	--您需要至少10个金贝才可以兑换珍珠项链
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif FreeSize < 1 then
		    local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 26, 2716)	--请先清理好背包再来兑换珍珠项链
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 3301055, 10, 420) then
	            role.AddRoleItem(MapID, InstanceID, RoleID, 3301056, 1, -1, 8, 420)
            end
		end
	end
end


-- 注册相应事件
aux.RegisterCreatureEvent(4900449, 7, "x4900449_OnTalk")


