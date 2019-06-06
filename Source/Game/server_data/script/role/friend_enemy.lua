-- test02的敌我关系判断 -- 默认为中立  返回值分别为 友好，对立，中立，忽略自身判断检查
function test02_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	-- 都是玩家
	if unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then

		-- 队友
		local teamid = role.IsRoleHaveTeam(MapID, InstanceID, SrcID)
		if teamid ~= 4294967295 and teamid == role.IsRoleHaveTeam(MapID, InstanceID, TargetID) then
			return true, false, false, false
		end

		-- 帮派
		local guildid = guild.GetRoleGuildID(SrcID)
		if guildid ~= 4294967295 and guildid == guild.GetRoleGuildID(TargetID) then
			return true, false, false, false
		end

		-- 夫妻
		if role.IsCouple(SrcID, TargetID) then
			return true, false, false, false
		end

	end

	return false, false, false, false
end

aux.RegisterMapEvent("test02", 16, "test02_FriendEnemy")

--------------------------------------------------------------------------------------
-- a04的敌我关系判断 -- 除队友外其他全为敌方
function a04_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	-- 都是玩家
	if unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then

		-- 队友
		local teamid = role.IsRoleHaveTeam(MapID, InstanceID, SrcID)
		if teamid ~= 4294967295 and teamid == role.IsRoleHaveTeam(MapID, InstanceID, TargetID) then
			return true, false, false, true
		else
			if role.IsRoleInStatus(SrcID, 1) or role.IsRoleInStatus(TargetID, 1) then			-- 有一方pk保护
				return false, false, false, true

			elseif role.IsRoleInStatus(SrcID, 4) then		-- 玩家开红才能攻击
				return false, true, false, true

			elseif role.IsRoleInStatus(SrcID, 1048576) and ( role.IsRoleInStatus(TargetID, 4) or role.IsRoleInStatus(TargetID, 1048576) )then
				return false, true, false, true

			else
				return false, false, false, true
			end

		end

	end

	return false, false, false, false
end

aux.RegisterMapEvent("a04", 16, "a04_FriendEnemy")

---------------------------------------------------------------------------------------

-- a05的敌我关系判断 -- 除队友外其他全为敌方
function a05_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	-- 都是玩家
	if unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then

		-- 队友
		local teamid = role.IsRoleHaveTeam(MapID, InstanceID, SrcID)
		if teamid ~= 4294967295 and teamid == role.IsRoleHaveTeam(MapID, InstanceID, TargetID) then
			return true, false, false, true
		else
			if role.IsRoleInStatus(SrcID, 1) or role.IsRoleInStatus(TargetID, 1) then			-- 有一方pk保护
				return false, false, false, true

			elseif role.IsRoleInStatus(SrcID, 4) then		-- 玩家开红才能攻击
				return false, true, false, true

			elseif role.IsRoleInStatus(SrcID, 1048576) and ( role.IsRoleInStatus(TargetID, 4) or role.IsRoleInStatus(TargetID, 1048576) )then
				return false, true, false, true

			else
				return false, false, false, true
			end

		end

	end

	return false, false, false, false
end

aux.RegisterMapEvent("a05", 16, "a05_FriendEnemy")

------------------------------------------------------------------------------------

-- a06的敌我关系判断 -- 除同色以外全是敌人
function a06_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
    local a,b = -1,-1
	-- 都是玩家
	if unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then
        for i=1, 6 do
		    if Totle_Arena[InstanceID][i] == SrcID then
			    if i <= 3 then
			        a = 1
				    break
				else
				    a = 0
					break
				end
			end
		end

		for i=1, 6 do
		    if Totle_Arena[InstanceID][i] == TargetID then
			    if i <= 3 then
			        b = 1
				    break
				else
				    b = 0
					break
				end
			end
		end

		if a ~= b and a~= -1 and b ~= -1 then
			return false, true, false, true
		elseif a == b and a~= -1 then
		    return true, false, false, true
		end
	end

	return false, false, false, false
end

aux.RegisterMapEvent("a06", 16, "a06_FriendEnemy")

---------------------------------------------------------------------------------------


-- brief: 辅助函数
-- remark: 神迹各地图 敌我判断
function GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)

	--特殊boss判断（必须开红）
	if unit.IsPlayer(SrcID) and unit.IsCreature(TargetID) then
		local TargetTypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
		local temp=0
		local boss_id_list={}
		boss_id_list[1] = 1535326
		boss_id_list[2] = 1535327
		boss_id_list[3] = 1550201
		boss_id_list[4] = 1550202
		boss_id_list[5] = 1550203
		boss_id_list[6] = 1550204

		for i=1,6 do
			if TargetTypeID==boss_id_list[i] then
				temp=1
				break
			end
		end

		if temp==1 then
			if role.IsRoleInStatus(SrcID, 1048576) or role.IsRoleInStatus(SrcID, 4) then
				return false, true, false, true
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26,	3624)		--今日已参加过5次激斗
				msg.DispatchRoleMsgEvent(SrcID, MsgID)
				return false, false, true, true
			end
		else
			return false, false, false, false
		end
	end


-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return false, false, false, false
	end

	--  没在神迹区域
	if  map.IsInArea(MapID, InstanceID, SrcID, GetAreaID(MapID)) == false and   map.IsInArea(MapID, InstanceID, TargetID, GetAreaID(MapID)) == false then
		return false, false, false, false
	else

		if unit.IsCreature(SrcID) == true or unit.IsCreature(TargetID) == true  then
			return false, false, false, false
		end

		-- 师徒关系不可打
		local Src = role.GetMasterID(MapID, InstanceID, SrcID)
		if (src ~= 4294967295) and src ==  TargetID then
			return false, false, false, false
		end

		local Target = role.GetMasterID(MapID, InstanceID, TargetID)
		if (Target ~= 4294967295) and   Target == SrcID then
			return false, false, false, false
		end

		-- 状态0和1是用来处理第一次
		local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
		if godCurState == 2 then

			local SrcGuild 		 = guild.GetRoleGuildID(SrcID)
			local TargetGuildID = guild.GetRoleGuildID(TargetID)

			if SrcGuild == 4294967295 and TargetGuildID == 4294967295 then
					return false, false, false, false
			elseif SrcGuild == 4294967295 and TargetGuildID ~= 4294967295 then
				if GetDefendGuildID(MapID) == TargetGuildID or GetCaptureGuildID(MapID) == TargetGuildID then
					return true, false, false, true
				end
			elseif SrcGuild ~= 4294967295 and TargetGuildID == 4294967295 then
				if GetDefendGuildID(MapID) == SrcGuild or GetCaptureGuildID(MapID) == SrcGuild then
					return true, false, false, true
				end
			elseif	SrcGuild ~= 4294967295 and TargetGuildID ~= 4294967295 then
				if SrcGuild == TargetGuildID then
					return true, false, false, true
				end
				if (GetDefendGuildID(MapID) == TargetGuildID or GetCaptureGuildID(MapID) == TargetGuildID) and (GetDefendGuildID(MapID) == SrcGuild or GetCaptureGuildID(MapID) == SrcGuild) then
					return false, true, false, true
				else
					return true, false, false, true
				end
			end
		else
			-- 不再争夺时间内，走一般判断
			return false, false, false, false
		end
	end

end

---------------------------------------------------------------------------------------
-- m03 神迹区域判断
function m03_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	return GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
end

-- m04 神迹区域判断
function m04_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	return GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
end
-- m05 神迹区域判断
function m05_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	return GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
end
-- m06 神迹区域判断
function m06_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	return GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
end
-- m07 神迹区域判断
function m07_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
    --葵灵宝树活动中M07无法PK
    if KuiLingBaoShu_Start == 1 and unit.IsPlayer(SrcID) and unit.IsPlayer(TargetID) then
	    return true, false, false, true
	else
	    return GodMiracle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	end
end
aux.RegisterMapEvent("m03", 16, "m03_FriendEnemy")
aux.RegisterMapEvent("m04", 16, "m04_FriendEnemy")
aux.RegisterMapEvent("m05", 16, "m05_FriendEnemy")
aux.RegisterMapEvent("m06", 16, "m06_FriendEnemy")
aux.RegisterMapEvent("m07", 16, "m07_FriendEnemy")

