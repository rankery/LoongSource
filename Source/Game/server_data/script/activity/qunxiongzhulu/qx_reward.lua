
--[[判断玩家是否获得活动奖励,返回名次和组别
local function GetRoleReward(RoleExpire, TeamIndex)
    if 0 == TeamIndex then
        return 0, 0
    end

    local Expire18 = act.GetActScriptData(18, 1, 0)
    local Expire19 = act.GetActScriptData(19, 1, 0)
    local Expire20 = act.GetActScriptData(20, 1, 0)
    local Expire21 = act.GetActScriptData(21, 1, 0)

    for k=1, 10 do
        if RoleExpire == Expire18 and TeamIndex == act.GetActScriptData(18, 1, k) then
            return k, 18
        end

        if RoleExpire == Expire19 and TeamIndex == act.GetActScriptData(19, 1, k) then
            return k, 19
        end

        if RoleExpire == Expire20 and TeamIndex == act.GetActScriptData(20, 1, k) then
            return k, 20
        end

        if RoleExpire == Expire21 and TeamIndex == act.GetActScriptData(21, 1, k) then
            return k , 21
        end
    end
    return 0, 0
end

function C4500203_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if -1 == TalkIndex then
        --如果您参加了本次的英雄乱武活动并取得了排名，即可在我这里领取奖励！
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	172)
		msg.AddMsgEvent(MsgID, 24,	TargetID)	    --npcid
		msg.AddMsgEvent(MsgID, 21,	4)			    --确定
		msg.AddMsgEvent(MsgID, 1,	21)
		msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		msg.AddMsgEvent(MsgID, 1,	22)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return
    end

    --如果玩家选择确定
	if 4 == TalkIndex then
	    --判断活动是否已经结束
	    local bStart = act.GetActIsStart(18)
	    if bStart then
	        local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20,	171)
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)
		    return
	    end

	    --判断玩家是否参加过活动并获得了名次
	    local RoleExpire, TeamIndex = role.GetRoleScriptData(RoleID, 2, RoleDataType["Act18_Expire"], RoleDataType["Act18_TeamKey"])
	    local PosIndex, Group = GetRoleReward(RoleExpire, TeamIndex)

	    if 0 == PosIndex then
	        --没有可以领取的奖励。
	        local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 20,	169)
		    msg.AddMsgEvent(MsgID, 21,	5)			    --取消
		    msg.AddMsgEvent(MsgID, 1,	22)
		    msg.DispatchRoleMsgEvent(RoleID, MsgID)
		    return
	    end

	    if 1 == PosIndex then
	        local FreeSize = role.GetBagFreeSize(RoleID)
	        if 2 > FreeSize then
	            --行囊空间不足
	            local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20,	168)
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)
		        return
	        end

	        if 18 == Group then
	            --获得绑定道具2612621
	            role.AddRoleItem(MapID, InstanceID, RoleID, 2612625, 1, 0, 13, 1001)
	        end

	        if 19 == Group then
	            --获得绑定道具2612621
	            role.AddRoleItem(MapID, InstanceID, RoleID, 2612626, 1, 0, 13, 1001)
	        end

	        if 20 == Group then
	            --获得绑定道具2612621
	            role.AddRoleItem(MapID, InstanceID, RoleID, 2612627, 1, 0, 13, 1001)
	        end

	        if 21 == Group then
	            --获得绑定道具2612621
	            role.AddRoleItem(MapID, InstanceID, RoleID, 2612628, 1, 0, 13, 1001)
	        end

	        --奖励8个道具英雄帖2612629
	        role.AddRoleItem(MapID, InstanceID, RoleID, 2612629, 10, 0, 13, 1001)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"], 0)
	    end

	    --各组第2、3名
	    if 2 <= PosIndex and 3 >= PosIndex then
	        local FreeSize = role.GetBagFreeSize(RoleID)
	        if 1 > FreeSize then
	            --行囊空间不足
	            local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20,	168)
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)
		        return
	        end

	        --奖励4个道具英雄帖
	        role.AddRoleItem(MapID, InstanceID, RoleID, 2612629, 8, 0, 13, 1001)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"], 0)
	    end

	    --各组第4-6名
	    if 4 <= PosIndex and 6 >= PosIndex then
	        local FreeSize = role.GetBagFreeSize(RoleID)
	        if 1 > FreeSize then
	            --行囊空间不足
	            local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20,	168)
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)
		        return
	        end

	        --奖励2个道具英雄帖
	        role.AddRoleItem(MapID, InstanceID, RoleID, 2612629, 4, 0, 13, 1001)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"], 0)
	    end

	    --各组第7-10
	    if 7 <= PosIndex and 10 >= PosIndex then
	        local FreeSize = role.GetBagFreeSize(RoleID)
	        if 1 > FreeSize then
	            --行囊空间不足
	            local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 20,	168)
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)
		        return
	        end

            --奖励1个道具英雄帖
	        role.AddRoleItem(MapID, InstanceID, RoleID, 2612629, 2, 0, 13, 1001)
	        role.SetRoleScriptData(RoleID, 1, RoleDataType["Act18_TeamKey"], 0)
	    end

	    --这是您的奖励，请拿好。
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	170)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end

--注册兑换奖励对话事件
aux.RegisterCreatureEvent(4500203, 7, "C4500203_OnTalk")]]
