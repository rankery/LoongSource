
function baizhan_addexp(MapID, InstanceID,RoleID,Wave)--完成一波之后给予奖励
    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= 4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then
				local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
				if Target_MapID == MapID then
					local rolelevel = role.GetRoleLevel(MapID, InstanceID, Role[i])
					if rolelevel >110 then
						rolelevel = 110
					end
					if Wave == 10 then
					    local exp = math.floor(activity_newexp[rolelevel]/39*1.95)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 9 then
					    local exp = math.floor(activity_newexp[rolelevel]/39*1.2)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 8 then
						local exp = math.floor(activity_newexp[rolelevel]/39*0.9)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 7 then
						local exp = math.floor(activity_newexp[rolelevel]/39*0.7)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 6 then
						local exp = math.floor(activity_newexp[rolelevel]/48*0.75)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 5 then
						local exp = math.floor(activity_newexp[rolelevel]/48*0.65)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 4 then
						local exp = math.floor(activity_newexp[rolelevel]/48*0.55)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 3 then
						local exp = math.floor(activity_newexp[rolelevel]/48*0.4)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 2 then
						local exp = math.floor(activity_newexp[rolelevel]/60*0.4)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					elseif Wave == 1 then
						local exp = math.floor(activity_newexp[rolelevel]/180)
						role.AddRoleExp(MapID, InstanceID, Role[i], exp)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
						msg.AddMsgEvent(MsgID, 9, Wave)
						msg.AddMsgEvent(MsgID, 9, exp)
						msg.DispatchRoleMsgEvent(Role[i], MsgID)
					end
				end
			end
		end
	else
	    local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if rolelevel >100 then
			rolelevel = 100
		end
		if Wave == 10 then
			local exp = math.floor(activity_newexp[rolelevel]/39*1.95)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 9 then
			local exp = math.floor(activity_newexp[rolelevel]/39*1.2)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 8 then
			local exp = math.floor(activity_newexp[rolelevel]/39*0.9)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
		    msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 7 then
			local exp = math.floor(activity_newexp[rolelevel]/39*0.7)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 6 then
			local exp = math.floor(activity_newexp[rolelevel]/48*0.75)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 5 then
			local exp = math.floor(activity_newexp[rolelevel]/48*0.65)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 4 then
			local exp = math.floor(activity_newexp[rolelevel]/48*0.55)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 3 then
			local exp = math.floor(activity_newexp[rolelevel]/48*0.4)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 2 then
			local exp = math.floor(activity_newexp[rolelevel]/60*0.4)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif Wave == 1 then
			local exp = math.floor(activity_newexp[rolelevel]/180)
			role.AddRoleExp(MapID, InstanceID, RoleID, exp)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 2772)  --百斩修罗第X关通过！奖励经验XX点！
			msg.AddMsgEvent(MsgID, 9, Wave)
			msg.AddMsgEvent(MsgID, 9, exp)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end
end

function baizhan_CreatTenCreature(MapID, InstanceID, Average_Level, Wave)--刷出一波新怪
    local num = 0
	if Average_Level >= 95 then
	    num = 11
    elseif Average_Level >= 90 and Average_Level <= 94 then
	    num = 10
	elseif Average_Level >= 85 and Average_Level <= 89 then
	    num = 9
	elseif Average_Level >= 80 and Average_Level <= 84 then
	    num = 8
	elseif Average_Level >= 75 and Average_Level <= 79 then
	    num = 7
	elseif Average_Level >= 70 and Average_Level <= 74 then
	    num = 6
	elseif Average_Level >= 65 and Average_Level <= 69 then
	    num = 5
	elseif Average_Level >= 60 and Average_Level <= 64 then
	    num = 4
	elseif Average_Level >= 55 and Average_Level <= 59 then
	    num = 3
	elseif Average_Level >= 50 and Average_Level <= 54 then
	    num = 2
	elseif Average_Level >= 45 and Average_Level <= 49 then
	    num = 1
	elseif Average_Level >= 40 and Average_Level <= 44 then
	    num = 0
	end
	if Wave == 1 then
		local xiaoguaiid = 1535000 + num*4
		local bossid = 1535048 + num*8
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		for i=3,12 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 11)
	elseif Wave == 2 then
	    local xiaoguaiid = 1535000 + num*4
		local bossid = 1535048 + num*8 + 1
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		for i=3,17 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 16)
	elseif Wave == 3 then
	    local xiaoguaiid = 1535000 + num*4 +1
		local bossid = 1535048 + num*8 + 2
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 4 then
	    local xiaoguaiid = 1535000 + num*4 +1
		local bossid = 1535048 + num*8 + 3
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 5 then
	    local xiaoguaiid = 1535000 + num*4 +2
		local bossid = 1535048 + num*8 + 4
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429501, bossid)
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 6 then
	    local xiaoguaiid = 1535000 + num*4 +2
		local bossid = 1535048 + num*8 + 5
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429501, bossid)
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 7 then
	    local xiaoguaiid = 1535000 + num*4 +3
		local bossid = 1535048 + num*8 + 6
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429501, bossid)
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 8 then
	    local xiaoguaiid = 1535000 + num*4 +3
		local bossid = 1535048 + num*8 + 7
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429501, bossid)
		for i=3,22 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 21)
	elseif Wave == 9 then
	    local bossid = 1535144 + num
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429501, bossid)
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 1)
	elseif Wave == 10 then
	    local xiaoguaiid = 1535000 + num*4 +3
		local bossid = 1535156 + num
	    map.MapCreateColCreature(MapID, InstanceID, bossid, BaiZhanXiuLuo_Point[2].x, BaiZhanXiuLuo_Point[2].y, BaiZhanXiuLuo_Point[2].z, 1, "")
		--unit.AddBuff(MapID, InstanceID, bossid, 9429901, bossid)
		for i=3,52 do
		    map.MapCreateColCreature(MapID, InstanceID, xiaoguaiid, BaiZhanXiuLuo_Point[i].x, BaiZhanXiuLuo_Point[i].y, BaiZhanXiuLuo_Point[i].z, 1, "")
		end
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 51)
    end
end
