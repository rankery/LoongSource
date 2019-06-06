




--与凤翔红娘4900201对话，选择确定，扣除1金进入M01.与M01的红娘对话，传送出M01，回到凤翔。
function n4900201_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
        if MapID == 3017298383 then
		    if Marriage_Available == 1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 501) -- "您想花费1金前往婵娟池与意中人一同赏月吗？"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
				msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
				msg.AddMsgEvent(MsgID, 1, 22)   --“取消”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 499) -- "婚姻系统即将开放，敬请期待。"
				msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
				msg.AddMsgEvent(MsgID, 21, 6) -- 确定按钮
				msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			end
        elseif MapID == 3017298895 then
		    local MsgID = msg.BeginMsgEvent()
            msg.AddMsgEvent(MsgID, 20, 502) -- "您想离开婵娟池返回凤翔吗？"
            msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
			msg.AddMsgEvent(MsgID, 21, 5) -- 确定按钮
            msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
            msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
            msg.AddMsgEvent(MsgID, 1, 22)   --“取消”
            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
        end
    elseif TalkIndex == 4 then
        local money = role.GetRoleSilver(MapID, InstanceID, RoleID)
        if money < 10000 then
            local MsgID = msg.BeginMsgEvent()
            msg.AddMsgEvent(MsgID, 20, 503) -- "您身上的金钱不足，无法前往婵娟池。"
            msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
        else
            role.DecRoleSilver(MapID, InstanceID, RoleID, 10000, 200)-- 系统扣除对应金钱
            role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298895, 745, 58, 831)
        end
    elseif TalkIndex == 5 then
        role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298383, 771, 10034, 2078)      --传送凤翔
    end
end

aux.RegisterCreatureEvent(4900201, 7, "n4900201_OnTalk")

--与吴刚4900203对话，可以每天获得一个月妖之影1361101。
function n4900203_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 504) -- "婵娟池中有许多隐藏得很好的月妖，你可以从它们身上获得提升友好度的道具。男性角色每天只能在我这领取一个月妖之影，使用它可以发现月妖的方位，配合从嫦娥那领取的月妖之镜，才能将隐藏在婵娟池的月妖给找出来。你想现在就领取一个月妖之影吗？"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
		msg.AddMsgEvent(MsgID, 21, 5)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 22)   --“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        local FreeSize = role.GetBagFreeSize(RoleID)
		if(FreeSize < 1) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 505) -- "您的行囊空间不足，无法领取该物品。"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
        else                   --时间判断，每天领取一次
			local LastTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["YueYaoZhiYing_Date"])
            local CurTime = tonumber(os.date("%j"))
			if CurTime == LastTime then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 506) -- "您今天已经领取过月妖之影了，请明天再来领取。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			elseif CurTime ~= LastTime then
				local sex = role.GetRoleSex(RoleID)
				if sex == 1 then
				    local i = role.GetRoleItemNum(RoleID, 1361101)
					if i > 0 then
					    role.RemoveFromRole(MapID, InstanceID, RoleID, 1361101, i, 420)
					end
					role.AddRoleItem(MapID, InstanceID, RoleID, 1361101, 1, -1, 8, 420)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["YueYaoZhiYing_Date"], CurTime)
				elseif sex == 0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 507) -- "只有男性角色才能在我这里领取月妖之影."
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
            end
        end
    end
end

aux.RegisterCreatureEvent(4900203, 7, "n4900203_OnTalk")

--与嫦娥4900202对话，可以每天获得一个月妖之镜1361102。
function n4900202_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
        local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 508) -- "婵娟池中有许多隐藏得很好的月妖，你可以从它们身上获得提升友好度的道具。女性角色每天只能在我这领取一个月妖之镜。月妖之镜可以在隐藏有月妖的地方使用，使月妖现身。你想现在就领取一个月妖之镜吗？"
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
		msg.AddMsgEvent(MsgID, 21, 5)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 22)   --“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        local FreeSize = role.GetBagFreeSize(RoleID)
		if(FreeSize < 1) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 505) -- "您的行囊空间不足，无法领取该物品。"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
        else                   --时间判断，每天领取一次
			local LastTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["YueYaoZhiJing_Date"])
            local CurTime = tonumber(os.date("%j"))
			if CurTime == LastTime then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 509) -- "您今天已经领取过月妖之镜了，请明天再来领取。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			elseif CurTime ~= LastTime then
				local sex = role.GetRoleSex(RoleID)
				if sex == 0 then
				    local i = role.GetRoleItemNum(RoleID, 1361102)
					if i > 0 then
					    role.RemoveFromRole(MapID, InstanceID, RoleID, 1361102, i, 420)
					end
					role.AddRoleItem(MapID, InstanceID, RoleID, 1361102, 1, -1, 8, 420)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["YueYaoZhiJing_Date"], CurTime)
				elseif sex == 1 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 510) -- "只有女性角色才能在我这里领取月妖之镜."
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				end
			end
        end
    end
end

aux.RegisterCreatureEvent(4900202, 7, "n4900202_OnTalk")
