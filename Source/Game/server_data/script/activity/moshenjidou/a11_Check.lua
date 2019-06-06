

function s9001901_CanCast(MapID, InstanceID, SkillID, RoleID, TargetID)
	if Jidou.RoomInfo[InstanceID].IsOver ~= 1 then
		local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, TargetID)
		if TypeID == 4902112 then --蓝
			if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Red" then
				if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 999 then
					return 14
				else
					return 0
				end
			else
				if Jidou.RoomInfo[InstanceID].BlueFlag.State == "outside" then
				   return 0
				else
				   return 14
				end
			end
		elseif TypeID == 4902113 then --红
			if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Blue" then
				if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 999 then
					return 14
				else
					return 0
				end
			else
				if Jidou.RoomInfo[InstanceID].RedFlag.State == "outside" then
				   return 0
				else
				   return 14
				end
			end
		elseif TypeID == 4902114 then --蓝方空旗杆
			if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Blue" and role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11402) then --蓝方带红旗
				if Jidou.RoomInfo[InstanceID].BlueFlag.State == "outside" then

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 12921)   --敌方旗帜必须放在己方旗帜旁边
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
					return 999
				else
					return 0
				end
			else
			   return 14
			end
		elseif TypeID == 4902133 then --红方空旗杆 需要新ID
			if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Red" and role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 11401) then
				if Jidou.RoomInfo[InstanceID].RedFlag.State == "outside" then

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 12921)   --敌方旗帜必须放在己方旗帜旁边
					msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
					return 999
				else
					return 0
				end
			else
			   return 14
			end
		end
	end
    return 14
end

--注册
aux.RegisterSkillEvent(9001901, 0, "s9001901_CanCast")


function BlueFlag_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Blue" then
		Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902112,97,5556,406)--蓝方蓝旗
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12922)			--蓝方XXX从红方手中夺回了旗帜
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Jidou.RoomInfo[InstanceID].BlueFlag.State = "home"
		Jidou.RoomInfo[InstanceID].BlueFlag.Role = 0
		Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
		if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)  == 999 then     --在摆在别人家的空旗杆处的有色旗帜要在第一个位置添加999,要刷空旗杆
		    map.MapCreateCreature(MapID, InstanceID, 4902113,425,5483,109)
		end

	else
	    unit.AddBuff(MapID, InstanceID, RoleID, 1140101, RoleID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12923)			--红方XXX扛起了红方的旗帜
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Jidou.RoomInfo[InstanceID].BlueFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].BlueFlag.Role = RoleID
		Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
	end

end

aux.RegisterCreatureEvent(4902112, 4, "BlueFlag_OnDie")

function RedFlag_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	if Jidou.RoomInfo[InstanceID].Role[RoleID].Side == "Red" then
		Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113,425,5483,109)--红方红旗
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12924)			--红方XXX从蓝方手中夺回了旗帜
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Jidou.RoomInfo[InstanceID].RedFlag.State = "home"
		Jidou.RoomInfo[InstanceID].RedFlag.Role = 0
		Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
		if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)  == 999 then     --在摆在别人家的空旗杆处的有色旗帜要在第一个位置添加999,要刷空旗杆
		    map.MapCreateCreature(MapID, InstanceID, 4902112,64,5555,440)
		end
	else
	    unit.AddBuff(MapID, InstanceID, RoleID, 1140201, RoleID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12925)			--蓝方XXX扛起了红方的旗帜
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Jidou.RoomInfo[InstanceID].RedFlag.State = "outside"
		Jidou.RoomInfo[InstanceID].RedFlag.Role = RoleID
		Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
	end

end

aux.RegisterCreatureEvent(4902113, 4, "RedFlag_OnDie")

function hongFangKong_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	Jidou.RoomInfo[InstanceID].BlueFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902112, x, y, z)
	unit.CancelBuff(MapID, InstanceID, RoleID, 1140101)
	cre.SetCreatureScriptData(MapID, InstanceID, Jidou.RoomInfo[InstanceID].BlueFlag.FlagID, 1, 0, 999)       --记录这个是夺来的旗帜
	local ID = map.MapCreateCreature(MapID, InstanceID, 4902134, 1, 1, 1)                                       --计时器,30秒后如果旗帜还在,则归位，和之前的计时器用同一个ID
	cre.GetCreatureScriptData(MapID, InstanceID, ID, 1, 0, Jidou.RoomInfo[InstanceID].BlueFlag.FlagID)       --记录归位的计时器
	Jidou.RoomInfo[InstanceID].BlueFlag.Role = RoleID
	Jidou.RoomInfo[InstanceID].BlueFlag.Time = 0
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12926)			--红方的XXX夺取了一面蓝方的旗帜！
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
	    if v.Leave == 0 then
            if  v.Side == "Red" then
			    v.GetFlag = v.GetFlag + 1
			else
			    v.MissFlag = v.MissFlag + 1
			end
		end
	end
	MSJD_SendAllNormalInformation(MapID, InstanceID)
    MSJD_CheckIsOver(MapID, InstanceID)

end

aux.RegisterCreatureEvent(4902133, 4, "hongFangKong_OnDie")

function LanFangKong_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	Jidou.RoomInfo[InstanceID].RedFlag.FlagID = map.MapCreateCreature(MapID, InstanceID, 4902113, x, y, z)
	unit.CancelBuff(MapID, InstanceID, RoleID, 1140201)
	cre.SetCreatureScriptData(MapID, InstanceID, Jidou.RoomInfo[InstanceID].RedFlag.FlagID, 1, 0, 999)                     --记录这个是夺来的旗帜
	local ID = map.MapCreateCreature(MapID, InstanceID, 4902134, 1, 1, 1)                                       --计时器,30秒后如果旗帜还在,则归位，和之前的计时器用同一个ID
	cre.GetCreatureScriptData(MapID, InstanceID, ID, 1, 0, Jidou.RoomInfo[InstanceID].RedFlag.FlagID)       --记录归位的计时器
	Jidou.RoomInfo[InstanceID].RedFlag.Role = RoleID
	Jidou.RoomInfo[InstanceID].RedFlag.Time = 0
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101,12927 )			--蓝方的XXX夺取了一面红方的旗帜！
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
	    if v.Leave == 0 then
            if  v.Side == "Blue" then
			    v.GetFlag = v.GetFlag + 1
			else
			    v.MissFlag = v.MissFlag + 1
			end
		end
	end
	MSJD_SendAllNormalInformation(MapID, InstanceID)
    MSJD_CheckIsOver(MapID, InstanceID)

end

aux.RegisterCreatureEvent(4902114, 4, "LanFangKong_OnDie")
