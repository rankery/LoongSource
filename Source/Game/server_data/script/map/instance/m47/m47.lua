--map script data
--0. 小怪击杀数量
--1. BOSS击杀数量
--2-7. 使用过心石碎片的人的id
--8. 副本进度index
--9. 召唤的start_tick
--10. 已参加召唤的人数
--11-16. 已领取奖励的人的id
--17. BOSS 类型
--18-20. 已参加召唤的人的id



--可否使用函数
function i3304016_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在心魔界，则不能使用
	if MapID ~= 3084408271 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x <= 364 or x >= 424 or z <= 260 or z >= 320 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
	    end

		for i=2,7 do

			if TargetID==map.GetMapScriptData(MapID, InstanceID, 1, i) then
				bRet = 32
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1824)			--已经使用过
				msg.DispatchRoleMsgEvent(TargetID, MsgID)
				break
			end

		end

		if bRet~=40 and map.GetMapScriptData(MapID, InstanceID, 1, 7)~=0 then
			bRet = 32
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1825)			--已经使用过
			msg.DispatchRoleMsgEvent(TargetID, MsgID)
		end

	end

	--返回
	return bRet, bIgnore
end

--钥匙使用效果
function i3304016_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	for i=2,7 do
		if map.GetMapScriptData(MapID, InstanceID, 1, i)==0 then
			map.SetMapScriptData(MapID, InstanceID, 1, i, TargetID)
			break
		end
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1826)			--已召唤出心魔界妖魔
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)

	map.MapCreateCreature(MapID, InstanceID, 1011011, 411, y, 297)
	map.MapCreateCreature(MapID, InstanceID, 1011011, 395, y, 300)
	map.MapCreateCreature(MapID, InstanceID, 1011011, 401, y, 276)
	map.MapCreateCreature(MapID, InstanceID, 1011011, 374, y, 262)
	map.MapCreateCreature(MapID, InstanceID, 1011011, 397, y, 260)
	map.MapCreateCreature(MapID, InstanceID, 1011012, 412, y, 312)
	map.MapCreateCreature(MapID, InstanceID, 1011012, 378, y, 319)
	map.MapCreateCreature(MapID, InstanceID, 1011012, 395, y, 332)
	map.MapCreateCreature(MapID, InstanceID, 1011012, 378, y, 300)
	map.MapCreateCreature(MapID, InstanceID, 1011012, 411, y, 310)

end

--注册
aux.RegisterItemEvent(3304016, 1, "i3304016_QuestUsable")
aux.RegisterItemEvent(3304016, 0, "i3304016_CanUse")


--神迹争夺奖励人脚本

function n_XinShiTai(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	local summon_start = map.GetMapScriptData(MapID, InstanceID, 1, 8)

	if summon_start==100 then		--战斗已开始

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1827)		--战斗已开始，不能进行召唤
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif summon_start==200 then	--BOSS已死亡，。领取奖励

		local can_receive = 1

		for i = 11, 16 do

			if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then

				can_receive = 0
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1828)		--您已领取过奖励
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			end

		end

		if map.GetMapScriptData(MapID, InstanceID, 1, 16)~=0 then

			can_receive = 0
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1829)		--已有6人领取过奖励
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

		end

		local FreeSize = role.GetBagFreeSize(RoleID)

		if FreeSize<=1 then
			can_receive = 0
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1831)		--行囊已满
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end

		if can_receive == 1 then
			for i = 11, 16 do
				if map.GetMapScriptData(MapID, InstanceID, 1, i)==0 then

					local BossType = map.GetMapScriptData(MapID, InstanceID, 1, 17)

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1830)		--领取奖励成功
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

					role.AddRoleItem(MapID, InstanceID, RoleID, 3304000+BossType, 3, -1, 8, 420)

					map.SetMapScriptData(MapID, InstanceID, 1, i, RoleID)

					break

				end
			end
		end

	else

		if TalkIndex == -1 then

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1832)
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	1833)		--召唤心魔1
			msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	1834)		--召唤心魔2
			msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	1835)		--召唤心魔3
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

		elseif TalkIndex == 4 or TalkIndex == 5 or TalkIndex == 6 then

			if summon_start==0 then
			--开始召唤

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1832+TalkIndex)	--已开始召唤
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

				local start_tick = aux.GetWorldTick()

				summon_start = TalkIndex

				map.SetMapScriptData(MapID, InstanceID, 1, 8, summon_start)
				map.SetMapScriptData(MapID, InstanceID, 1, 9, start_tick)
				map.SetMapScriptData(MapID, InstanceID, 1, 18, RoleID)
				map.SetMapScriptData(MapID, InstanceID, 1, 10, 1)

			elseif summon_start==TalkIndex then

				local can_summon = 1
				local start_tick = map.GetMapScriptData(MapID, InstanceID, 1, 9)
				local cur_tick = aux.GetWorldTick()

				for i = 18, 20 do
					if map.GetMapScriptData(MapID, InstanceID, 1, i)==RoleID then
						can_summon = 0
					end
				end

				if cur_tick-start_tick>150 then
					map.SetMapScriptData(MapID, InstanceID, 1, 8, 0)
					map.SetMapScriptData(MapID, InstanceID, 1, 10, 0)

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 1839)	--请重新开始召唤
					msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

				elseif can_summon==0 then

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1840)	--您已经进行过召唤
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.DispatchRoleMsgEvent(RoleID, MsgID)

				else

					local summon_num = map.GetMapScriptData(MapID, InstanceID, 1, 10)+1
					map.SetMapScriptData(MapID, InstanceID, 1, 17+summon_num, RoleID)
					map.SetMapScriptData(MapID, InstanceID, 1, 10, summon_num)

					if summon_num == 3 then

						local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)

						map.MapCreateCreature(MapID, InstanceID, 1011019+TalkIndex-4, x, y, z)		--刷新BOSS

						map.SetMapScriptData(MapID, InstanceID, 1, 8, 100)
						map.SetMapScriptData(MapID, InstanceID, 1, 17, TalkIndex)
						--[[所有人获得心锥

						local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
						--如果没有小队

						if TeamID == nil or TeamID == 4294967295 then
							role.AddRoleItem(MapID, InstanceID, RoleID, 3304017, 99, -1, 8, 420)
						else
							local Role = {}
							Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
							for k in pairs(Role) do
								role.AddRoleItem(MapID, InstanceID, Role[1], 3304017, 99, -1, 8, 420)
							end
						end]]--

						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 101, 1841)	--BOSS已出现
						msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

					end
				end
			elseif summon_start~=TalkIndex then

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1842)		--选择与其他人不符，请重新选择
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	1833)		--召唤心魔1
				msg.AddMsgEvent(MsgID, 21,	5)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	1834)		--召唤心魔2
				msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	1835)		--召唤心魔3
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			end


		end

	end
end

-- 注册
aux.RegisterCreatureEvent(3075222, 7, "n_XinShiTai")
