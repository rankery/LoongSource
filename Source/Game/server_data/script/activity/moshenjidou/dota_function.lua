
function SendNormalInformation(MapID, InstanceID, RoleID)
   local Target_MapID, Target_InstanceID = role.GetRoleMapID(k)
   local char = tostring(1200-os.difftime(os.time(),Jidou.RoomInfo[InstanceID].BeginTime))..","..tostring(Jidou.RoomInfo[InstanceID].Role[RoleID].GetFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[RoleID].MissFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[RoleID].Kill)..","..tostring(Jidou.RoomInfo[InstanceID].Role[RoleID].Die)
   role.DotaSendMsg(Target_MapID, Target_InstanceID,RoleID,4,char)
end

function MSJD_SendAllNormalInformation(MapID, InstanceID)
	for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
	    local Target_MapID, Target_InstanceID = role.GetRoleMapID(k)
		if Target_MapID ~= nil and Target_MapID == 2108961076 then
		    local char = tostring(1200-os.difftime(os.time(),Jidou.RoomInfo[InstanceID].BeginTime))..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].GetFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].MissFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].Kill)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].Die)
			role.DotaSendMsg(Target_MapID, Target_InstanceID, k, 4, char)
		end
	end
end

function MSJD_SendAllFinalInformation(MapID, InstanceID)
	local table1 = {}
    for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
	    table1 = {}
		local Target_MapID, Target_InstanceID = role.GetRoleMapID(k)
		if Target_MapID ~= nil and Target_MapID == 2108961076 and Target_InstanceID == InstanceID then
			for k1,v1 in pairs(Jidou.RoomInfo[InstanceID].Role) do

				local Name = Jidou.RoomInfo[InstanceID].Role[k1].Name
				local A = Jidou.RoomInfo[InstanceID].Role[k1].GetFlag
				local B = Jidou.RoomInfo[InstanceID].Role[k1].MissFlag
				local C = Jidou.RoomInfo[InstanceID].Role[k1].Kill
				local D = Jidou.RoomInfo[InstanceID].Role[k1].Die
				local T = math.floor(Jidou.RoomInfo[InstanceID].Timer30/2)
				local Point,B1,D1 = 0,0,0
				local tbl = {Name,A,B,C,D}
				if B == 0 then
				    B1 = 1
				end
				if D == 0 then
				    D1 = 1
				end
				Point = 5*A/B1 + 2*C/D1 + math.floor(T/3)
				if V1.leave == 1 then
				    Point = 0
				end
                tbl[6] = Point
				if k1 == k then
					table.insert(table1,tbl,1)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["MSJD_Award"], role.GetRoleScriptData(RoleID, 1, RoleDataType["MSJD_Award"])+T)
					role.SetRoleScriptData(RoleID, 1, RoleDataType["MSJD_TotlePoint"], role.GetRoleScriptData(RoleID, 1, RoleDataType["MSJD_TotlePoint"])+T)
				elseif Jidou.RoomInfo[InstanceID].Role[k].Side == Jidou.RoomInfo[InstanceID].Role[k1].Side then
					if #table1 == 0 then
						table.insert(table1,tbl,1)
					else
						table.insert(table1,tbl,2)
					end
				else
					table.insert(table1,tbl)
				end

			end
			local char = ""
			for i = 1,8 do
				char = char..table1[i][1]
				for j = 2,6 do
				   char = char..table1[i][j]
				end
			end

			role.DotaSendMsg(Target_MapID, Target_InstanceID, k, 5, char)
		end
	end
end

--[[function MSJD_SendAllNormalInformation(MapID, InstanceID)
	for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
	    local Target_MapID, Target_InstanceID = role.GetRoleMapID(k)
		if Target_MapID ~= nil and Target_MapID == 2108961076 then
		    local char = tostring(1200-os.difftime(os.time(),Jidou.RoomInfo[InstanceID].BeginTime))..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].GetFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].MissFlag)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].Kill)..","..tostring(Jidou.RoomInfo[InstanceID].Role[k].Die)
			role.DotaSendMsg(Target_MapID, Target_InstanceID, k, 4, char)
		end
	end
end]]

function MSJD_CheckIsOver(MapID, InstanceID)
    if Jidou.RoomInfo[InstanceID].IsOver ~= 1 then
		local Red = 0
		local Blue = 0
		local RedGetFlag = 0
		local BlueGetFlag = 0
		for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
			if v.Side == "Red" and v.Leave == 0 then
				Red = Red + 1
				if v.GetFlag > RedGetFlag then
					RedGetFlag = v.GetFlag
				end
			else
				Blue = Blue + 1
				if v.GetFlag > BlueGetFlag then
					BlueGetFlag = v.GetFlag
				end
			end
		end
        if Red <= 2 or Blue <= 2 then
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 12933)			--由于参与比赛的玩家过少，比赛提前结束。
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end
		if Red <= 2 or BlueGetFlag >= 3 or (Jidou.RoomInfo[InstanceID].Timer30 == 40 and RedGetFlag < BlueGetFlag) then
		    Jidou.RoomInfo[InstanceID].IsOver = 1
			for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
			    if v.Side == "Red" and v.Leave == 0 then
					role.DotaSendMsg(MapID,InstanceID,RoleID,6,"0")--新加接口，用来弹出图片，0是输，1是赢，2是平局
					unit.AddBuff(MapID, InstanceID, k, 1141001, k) --加一个可添加死亡目标的30秒无敌BUFF。需要新提需求
				elseif v.Side == "Blue" and v.Leave == 0 then
				    role.DotaSendMsg(MapID,InstanceID,RoleID,6,"1")
					unit.AddBuff(MapID, InstanceID, k, 1141001, k) --加一个可添加死亡目标的30秒无敌BUFF。需要新提需求
				end
			end
			MSJD_SendAllNormalInformation(MapID, InstanceID)
			MSJD_SendAllFinalInformation(MapID, InstanceID)
			local ID = map.MapCreateCreature(2108961076, InstanceID, 4902134, 1, 1, 1)--刷新一个30秒的透明计时器，该怪物未提需求.
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 40)
			Jidou.RoomInfo[InstanceID].Timer30 = 40
        elseif Blue <= 2 or RedGetFlag >= 3 or (Jidou.RoomInfo[InstanceID].Timer30 == 40 and RedGetFlag > BlueGetFlag) then
            Jidou.RoomInfo[InstanceID].IsOver = 1
			for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
			    if v.Side == "Blue" and v.Leave == 0 then
					role.DotaSendMsg(MapID,InstanceID,RoleID,6,"0")             --新加接口，用来弹出图片，0是输，1是赢，2是平局
					unit.AddBuff(MapID, InstanceID, k, 1141001, k) --加一个可添加死亡目标的30秒无敌BUFF。需要新提需求
				elseif v.Side == "Red" and v.Leave == 0 then
				    role.DotaSendMsg(MapID,InstanceID,RoleID,6,"1")
					unit.AddBuff(MapID, InstanceID, k, 1141001, k) --加一个可添加死亡目标的30秒无敌BUFF。需要新提需求
				end
			end
			MSJD_SendAllNormalInformation(MapID, InstanceID)
			MSJD_SendAllFinalInformation(MapID, InstanceID)
            local ID = map.MapCreateCreature(2108961076, InstanceID, 4902134, 1, 1, 1)--刷新一个30秒的透明计时器，该怪物未提需求.
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 40)
			Jidou.RoomInfo[InstanceID].Timer30 = 40
		elseif Jidou.RoomInfo[InstanceID].Timer30 == 40 and RedGetFlag == BlueGetFlag then
            Jidou.RoomInfo[InstanceID].IsOver = 1
			for k,v in pairs(Jidou.RoomInfo[InstanceID].Role) do
			    if v.Leave == 0 then
					role.DotaSendMsg(MapID,InstanceID,RoleID,6,"2")             --新加接口，用来弹出图片，0是输，1是赢，2是平局
					unit.AddBuff(MapID, InstanceID, k, 1141001, k) --加一个可添加死亡目标的30秒无敌BUFF。需要新提需求
				end
			end
			MSJD_SendAllNormalInformation(MapID, InstanceID)
			MSJD_SendAllFinalInformation(MapID, InstanceID)
            local ID = map.MapCreateCreature(2108961076, InstanceID, 4902134, 1, 1, 1)--刷新一个30秒的透明计时器，该怪物未提需求.
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 40)
			Jidou.RoomInfo[InstanceID].Timer30 = 40
        end
	end
end
