
Quest_WaBao_Pos = {}
Quest_WaBao_Pos[1] = {x=2074, z=1661}
Quest_WaBao_Pos[2] = {x=2013, z=2039}
Quest_WaBao_Pos[3] = {x=2330, z=1845}
Quest_WaBao_Pos[4] = {x=1748, z=3141}
Quest_WaBao_Pos[5] = {x=1509, z=3184}
Quest_WaBao_Pos[6] = {x=1349, z=3041}
Quest_WaBao_Pos[7] = {x=1322, z=2990}
Quest_WaBao_Pos[8] = {x=1744, z=2635}
Quest_WaBao_Pos[9] = {x=1554, z=2149}
Quest_WaBao_Pos[10] = {x=1323, z=2160}
Quest_WaBao_Pos[11] = {x=1828, z=1479}
Quest_WaBao_Pos[12] = {x=1026, z=1084}
Quest_WaBao_Pos[13] = {x=1092, z=1008}
Quest_WaBao_Pos[14] = {x=2489, z=948}
Quest_WaBao_Pos[15] = {x=2713, z=775}
Quest_WaBao_Pos[16] = {x=2637, z=996}
Quest_WaBao_Pos[17] = {x=2272, z=1173}
Quest_WaBao_Pos[18] = {x=3052, z=1472}
Quest_WaBao_Pos[19] = {x=2970, z=1413}
Quest_WaBao_Pos[20] = {x=2411, z=1930}

--与天地龙脉传送人对话
function n3011065_OnTalk(MapID, InstanceID, NPCID, NPCTypeID, TargetID, Index)
	--检验玩家是否是合法的（只有使用贪狼图的玩家及其队友是合法的)
	local TriggerRoleID = cre.GetCreatureScriptData(MapID, InstanceID, NPCID, 1, 1)
	--Flag 0:该对话玩家不是合法的 1：合法玩家，并且就是使用宝图者 2：合法玩家，是使用宝图者的队友
	local Flag = 0
	if TargetID == TriggerRoleID then
		Flag = 1
	else
	  local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, TargetID)
	  local TeamMemID = {}
		TeamMemID[1], TeamMemID[2], TeamMemID[3], TeamMemID[4], TeamMemID[5], TeamMemID[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
	    if TeamMemID[i] == TriggerRoleID then
	    	Flag = 2
	    	break
	    end
    end
end



	if Flag == 0 then
		return
	end

	if Flag == 1 then

		if Index == -1 then
		  --对话后提示：“您是否确定前往天地龙脉？（天地龙脉副本难度较高，建议您组织六人队伍，一起挑战！）——确定/取消”。
		  local MsgID = msg.BeginMsgEvent()
		  msg.AddMsgEvent(MsgID, 20, 57)	--“您是否确定前往天地龙脉？”
			msg.AddMsgEvent(MsgID, 24,	NPCID)	--npcid
		  msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	21)				--“确定”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)				--“取消”
			msg.DispatchRoleMsgEvent(TargetID, MsgID)
		end
		if Index == 4 then
			local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, TargetID)
			local Man_InTeam = 0
			local TeamMemID = {}
				   TeamMemID[1], TeamMemID[2], TeamMemID[3], TeamMemID[4], TeamMemID[5], TeamMemID[6] = role.GetRoleTeamMemID(TeamID)
				for i=1, 6 do
					if  TeamMemID[i] and TeamMemID[i]~= 4294967295 then
						Man_InTeam = Man_InTeam + 1
					end
				end
				--限制进入天地龙脉副本，队伍人数必须达到3人
			if Man_InTeam < 3  then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 71, 414)--提示队伍人数不足3人
				msg.DispatchRoleMsgEvent(TargetID,MsgID)
				return -1
			else
				role.SetRoleScriptData(TriggerRoleID, 1, RoleDataType["LongMaiXunBao_Do"], 1)
				--发提示给其队友
				--if TeamID == 4294967295 then
					--玩家进入天地龙脉副本（2092184116）=17300  Y=225  Z=9250
					--role.RoleGotoNewMap(MapID, InstanceID, TargetID, 2092184116, 346, 20, 185)
					--return
				--else
				local TeamMemID = {}
					TeamMemID[1], TeamMemID[2], TeamMemID[3], TeamMemID[4], TeamMemID[5], TeamMemID[6] = role.GetRoleTeamMemID(TeamID)
					for i=1, 6 do
						if TeamMemID[i] ~= 4294967295 and TeamMemID[i] ~= TriggerRoleID then
							local NPCx,NPCy,NPCz = role.GetRoleScriptData(TriggerRoleID, 3, RoleDataType["LongMaiXunBao_NPC_X"], RoleDataType["LongMaiXunBao_NPC_Y"], RoleDataType["LongMaiXunBao_NPC_Z"])
							  --“您的队友已前往天地龙脉寻宝，请前往[地图名##，坐标##]寻找天地龙脉传送人，与他共同参加挑战！”
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20, 65)
							msg.AddMsgEvent(MsgID, 6, MapID)
							msg.AddMsgEvent(MsgID, 9, NPCx)
							msg.AddMsgEvent(MsgID, 9, NPCz)
							msg.AddMsgEvent(MsgID, 21, 4)			--确定按钮
							msg.AddMsgEvent(MsgID, 1, 21)			--“确定”
							msg.DispatchRoleMsgEvent(TeamMemID[i], MsgID)
						end
				    end
			    end
			    --玩家进入天地龙脉副本（2092184116）=17300  Y=225  Z=9250
			    role.RoleGotoNewMap(MapID, InstanceID, TargetID, 2092184116, 346, 20, 185)
				map.MapDeleteCreature(MapID, InstanceID, NPCID)
			end
		end
		return
	end

	if Flag == 2 then
		--判断使用宝图的玩家是否同意进入副本
		local Agree = role.GetRoleScriptData(TriggerRoleID, 1, RoleDataType["LongMaiXunBao_Do"])
		if Agree == 1 then
		else
			return
		end
		--玩家与其对话后提示：“您是否确定前往天地龙脉？——确定/取消”。
		if Index == -1 then
		  local MsgID = msg.BeginMsgEvent()
		  msg.AddMsgEvent(MsgID, 20, 	57)			--“您是否确定前往天地龙脉？”
		  msg.AddMsgEvent(MsgID, 24,	NPCID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	21)				--“确定”
			msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)				--“取消”
			msg.DispatchRoleMsgEvent(TargetID, MsgID)
		end
		if Index == 4 then
			--玩家进入天地龙脉副本（2092184116）
			role.RoleGotoNewMap(MapID, InstanceID, TargetID, 2092184116, 346, 20, 185)
		end
	end



--离开天地龙脉副本的脚本
function a02_LeaveMap(MapID, InstanceID, TargetID)
	local bool = role.GetRoleScriptData(TargetID, 1, RoleDataType["LongMaiXunBao_Do"])
	if bool == 1 then
		role.SetRoleScriptData(TargetID, 1, RoleDataType["LongMaiXunBao_Do"], 0)
	end
end

-- 注册
aux.RegisterCreatureEvent(3011065, 7, "n3011065_OnTalk")
aux.RegisterMapEvent("a02", 3, "a02_LeaveMap")
