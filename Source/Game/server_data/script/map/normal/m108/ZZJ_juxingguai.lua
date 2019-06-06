

--震珠界刷新巨型怪
--2310171	迷迭香
--role.GetRoleTotalStrength(MapID, InstanceID, Role[i]) 得到玩家最大实力值

function ZZJ_juxingguai(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local midiexiang = 0
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
    local a = math.random(100)
	local b = math.random(500)
	if MapID and MapID == 2291723889 then
		if a == 1 then
			if TeamID ~= nil and TeamID ~= 4294967295 then
				local Role = {}
				Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
				local Num = 0
				local totalshili = 0
				-- 判断队伍成员身上是否有迷迭香
				for i=1, 6 do
					if Role[i] and Role[i]~= 4294967295 then
						local Item = role.GetRoleItemNum(Role[i], 2310171)
						if  Item ~= nil and Item ~= 0 then
							midiexiang = 1
							role.RemoveFromRole(MapID, InstanceID, Role[i], 2310171, 1, 420)
						break
						end
					end
				end
				--有迷迭香的情况下，获取队伍的实力值，并计算出平均实力值
				if 	midiexiang ~= nil and midiexiang == 1 then
					for t = 1, 6 do
						if Role[t] and Role[t]~= 4294967295 then
							local level = role.GetRoleLevel(MapID, InstanceID, Role[t]) --玩家等级
							if level ~= nil and level >= 100 then 	--超过100级的玩家才可以计算实力值
								local roleshili = role.GetRoleTotalStrength(MapID, InstanceID, Role[t]) --得到玩家的实力值
								if roleshili ~= nil  then
									Num = Num + 1
									totalshili = totalshili + roleshili
								end
							end
						end
					end
				end
				-- 根据玩家队伍的平均实力值刷新怪物
				if midiexiang ~= nil and midiexiang == 1 then
					local average_shili = math.floor(totalshili/Num)
					if average_shili <= 10000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544152)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544152 ,x ,y ,z)
					elseif average_shili > 10000 and average_shili <= 18000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544153)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544153 ,x ,y ,z)
					elseif average_shili > 18000 and average_shili <= 26000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544154)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544154 ,x ,y ,z)
					elseif average_shili > 26000 and average_shili <= 31500 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544155)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544155 ,x ,y ,z)
					elseif average_shili > 31500 and average_shili <= 43000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544156)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544156 ,x ,y ,z)
					else
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544157)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544157 ,x ,y ,z)
					end
				end
			else
				local Item = role.GetRoleItemNum(RoleID, 2310171)
				if Item ~=nil and Item >= 1 then
					local roleshili = role.GetRoleTotalStrength(MapID, InstanceID, RoleID) --得到玩家的实力值
					if roleshili <= 10000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544152)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544152 ,x ,y ,z)
					elseif roleshili > 10000 and roleshili <= 18000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544153)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544153 ,x ,y ,z)
					elseif roleshili > 18000 and roleshili <= 26000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544154)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544154 ,x ,y ,z)
					elseif roleshili > 26000 and roleshili <= 31500 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544155)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544155 ,x ,y ,z)
					elseif roleshili > 31500 and roleshili <= 43000 then
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544156)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544156 ,x ,y ,z)
					else
						local MsgID = msg.BeginMsgEvent() --提示刷新出了XX怪物
						msg.AddMsgEvent(MsgID, 26, 4220)
						msg.AddMsgEvent(MsgID, 5, 1544157)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						map.MapCreateCreature(MapID, InstanceID, 1544157 ,x ,y ,z)
					end
					role.RemoveFromRole(MapID, InstanceID, RoleID, 2310171, 1, 420)
				end
			end
		end
		if b == 1 then
			map.MapCreateCreature(MapID, InstanceID, 1544158 ,x ,y ,z)
		end
	end
end

aux.RegisterCreatureEvent(1544101,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544102,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544103,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544104,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544105,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544106,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544107,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544108,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544109,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544110,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544111,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544112,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544113,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544114,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544115,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544116,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544117,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544118,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544119,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544120,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544121,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544122,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544123,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544124,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544125,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544126,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544127,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544128,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544129,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544130,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544131,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544132,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544133,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544134,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544135,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544136,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544137,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544138,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544139,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544140,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544141,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544142,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544143,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544144,4,"ZZJ_juxingguai")
aux.RegisterCreatureEvent(1544145,4,"ZZJ_juxingguai")

---杀死巨型怪物添加代币以及迷迭香

function ZZJ_tianjiadaoju(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--根据实力值添加代币
	local roleshili = role.GetRoleTotalStrength(MapID, InstanceID, RoleID) --得到玩家的实力值
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID) --玩家等级
	local k = math.random(10)
	if level >= 100 then 	--超过100级的玩家才可以计算实力值
		if roleshili <= 10000 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 1, -1, 8, 420)
		elseif roleshili > 10000 and roleshili <= 18000 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 2, -1, 8, 420)
		elseif roleshili > 18000 and roleshili <= 26000 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 3, -1, 8, 420)
		elseif roleshili > 26000 and roleshili <= 31500 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 4, -1, 8, 420)
		elseif roleshili > 31500 and roleshili <= 43000 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 5, -1, 8, 420)
		else
			role.AddRoleItem(MapID, InstanceID, RoleID, 2310170, 6, -1, 8, 420)
		end
	end
	--10%几率添加迷迭香
	if k == 1 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 2310171, 1, -1, 8, 420)
	end
end

function ZZJ_juxingguai_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= nil and  TeamID ~=4294967295 then
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			ZZJ_tianjiadaoju(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
		end
	else
		ZZJ_tianjiadaoju(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	end
end

aux.RegisterCreatureEvent(1544152,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544153,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544154,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544155,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544156,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544157,4,"ZZJ_juxingguai_OnDie")
aux.RegisterCreatureEvent(1544158,4,"ZZJ_juxingguai_OnDie")

























