--固定活动“承安之殿“
--生成副本怪物
function chengan_monid(MapID, InstanceID, RoleID)
	local monid = 0
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local monlevel = 0
	--如果没有小队
	if 4294967295 == TeamID  then
		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local m = math.fmod(rolelevel,5)
		rolelevel = math.floor(rolelevel/5)*5
		if m > 3 then
			rolelevel = rolelevel + 5
		end
		return  rolelevel
	else
		--获得小队玩家ID
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		local maxlevel = 0
		--local maxlevelrole = 0
		local totallevel = 0
		local teamnum = 0
		for k in pairs(Role) do
			if 4294967295 ~= Role[k] then
			    local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
				local rolelevel = role.GetRoleLevel(Role_MapID, Role_InstanceID, Role[k])
				if rolelevel >= 45 then
					--计算队伍总等级之和
					totallevel = totallevel + rolelevel
					teamnum = teamnum + 1
					if rolelevel > maxlevel then
						maxlevel = rolelevel
						--maxlevelrole = Role[k]
					end
				end
			end
		end
		if teamnum == 1 then--当等级40以上的只有一人时
			local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
			monlevel = math.floor(rolelevel/5)*5
		else
			local avglevel = (totallevel - maxlevel)/(teamnum-1)
			monlevel = avglevel + (maxlevel-avglevel)/6
			local m = math.fmod(monlevel,5)
			monlevel = math.floor(monlevel/5)*5
			if m > 3 then
				monlevel = monlevel + 5
			end
		end
		return monlevel
	end
end
