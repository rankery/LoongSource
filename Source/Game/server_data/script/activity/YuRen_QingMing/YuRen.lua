YuRenCreature = {}
function YuRen_OnStart(actID)
	YuRenCreature[1] = map.MapCreateColCreature(3017298127, -1, 4900448, 2196, 6646, 2234,1)
end
function YuRen_OnEnd(actID)
	map.MapDeleteCreature(3017298127, -1, YuRenCreature[1])
end

--注册该活动的相关事件
aux.RegisterActEvent(79, 2, "YuRen_OnStart")-----活动ID
aux.RegisterActEvent(79, 3, "YuRen_OnEnd")-----活动ID

--与周不通对话
function ZhouBuTong_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2630)	-- "周不通：我整天呆在这主城之内，看着你们来来往往，自己却不能走动，实在无聊。你帮我去戏弄一下我的几个好友如何?(20级可接此任务)"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
		msg.AddMsgEvent(MsgID, 1, 2631)	-- "接受愚人牌任务"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		if rolelevel >= 20 then
			local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"])
			local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"])
			--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
			local CurTime = tonumber(os.date("%j"))
			if CurTime~=UpdateTime then
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], 0)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Update"], CurTime)
			end
			if TotalNum >= 50 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 147)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				local IsHaveQ1 = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20196)
				local IsHaveQ2 = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20197)
				local IsHaveQ3 = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20198)
				if IsHaveQ1 == true or IsHaveQ2 == true or IsHaveQ3 == true then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 2632)-- "您已经接受过愚人节任务，请先完成。"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 1303)	-- "确定"
					msg.AddMsgEvent(MsgID, 24, TargetID)
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
					local temp = math.random(20196,20198)
					role.AddQuest(RoleID,temp)
				end
			end
		end
		TalkIndex = -1
	end
end

aux.RegisterCreatureEvent(4900448, 7, "ZhouBuTong_OnTalk")

--愚人牌
function I3303601_OnUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local i = math.random(1,20)
	if i >= 10 and i <= 13 then --1/5概率获得物品
		local curmon = tonumber(os.date("%m"))
		local curday = tonumber(os.date("%d"))
		if (curmon == 3 and curday == 31) or (curmon == 4 and (curday ==1 or curday ==2)) then
			local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["YuRen_Update"])
			--检测上次更新日期，若与当前日期不一致，则重置总环数及更新日期
			local CurTime = tonumber(os.date("%j"))
			if CurTime~=UpdateTime then
				role.SetRoleScriptData(RoleID, 1, RoleDataType["YuRen_Total"], 0)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["YuRen_Update"], CurTime)
			end
			local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["YuRen_Total"])
			if TotalNum < 5 then
				local Temp = math.random(100)
				local ID = 0
				if Temp >= 70 then
					ID = 1320022
				else
					ID = 1320011
				end
				role.AddRoleItem(MapID, InstanceID, RoleID, ID, 1, -1, 8, 420)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["YuRen_Total"], TotalNum+1)
			end
		end
	end

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5 + 1)
	if k <= 23 then
        role.AddRoleExp(MapID, InstanceID, RoleID, SanBa_exp[k])
	end
	local Temp1 = math.random(2601,2629)
	local Temp2 = math.random(1,100)
	if Temp2 > 80 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, Temp1) --"XXX对XXX说：搞笑信息。"
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.DispatchWorldMsgEvent(MsgID)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, Temp1) --"XXX对XXX说：搞笑信息。"
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

		local MsgID1 = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID1, 13, 0)
		msg.AddMsgEvent(MsgID1, 1, Temp1) --"XXX对XXX说：搞笑信息。"
		msg.AddMsgEvent(MsgID1, 2, RoleID)
		msg.AddMsgEvent(MsgID1, 2, TargetID)
		msg.DispatchRoleMsgEvent(TargetID, MsgID1)
	end
end

function I3303601_CanUse(MapID, InstanceID, TypeID, RoleID, TargetID)
	local FreeSize1 = role.GetBagFreeSize(RoleID)
	if FreeSize1 < 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 168)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 40, false
	end
end

aux.RegisterItemEvent(3303601, 1, "I3303601_OnUse")
aux.RegisterItemEvent(3303601, 0, "I3303601_CanUse")



function YuRen_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"])
	role.SetRoleScriptData(RoleID, 1, RoleDataType["QingMing_Total"], TotalNum+1)
end

aux.RegisterQuestEvent(20196, 1, "YuRen_OnComplete")
aux.RegisterQuestEvent(20197, 1, "YuRen_OnComplete")
aux.RegisterQuestEvent(20198, 1, "YuRen_OnComplete")
