--NPC传送进入场景todo

--判断玩家是否为死亡后离开活动
local function IsDeadLeave(MapID, InstanceID, RoleID, RoleLevel)
    if Act14_RoleData[RoleID] then
		if RoleLevel < 60 then
			return true, 2000
		elseif RoleLevel < 80 then
			return true, 5000
		elseif RoleLevel < 100 then
			return true, 10000
		else
			return true, 20000
		end
	else
	    return false, 0
	end
end

function C4500101_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	--判断活动是否已经开始
	local bStart = false
	if act.GetActIsStart(14) or act.GetActIsStart(97) or act.GetActIsStart(18) or act.GetActIsStart(103) then
	    bStart = true
	end

	if -1 == TalkIndex then
	    if PVPactivity_trigger == 1 then
			--活动已经开始
			if bStart then
				local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				if RoleLevel < 40 then
					--提示玩家等级不够，无法进入活动
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	157)
					msg.AddMsgEvent(MsgID, 21,	5)			    --取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
					return
				end

				--如果玩家是死亡后离开，提示玩家再次进入需要花费金钱
				local bDeadLeave, Money = IsDeadLeave(MapID, InstanceID, RoleID, RoleLevel)
				if bDeadLeave then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 151)
					msg.AddMsgEvent(MsgID, 9, Money)
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	4)			--确定
					msg.AddMsgEvent(MsgID, 1,	21)
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					--提示玩家是否进入活动场景
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	150)		--您是否要进入英雄乱武活动？
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	4)			--确定
					msg.AddMsgEvent(MsgID, 1,	21)
					msg.AddMsgEvent(MsgID, 21,	5)			--取消
					msg.AddMsgEvent(MsgID, 1,	22)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else --活动还没有开始
				--提示玩家活动没有开始
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 152)			--英雄乱武活动还没有开始。
				msg.AddMsgEvent(MsgID, 21,	5)			    --取消
				msg.AddMsgEvent(MsgID, 1,	22)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		else
		    --提示玩家活动没有开始
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 2862)			--英雄乱舞活动暂不开放，敬请谅解。
			msg.AddMsgEvent(MsgID, 21,	5)			    --取消
			msg.AddMsgEvent(MsgID, 1,	22)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end

	--如果玩家选择确定
	if TalkIndex == 4 then
		--传送玩家到活动场景
		local Index = math.random(1, 8)
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2983744463, m21_BornPos[Index].x, m21_BornPos[Index].y, m21_BornPos[Index].z)
	end
end

--注册玩家进入活动事件
aux.RegisterCreatureEvent(4500101, 7, "C4500101_OnTalk")
