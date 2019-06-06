

--击杀怪物几率刷出巨型怪物


---------------------------------------------------野外场景-------------------------------------------------

-- MapID == 3017298127 --龙城
-- MapID == 3017298383 --凤翔
-- MapID == 3017299663 --涿鹿
-- MapID == 3017299919 --巫水
-- MapID == 3017299151 --武夷
-- MapID == 3017299407 --昆仑

function YW_JXG_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

    if MapID == 3017298127 then  ---------------------龙城场景 ------------------------

			if TargetTypeID == 1003014 or TargetTypeID == 1003015 or TargetTypeID == 1003017 or TargetTypeID == 1003018 then
					local temp = math.random(1,200)
					if temp==1 then
							--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534301)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534301)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534301, x, y, z)
					end
			elseif TargetTypeID == 1003021 or TargetTypeID == 1003022 or TargetTypeID == 1003025 or TargetTypeID == 1003026 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534302)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534302)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534302, x, y, z)
					end
			elseif TargetTypeID == 1003035 or TargetTypeID == 1003038 or TargetTypeID == 1003041 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
						msg.AddMsgEvent(MsgID, 5, 1534303)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 364)
						msg.AddMsgEvent(MsgID, 5, 1534303)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						--1/200几率出现
						map.MapCreateCreature(MapID, InstanceID, 1534303, x, y, z)
					end
			elseif TargetTypeID == 1003046 or TargetTypeID == 1003047 or TargetTypeID == 1003049 or TargetTypeID == 1003050 or TargetTypeID == 1003125 or TargetTypeID == 1003126 or TargetTypeID == 1003127  or TargetTypeID == 1003128 then
				    local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534304)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534304)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534304, x, y, z)
					end
			elseif TargetTypeID == 1003076 or TargetTypeID == 1003077 then
				    local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534305)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534305)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534305, x, y, z)
					end
			elseif TargetTypeID == 1003088 or TargetTypeID == 1003089 then
				    local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534306)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534306)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							 map.MapCreateCreature(MapID, InstanceID, 1534306, x, y, z)
					end
			elseif TargetTypeID == 1003082 or TargetTypeID == 1003083 or TargetTypeID == 1003084 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534307)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534307)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534307, x, y, z)
					end
			elseif TargetTypeID == 1003085 or TargetTypeID == 1003086 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534308)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534308)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534308, x, y, z)
					end
			elseif TargetTypeID == 1003093 or TargetTypeID == 1003094 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534309)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534309)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534309, x, y, z)
					end
			elseif TargetTypeID == 1003091 or TargetTypeID == 1003092 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534310)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534310)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534310, x, y, z)
					end
			elseif TargetTypeID == 1003095 or TargetTypeID == 1003151 or TargetTypeID == 1003152 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534311)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534311)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534311, x, y, z)
					end
			elseif TargetTypeID == 1003153 or TargetTypeID == 1003154 then
					local temp = math.random(1,200)
					if temp==1 then
							--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534312)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534312)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534312, x, y, z)
					end
			elseif TargetTypeID == 1003101 then
					local temp = math.random(1,200)
					if temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534313)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534313)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534313, x, y, z)
					end
			elseif TargetTypeID == 1003102 then
					local temp = math.random(1,200)
					if temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534314)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534314)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534314, x, y, z)
					end
			elseif TargetTypeID == 1003105 or TargetTypeID == 1003106 then
					local temp = math.random(1,200)
					if temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534315)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534315)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534315, x, y, z)
					end
            end

	elseif MapID == 3017298383  then  -------------------- 凤翔场景 -----------------------

			if TargetTypeID == 1004014 or TargetTypeID == 1004015 or TargetTypeID == 1004016 or TargetTypeID == 1004017 then
					local temp = math.random(1,200)
					if temp==1 then
							--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534316)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534316)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534316, x, y, z)
					end
		    elseif TargetTypeID == 1004101 or TargetTypeID == 1004102 or TargetTypeID == 1004103 or TargetTypeID == 1004104 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534317)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534317)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534317, x, y, z)
					end
		    elseif TargetTypeID == 1004114 or TargetTypeID == 1004116 or TargetTypeID == 1004117 or TargetTypeID == 1004118 or TargetTypeID == 1004119 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534318)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534318)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534318, x, y, z)
					end
		    elseif TargetTypeID == 1004129 or TargetTypeID == 1004130 or TargetTypeID == 1004131 or TargetTypeID == 1004132 or TargetTypeID == 1004133 or TargetTypeID == 1004134 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534319)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534319)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534319, x, y, z)
					end
		    elseif TargetTypeID == 1004210 or TargetTypeID == 1004211 or TargetTypeID == 1004212 or TargetTypeID == 1004213 or TargetTypeID == 1004214 or TargetTypeID == 1004215 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534320)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534320)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534320, x, y, z)
					end
		    elseif TargetTypeID == 1004236 or TargetTypeID == 1004237 or TargetTypeID == 1004238 or TargetTypeID == 1004239 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534321)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534321)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534321, x, y, z)
					end
		    elseif TargetTypeID == 1004311 or TargetTypeID == 1004312 or TargetTypeID == 1004313 or TargetTypeID == 1004314 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534322)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534322)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534322, x, y, z)
					end
		    elseif TargetTypeID == 1004327 or TargetTypeID == 1004328 or TargetTypeID == 1004329 or TargetTypeID == 1004330 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534323)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534323)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534323, x, y, z)
					end
			end

	elseif MapID == 3017299663  then --------------------- 涿鹿场景 --------------------------

			if TargetTypeID == 1005006 or TargetTypeID == 1005007 or TargetTypeID == 1005008 or TargetTypeID == 1005009 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534324)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534324)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534324, x, y, z)
					end
		    elseif TargetTypeID == 1005021 or TargetTypeID == 1005022 or TargetTypeID == 1005023 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534325)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534325)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534325, x, y, z)
					end
		    elseif TargetTypeID == 1005109 or TargetTypeID == 1005110 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534326)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534326)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534326, x, y, z)
					end
		    elseif TargetTypeID == 1005205 or TargetTypeID == 1005206 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534327)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534327)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534327, x, y, z)
					end
		    elseif TargetTypeID == 1005217 or TargetTypeID == 1005218 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534328)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534328)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534328, x, y, z)
					end
		    elseif TargetTypeID == 1005223 or TargetTypeID == 1005224 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534329)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534329)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534329, x, y, z)
					end
		    elseif TargetTypeID == 1005303 or TargetTypeID == 1005304 or TargetTypeID == 1005305 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534330)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534330)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534330, x, y, z)
					end
			elseif TargetTypeID == 1005318 or TargetTypeID == 1005319 or TargetTypeID == 1005320 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534331)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534331)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534331, x, y, z)
					end
			end

	elseif MapID == 3017299919   then ----------------巫水场景---------------

			if TargetTypeID == 1006101 or TargetTypeID == 1006102 or TargetTypeID == 1006103 or TargetTypeID == 1006104 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534332)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534332)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534332, x, y, z)
					end
			elseif TargetTypeID == 1006113 or TargetTypeID == 1006114 or TargetTypeID == 1006115 or TargetTypeID == 1006116 or TargetTypeID == 1006117 or TargetTypeID == 1006118 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534333)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534333)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534333, x, y, z)
					end
			elseif TargetTypeID == 1006217 or TargetTypeID == 1006218 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534334)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534334)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534334, x, y, z)
					end
			elseif TargetTypeID == 1006208 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534335)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534335)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534335, x, y, z)
					end
			elseif TargetTypeID == 1006301 or TargetTypeID == 1006302 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534336)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534336)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534336, x, y, z)
					end
			elseif TargetTypeID == 1006314 or TargetTypeID == 1006315 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534337)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534337)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534337, x, y, z)
					end
			elseif TargetTypeID == 1006329 or TargetTypeID == 1006330 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534338)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534338)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534338, x, y, z)
					end
			elseif TargetTypeID == 1006417 or TargetTypeID == 1006418 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534339)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534339)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534339, x, y, z)
					end
			end

	elseif MapID == 3017299151  then ----------------------- 武夷场景 ---------------------

			if TargetTypeID == 1007007 or TargetTypeID == 1007008 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534340)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534340)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534340, x, y, z)
					end
			elseif TargetTypeID == 1007013 or TargetTypeID == 1007014 or TargetTypeID == 1007015 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534341)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534341)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534341, x, y, z)
					end
			elseif TargetTypeID == 1007032 or TargetTypeID == 1007033 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534342)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534342)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534342, x, y, z)
					end
			elseif TargetTypeID == 1007040 or TargetTypeID == 1007041 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534343)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534343)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534343, x, y, z)
					end
			elseif TargetTypeID == 1007117 or TargetTypeID == 1007118 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534344)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534344)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534344, x, y, z)
					end
			elseif TargetTypeID == 1007123 or TargetTypeID == 1007124 or TargetTypeID == 1007125 or TargetTypeID == 1007126 or TargetTypeID == 1007127 or TargetTypeID == 1007128 or TargetTypeID == 1007129 or TargetTypeID == 1007138 or TargetTypeID == 1007139 or TargetTypeID == 1007140 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534345)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534345)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534345, x, y, z)
					end
			elseif TargetTypeID == 1007147 or TargetTypeID == 1007148 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534346)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534346)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534346, x, y, z)
					end
			elseif TargetTypeID == 1007153 or TargetTypeID == 1007154 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534347)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534347)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534347, x, y, z)
					end
			elseif TargetTypeID == 1007166 or TargetTypeID == 1007167 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534348)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534348)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534348, x, y, z)
					end
			end

	elseif MapID == 1146339967   then --------------------盘龙谷 ------------------------

			if TargetTypeID == 1007206 or TargetTypeID == 1007207 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534349)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534349)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534349, x, y, z)
					end
			elseif TargetTypeID == 1007226 or TargetTypeID == 1007227 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534350)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534350)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534350, x, y, z)
					end
			elseif TargetTypeID == 1007232 or TargetTypeID == 1007233 or TargetTypeID == 1007234 or TargetTypeID == 1007235 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534351)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534351)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534351, x, y, z)
					end
			elseif TargetTypeID == 1007253 or TargetTypeID == 1007254 or TargetTypeID == 1007255 or TargetTypeID == 1007256 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534352)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534352)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534352, x, y, z)
					end
			end

	elseif MapID == 3017299407   then ------------------- 昆仑 ---------------------

			if TargetTypeID == 1009001 or TargetTypeID == 1009002 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534353)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534353)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534353, x, y, z)
					end
			elseif TargetTypeID == 1009020 or TargetTypeID == 1009021 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534354)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534354)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534354, x, y, z)
					end
			elseif TargetTypeID == 1009031 then
				local temp = math.random(1,100)
					if temp>=1 and temp<=10 then
						local a = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 15107)
						if a == true then
							local FreeSize = role.GetBagFreeSize(RoleID)
							if FreeSize > 0 then
								role.AddRoleItem(MapID, InstanceID, RoleID, 2615029, 1, -1, 8, 420)
							end
						end
					end
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534355)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534355)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534355, x, y, z)
					end
			elseif TargetTypeID == 1009041 or TargetTypeID == 1009042 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534356)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534356)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534356, x, y, z)
					end
			elseif TargetTypeID == 1009055 or TargetTypeID == 1009056 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534357)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534357)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534357, x, y, z)
					end
			elseif TargetTypeID == 1009109 or TargetTypeID == 1009110 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534358)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534358)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534358, x, y, z)
					end
			elseif TargetTypeID == 1009112 or TargetTypeID == 1009113 or TargetTypeID == 1009114 or TargetTypeID == 1009115 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534359)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534359)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534359, x, y, z)
					end
			elseif TargetTypeID == 1009126 or TargetTypeID == 1009127 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534360)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534360)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534360, x, y, z)
					end
			elseif TargetTypeID == 1009137 or TargetTypeID == 1009138 or TargetTypeID == 1009141 or TargetTypeID == 1009142 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534361)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534361)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534361, x, y, z)
					end
			elseif TargetTypeID == 1009208 or TargetTypeID == 1009209 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534362)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534362)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534362, x, y, z)
					end
			elseif TargetTypeID == 1009307 or TargetTypeID == 1009308 or TargetTypeID == 1009309 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534363)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534363)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534363, x, y, z)
					end
			elseif TargetTypeID == 1009311 or TargetTypeID == 1009312 then
					local temp = math.random(1,200)
					if temp==1 then
						--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534364)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534364)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534364, x, y, z)
					end

			end
	end
end
aux.RegisterCreatureEvent(1003014,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003015,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003017,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003018,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003021,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003022,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003025,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003026,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003035,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003038,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003041,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003046,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003047,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003049,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003050,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003125,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003126,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003127,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003128,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003076,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003077,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003088,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003089,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003082,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003083,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003084,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003085,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003086,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003091,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003092,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003093,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003094,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003095,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003151,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003152,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003153,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003154,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003101,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003102,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003105,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1003106,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004014,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004015,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004016,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004017,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004101,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004102,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004103,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004104,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004114,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004116,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004117,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004118,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004119,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004129,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004130,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004131,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004132,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004133,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004134,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004210,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004211,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004212,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004213,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004214,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004215,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004236,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004237,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004238,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004239,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004311,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004312,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004313,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004314,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004327,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004328,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004329,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1004330,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005006,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005007,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005008,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005009,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005021,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005022,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005023,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005109,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005110,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005205,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005206,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005217,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005218,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005223,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005224,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005303,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005304,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005305,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005318,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005319,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1005320,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006101,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006102,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006103,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006104,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006113,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006114,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006115,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006116,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006117,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006118,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006217,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006218,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006208,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006301,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006302,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006314,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006315,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006329,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006330,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006417,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1006418,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007007,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007008,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007013,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007014,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007015,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007032,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007033,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007040,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007041,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007117,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007118,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007123,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007124,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007125,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007126,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007127,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007128,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007129,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007138,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007139,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007140,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007147,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007148,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007153,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007154,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007166,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007167,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007206,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007207,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007226,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007227,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007232,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007233,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007234,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007235,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007253,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007254,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007255,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1007256,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009001,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009002,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009020,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009021,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009031,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009041,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009042,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009055,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009056,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009109,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009110,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009112,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009113,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009114,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009115,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009126,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009127,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009137,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009138,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009141,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009142,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009208,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009209,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009307,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009308,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009309,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009311,4,"YW_JXG_OnDie")
aux.RegisterCreatureEvent(1009312,4,"YW_JXG_OnDie")

------------------------------------------修炼场--------------------------------------------------

function XL_JXG_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

		if TargetTypeID == 1543609 or TargetTypeID == 1543610 or TargetTypeID == 1543611 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534401)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534401)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534401, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534402)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534402)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534402, x, y, z)
					end
		elseif TargetTypeID == 1543621 or TargetTypeID == 1543622 or TargetTypeID == 1543623 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534403)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534403)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534403, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534404)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534404)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534404, x, y, z)
					end
		elseif TargetTypeID == 1543706 or TargetTypeID == 1543707 or TargetTypeID == 1543708 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534405)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534405)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534405, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534406)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534406)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534406, x, y, z)
					end
		elseif TargetTypeID == 1543721 or TargetTypeID == 1543722 or TargetTypeID == 1543723 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534407)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534407)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534407, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534408)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534408)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534408, x, y, z)
					end
		elseif TargetTypeID == 1543809 or TargetTypeID == 1543810 or TargetTypeID == 1543811 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534409)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534409)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534409, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534410)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534410)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534410, x, y, z)
					end
		elseif TargetTypeID == 1543821 or TargetTypeID == 1543822 or TargetTypeID == 1543823 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534411)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534411)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534411, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534412)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534412)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534412, x, y, z)
					end

		elseif TargetTypeID == 1543009 or TargetTypeID == 1543010 or TargetTypeID == 1543011 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534413)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534413)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534413, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534414)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534414)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534414, x, y, z)
					end
		elseif TargetTypeID == 1543021 or TargetTypeID == 1543022 or TargetTypeID == 1543023 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534415)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534415)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534415, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534416)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534416)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534416, x, y, z)
					end

		elseif TargetTypeID == 1543109 or TargetTypeID == 1543110 or TargetTypeID == 1543111 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534417)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534417)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534417, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534418)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534418)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534418, x, y, z)
					end
	    elseif TargetTypeID == 1543121 or TargetTypeID == 1543122 or TargetTypeID == 1543123 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534419)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534419)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534419, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534420)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534420)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534420, x, y, z)
					end

		elseif TargetTypeID == 1543209 or TargetTypeID == 1543210 or TargetTypeID == 1543211 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534421)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534421)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534421, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534422)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534422)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534422, x, y, z)
					end
		elseif TargetTypeID == 1543221 or TargetTypeID == 1543222 or TargetTypeID == 1543223 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534423)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534423)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534423, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534424)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534424)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							--1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534424, x, y, z)
					end

		elseif TargetTypeID == 1543309 or TargetTypeID == 1543310 or TargetTypeID == 1543311 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534425)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534425)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534425, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534426)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534426)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534426, x, y, z)
					end
		elseif TargetTypeID == 1543321 or TargetTypeID == 1543322 or TargetTypeID == 1543323 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534427)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534427)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534427, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534428)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534428)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534428, x, y, z)
					end

		elseif TargetTypeID == 1543409 or TargetTypeID == 1543410 or TargetTypeID == 1543411 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534429)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534429)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534429, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534430)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534430)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534430, x, y, z)
					end
		elseif TargetTypeID == 1543421 or TargetTypeID == 1543422 or TargetTypeID == 1543423 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534431)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534431)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534431, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534432)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534432)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534432, x, y, z)
					end

		elseif TargetTypeID == 1543509 or TargetTypeID == 1543510 or TargetTypeID == 1543511 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534433)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534433)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534433, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534434)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534434)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
							map.MapCreateCreature(MapID, InstanceID, 1534434, x, y, z)
					end
		elseif TargetTypeID == 1543521 or TargetTypeID == 1543522 or TargetTypeID == 1543523 then
					local temp = math.random(1,200)
					if  temp==1 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534435)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534435)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534435, x, y, z)
					end
					if  temp==2 then
								--沉睡的##被您惊醒了!
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 0)
							msg.AddMsgEvent(MsgID, 1, 364)
							msg.AddMsgEvent(MsgID, 5, 1534436)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 364)
							msg.AddMsgEvent(MsgID, 5, 1534436)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
					        --1/200几率出现
					        map.MapCreateCreature(MapID, InstanceID, 1534436, x, y, z)
					end
	    end
end
aux.RegisterCreatureEvent(1543609,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543610,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543611,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543621,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543622,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543623,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543706,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543707,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543708,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543721,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543722,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543723,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543809,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543810,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543811,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543821,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543822,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543823,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543009,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543010,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543011,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543021,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543022,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543023,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543109,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543110,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543111,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543121,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543122,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543123,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543209,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543210,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543211,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543221,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543222,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543223,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543309,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543310,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543311,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543321,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543322,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543323,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543409,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543410,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543411,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543421,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543422,4,"XL_JXG_OnDie")
aux.RegisterCreatureEvent(1543423,4,"XL_JXG_OnDie")

