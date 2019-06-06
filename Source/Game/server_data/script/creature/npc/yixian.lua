--NPC医仙脚本——ID：4001056
--治疗内伤

--治疗内伤的基础金钱消耗
c4001056_BaseCost = {[1]    =   1,          [2]     =   15,         [3]     =   60,
                     [4]    =   135,        [5]     =   248,        [6]     =   400,
                     [7]    =   600,        [8]     =   870,        [9]		=   1200,
                     [10]	=   1650,       [11]    =   2100,       [12]	=   2750,
                     [13]	=   3500,       [14]    =   4350,       [15]    =   5330,
                     [16]	=   6450,       [17]    =   7680,       [18]	=   9200,
                     [19]	=   10950,      [20]	=   12850,      [21]	=   15000,
                     [22]	=   17300,      [23]	=   19850,      [24]	=   22650,
                     [25]	=   25650,      [26]	=   29300,      [27]	=   33200,
                     [28]	=   37440,      [29]	=   42000,      [30]	=   46950,
                     [31]	=   52250,
                    }

--医仙对话脚本
function c4001056_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    --如果是第一次对话
	if TalkIndex == -1 then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["Injury"], 1)
	end

	local stepindex = role.GetRoleScriptData(RoleID, 1, RoleDataType["Injury"])

	-- 用到的属性
	local injure	= role.GetRoleAttValue(MapID, InstanceID, RoleID, 64)
	local moneycur	= role.GetRoleSilver(MapID, InstanceID, RoleID)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	
	-- 计算治疗内伤所需要的金钱
	local k = math.floor(rolelevel/5)+1
	local moneyneed	= c4001056_BaseCost[k] * injure
	
	-- 第一步
	if stepindex == 1 then -- 判断角色内伤属性是否为0，
		local MsgID = msg.BeginMsgEvent()
		
		if injure == 0 then --若为0，则弹出对话框，提示：“您不需要进行任何治疗”，
			msg.AddMsgEvent(MsgID, 20, 	18)			--“您不需要进行任何治疗”
			msg.AddMsgEvent(MsgID, 21, 	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1, 	21)			--“确定”
			
			stepindex = 1                           --保持第一步
			
		else --若不为0，弹出治疗界面，界面上显示角色当前内伤值、所需费用以及“治疗”和“取消”两个按钮
			msg.AddMsgEvent(MsgID, 20,	19)			--“您的内伤为####，需要####金####银”
			msg.AddMsgEvent(MsgID, 9,	injure)		--内伤
			msg.AddMsgEvent(MsgID, 9,	moneyneed / 10000)	--金
			msg.AddMsgEvent(MsgID, 9,	moneyneed % 10000)	--银
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
			
			stepindex = 2                           --前进到第二步
		end
		
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		
	-- 第二步
	elseif stepindex == 2 then
	
		if TalkIndex == 4 then                      --确定
			-- 判断角色金钱是否足够	        
			if moneyneed > moneycur then            -- 若不足，则提示：
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 20) 		--“您的金钱不足，无法治疗”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			-- 若足够，则治疗成功，
			else
				-- 系统扣除对应金钱
				role.DecRoleSilver(MapID, InstanceID, RoleID, moneyneed, 452)
				-- 并将角色内伤属性恢复为0
				role.ModRoleAttValue(MapID, InstanceID, RoleID, 64, -injure)
			end
		end
		stepindex = 1
	end

	role.SetRoleScriptData(RoleID, 1, RoleDataType["Injury"], stepindex)
end

-- 注册
aux.RegisterCreatureEvent(4001056, 7, "c4001056_OnTalk")
