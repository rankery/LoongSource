
function feisheng_clickbutton(MapID, InstanceID, RoleID)
    if FeiSheng_trigger == 1 then
		local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if level >= 80 then
			if (role.GetSoarValue(RoleID) == 0 or role.GetSoarValue(RoleID) == 2) and not (role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 40001) or role.IsRoleCompleteQuest(MapID, InstanceID, RoleID, 40001)) then
				role.AddQuest(RoleID, 40001)
			end
			if (role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 40001) or role.IsRoleCompleteQuest(MapID, InstanceID, RoleID, 40001)) and role.GetSoarValue(RoleID) ~= 1 then
				role.SetSoarValue(RoleID, 2)
			end
		end
	else
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3114)--飞升系统尚未开放，敬请期待！
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end

aux.RegisterRoleEvent(39, "feisheng_clickbutton")

--完成任务
function feishengquest_Oncheckaccept(MapID, InstanceID, QuestID, RoleID, NPCID)

    if role.GetSoarValue(RoleID) == 1 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3181)	--您已经飞升，无法接取此任务
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	    return 0
	else
	    return 1
	end


end

aux.RegisterQuestEvent(40004, 4, "feishengquest_Oncheckaccept")
aux.RegisterQuestEvent(40005, 4, "feishengquest_Oncheckaccept")
aux.RegisterQuestEvent(40006, 4, "feishengquest_Oncheckaccept")
aux.RegisterQuestEvent(40007, 4, "feishengquest_Oncheckaccept")

-- npc对话，兑换过程
function x4900702_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then    	--开始对话
	    if KuiLingBaoShu_Start == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3136)	--白眉仙官:\n        葵灵宝树乃是世间灵物，三百年一开花，三百年一结果，长成之后能聚天地正气，妖魔不侵。本仙欲将其植于凌霄天宫之下以阻混沌戾气外泄，不知少侠可否相助？ \n        小提示：该活动持续一个半小时，参与活动将有机会获得稀有强化道具和大量经验。活动结束以后您可以继续用材料兑换勇者令。\n        玩家必须组队才能领取各种道具。当队伍中有6名成员时，浇水或施肥获得的经验奖励等于单人时的6倍。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 16)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 3133)	-- "领取仙界灵水"
			msg.AddMsgEvent(MsgID, 21, 17)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 3134)	-- "领取仙界灵肥"
			msg.AddMsgEvent(MsgID, 21, 18)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 3135)	-- "领取混沌经验箱"
			msg.AddMsgEvent(MsgID, 21, 19)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			if role.GetSoarValue(RoleID) == 2 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3115)	--白眉仙官:\n        本仙乃是司库仙官，天庭之上的寿宴庆典，觐见巡游等诸事一应由本仙筹办。想必侠士此行乃是为求飞升之道，本仙正好可以给予你一些力所能及的帮助。只要你将一些天界人间共用的材料交付于本仙，本仙就将这些勇者令赠送于你。至于能拿到多少勇者令，就看你的诚心了。另外，最早飞升的五个人的名字将被我记在天书之上上奏天帝，侠士若想扬名可要抓紧才是。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, 5)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 3116)	-- "换取勇者令"
				msg.AddMsgEvent(MsgID, 21, 19)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3117)	--白眉仙官:\n        得道飞升之人应以天下苍生为重，少些尘世杂念，少些嗔痴贪欲。凡不尊天命，不敬神佛，斗狠好杀者，必遭天谴。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, 19)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 5 then
	    --[[local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"])
		local CurTime = tonumber(os.date("%j"))
		if k ~= CurTime then
		    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"], CurTime)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"], 0)
	        local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3118)	  --白眉仙官：\n        只有矿石和布料这两类东西才能在天界派上用场。你可以选择其中一种类跟我兑换勇者令。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 6)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 3119)	-- "缴纳矿石"
			msg.AddMsgEvent(MsgID, 21, 7)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 3120)	-- "缴纳布料"
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
	   else
		    local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"])
			if b < 10 then]]
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3118)	  --白眉仙官：\n        只有矿石和布料这两类东西才能在天界派上用场。你可以选择其中一种类跟我兑换勇者令。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, 6)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 3119)	-- "缴纳矿石"
				msg.AddMsgEvent(MsgID, 21, 7)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 3120)	-- "缴纳布料"
				msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			--[[else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3121)	  --白眉仙官：\n        您每天只能用材料兑换10次勇者令，请准备好材料明天再来兑换。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
    		end
		end]]
	elseif TalkIndex == 6 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 3122)	  --白眉仙官：\n        所有的石料，玉石，铁矿，银矿，金矿都能用来换取勇者令。每天只能兑换10次，请选择您想要兑换的类型：
	    msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 8)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3123)	-- "10超绝材料兑换200勇者令"
		msg.AddMsgEvent(MsgID, 21, 9)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3124)	-- "10极品材料兑换35勇者令"
		msg.AddMsgEvent(MsgID, 21, 10)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3125)	-- "10稀有材料兑换6勇者令"
		msg.AddMsgEvent(MsgID, 21, 11)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3126)	-- "10优质材料兑换1勇者令"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 7 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 3127)	  --白眉仙官：\n        所有的麻布，棉布，皮革，丝绸，素纱都能用来换取勇者令。每天只能兑换10次，请选择您想要兑换的类型：
	    msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.AddMsgEvent(MsgID, 21, 12)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3123)	-- "10超绝材料兑换200勇者令"
		msg.AddMsgEvent(MsgID, 21, 13)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3124)	-- "10极品材料兑换35勇者令"
		msg.AddMsgEvent(MsgID, 21, 14)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3125)	-- "10稀有材料兑换6勇者令"
		msg.AddMsgEvent(MsgID, 21, 15)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 3126)	-- "10优质材料兑换1勇者令"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex <= 11 and TalkIndex >= 8 then
	    local FreeSize = role.GetBagFreeSize(RoleID)
		if FreeSize < 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3128)	  --白眉仙官：\n        您行囊中的空位不足。请保证您身上多于一个空位再来兑换。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 6)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		--[[local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"])
		local CurTime = tonumber(os.date("%j"))
		local bool_times = true
		if k ~= CurTime then
		    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"], CurTime)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"], 0)
	    else
		    local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"])
			if b >= 10 then
                bool_times = false
    		end
		end
		if 	bool_times then]]
		    local num_type = {}
			num_type[8] = {10, 200, 2000901,2000902,2000903,2000904,2000905}
			num_type[9] = {10, 35, 1160001,2000005,2000042,2000010,2000045,1160002,2000015,2000048,2000020,2000051,2000025,2000054}
			num_type[10] = {10, 6, 1160101,2000004,2000041,1160102,2000009,2000044,1160103,2000014,2000047,2000019,2000050,2000024,2000053}
			num_type[11] = {10, 1, 1160201,2000003,2000040,2000008,2000043,1160202,2000013,2000046,2000018,2000049,2000023,2000052}
		    local bool_duihuan = false
			local allnum = 0
			for k,v in pairs(num_type[TalkIndex]) do
			    if k > 2 then
					local i = role.GetRoleItemNum(RoleID, v)
					allnum = i + allnum
				end
				if num_type[TalkIndex][1] <= allnum then
				    bool_duihuan = true
					break
				end
			end
			local shengyu = 10
			if bool_duihuan then
			    for k,v in pairs(num_type[TalkIndex]) do
					if k > 2 then
						local i = role.GetRoleItemNum(RoleID, v)
						if i < shengyu then
							role.RemoveFromRole(MapID, InstanceID, RoleID, v, i, 420)
							shengyu = shengyu - i
						else
							role.RemoveFromRole(MapID, InstanceID, RoleID, v, shengyu, 420)
							break
						end
					end
				end
				role.AddRoleItem(MapID, InstanceID, RoleID, 1351203, num_type[TalkIndex][2], -1, 8, 420)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3129)	  --白眉仙官：\n        您没有足够的材料来进行兑换，请确认您的行囊中有指定类型的材料。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, 6)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
		--[[else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3121)	  --白眉仙官：\n        您每天只能用材料兑换10次勇者令，请准备好材料明天再来兑换。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end]]
	elseif TalkIndex <= 15 and TalkIndex >= 12 then
	    local FreeSize = role.GetBagFreeSize(RoleID)
		if FreeSize < 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3128)	  --白眉仙官：\n        您行囊中的空位不足。请保证您身上多于一个空位再来兑换。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, 7)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		--[[local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"])
		local CurTime = tonumber(os.date("%j"))
		local bool_times = true
		if k ~= CurTime then
		    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_Update"], CurTime)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"], 0)
	    else
		    local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiShengDuihuan_times"])
			if b >= 10 then
                bool_times = false
    		end
		end
		if 	bool_times then]]
		    local num_type = {}
			num_type[12] = {10, 200, 2000906,2000908,2000907,2000910,2000909}
			num_type[13] = {10, 35,2000205,2000242,2000215,2000248,2000210,2000245,2000225,2000254,2000220,2000251}
			num_type[14] = {10, 6,1160104,2000204,2000241,1160106,2000214,2000247,1160105,2000209,2000244,2000224,2000253,2000219,2000250}
			num_type[15] = {10, 1,2000203,2000240,2000213,2000246,2000208,2000243,2000223,2000252,2000218,2000249}
		    local bool_duihuan = false
			local allnum = 0
			for k,v in pairs(num_type[TalkIndex]) do
			    if k > 2 then
					local i = role.GetRoleItemNum(RoleID, v)
					allnum = i + allnum
				end
				if num_type[TalkIndex][1] <= allnum then
				    bool_duihuan = true
					break
				end
			end
			local shengyu = 10
			if bool_duihuan then
			    for k,v in pairs(num_type[TalkIndex]) do
					if k > 2 then
						local i = role.GetRoleItemNum(RoleID, v)
						if i < shengyu then
							role.RemoveFromRole(MapID, InstanceID, RoleID, v, i, 420)
							shengyu = shengyu - i
						else
							role.RemoveFromRole(MapID, InstanceID, RoleID, v, shengyu, 420)
							break
						end
					end
				end
				role.AddRoleItem(MapID, InstanceID, RoleID, 1351203, num_type[TalkIndex][2], -1, 8, 420)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3129)	  --白眉仙官：\n        您没有足够的材料来进行兑换，请确认您的行囊中有指定类型的材料。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, 7)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		--[[else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3121)	  --白眉仙官：\n        您每天只能用材料兑换10次勇者令，请准备好材料明天再来兑换。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)]]
		end
	elseif TalkIndex == 16 then
	    if KuiLingBaoShu_Complete == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3137)	  --白眉仙官：\n        葵灵宝树已经长成，不再需要这些养料了。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			if TeamID ~= nil and TeamID ~= 4294967295 then
				local FreeSize = role.GetBagFreeSize(RoleID)
				if FreeSize < 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 3140)	  --白眉仙官：\n        您行囊中的空位不足。请保证您身上多于一个空位再来领取。
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local i = role.GetRoleItemNum(RoleID, 1351202)
					if i < 5 then
						role.AddRoleItem(MapID, InstanceID, RoleID, 1351202, 5-i, -1, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 3138)	  --白眉仙官：\n        这是X个XXX，请拿好。用完之后可以再来找本仙领取。\n        小提示：活动中每个人都可以获得120次浇水和施肥经验，只要队伍中有一人浇水或者施肥，全队的所有处于昆仑的玩家都将获得经验奖励。
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.AddMsgEvent(MsgID, 9, 5-i)
						msg.AddMsgEvent(MsgID, 4, 1351202)
						msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 3139)	  --白眉仙官：\n        每个人只能携带5个天界灵水，5个天界灵肥，1个混沌经验箱。您不能领取更多的该物品了。
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3179)	  --白眉仙官：\n        您必须加入一个队伍才能领取此物品。队伍的人数越多，您能获得的浇水与施肥经验奖励就越高。当队伍的成员达到6人时，浇水或施肥一次所能获得经验将是一个人时的6倍。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 17 then
        if KuiLingBaoShu_Complete == 1 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3137)	  --白眉仙官：\n        葵灵宝树已经长成，不再需要这些养料了。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			if TeamID ~= nil and TeamID ~= 4294967295 then
				local FreeSize = role.GetBagFreeSize(RoleID)
				if FreeSize < 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 3140)	  --白眉仙官：\n        您行囊中的空位不足。请保证您身上多于一个空位再来领取。
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local i = role.GetRoleItemNum(RoleID, 1351201)
					if i < 5 then
						role.AddRoleItem(MapID, InstanceID, RoleID, 1351201, 5-i, -1, 8, 420)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 3138)	  --白眉仙官：\n        这是X个XXX，请拿好。用完之后可以再来找本仙领取。\n        小提示：活动中每个人都可以获得120次浇水和施肥经验，只要队伍中有一人浇水或者施肥，全队的所有处于昆仑的玩家都将获得经验奖励。
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.AddMsgEvent(MsgID, 9, 5-i)
						msg.AddMsgEvent(MsgID, 4, 1351201)
						msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 3139)	  --白眉仙官：\n        每个人只能携带5个天界灵水，5个天界灵肥，1个混沌经验箱。您不能领取更多的该物品了。
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
						msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3179)	  --白眉仙官：\n        您必须加入一个队伍才能领取此物品。队伍的人数越多，您能获得的浇水与施肥经验奖励就越高。当队伍的成员达到6人时，浇水或施肥一次所能获得经验将是一个人时的6倍。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 18 then
	    local FreeSize = role.GetBagFreeSize(RoleID)
		if FreeSize < 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 3140)	  --白眉仙官：\n        您行囊中的空位不足。请保证您身上多于一个空位再来领取。
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
			msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
		    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
			if TeamID ~= nil and TeamID ~= 4294967295 then
				if KuiLingBaoShu_Start == 1 and KuiLingBaoShu_jilu[RoleID] == nil then
					KuiLingBaoShu_jilu[RoleID] = {jingyanxiang = 1, guaiwu = 0, jiaoshui = 0}
					role.AddRoleItem(MapID, InstanceID, RoleID, 1351214, 1, -1, 8, 420)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 3141)	  --白眉仙官：\n        混沌经验箱乃是天界灵宝，可以将妖魔之魂封印并转化成真气储存起来。击杀前来阻止葵灵宝树生长的那些妖魔可以累积经验箱中储存的妖魔之魂（队伍中任何一个玩家击杀妖魔之魂将为所有的队伍成员的混沌经验箱增加一个妖魔之魂）。当经验箱中的妖魔之魂达到一定数量之后，使用它可以获得大量经验。
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif KuiLingBaoShu_Start == 1 and KuiLingBaoShu_jilu[RoleID].jingyanxiang == 0 then
					KuiLingBaoShu_jilu[RoleID].jingyanxiang = 1
					role.AddRoleItem(MapID, InstanceID, RoleID, 1351214, 1, -1, 8, 420)
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 3141)	  --白眉仙官：\n        混沌经验箱乃是天界灵宝，可以将妖魔之魂封印并转化成真气储存起来。击杀前来阻止葵灵宝树生长的那些妖魔可以累积经验箱中储存的妖魔之魂（队伍中任何一个玩家击杀妖魔之魂将为所有的队伍成员的混沌经验箱增加一个妖魔之魂）。当经验箱中的妖魔之魂达到一定数量之后，使用它可以获得大量经验。
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif role.GetRoleItemNum(RoleID, 1351214) > 1 or KuiLingBaoShu_jilu[RoleID].jingyanxiang == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 3142)	  --白眉仙官：\n        每次活动每个人只能领取一个混沌经验箱。
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
					msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 3179)	  --白眉仙官：\n        您必须加入一个队伍才能领取此物品。队伍的人数越多，您能获得的浇水与施肥经验奖励就越高。当队伍的成员达到6人时，浇水或施肥一次所能获得经验将是一个人时的6倍。
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.AddMsgEvent(MsgID, 21, -1)	-- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 207)	-- "返回"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end
end


-- 注册相应事件
aux.RegisterCreatureEvent(4900702, 7, "x4900702_OnTalk")
