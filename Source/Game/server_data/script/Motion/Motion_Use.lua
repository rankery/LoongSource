
--
function Motion_OnUse(MapID,InstanceID,RoleID, DstRoleID, MotionID)
	if Qingrenjie == 1 then
		if MotionID==14  and role.GetRoleItemNum(RoleID,2616518) > 0 then
			local k = math.random(100)
			if k <= 25 then
				role.RemoveFromRole(MapID,InstanceID,RoleID,2616518,1,420)
				role.AddRoleItem(MapID, InstanceID, RoleID, 2616519, 1, -1, 8, 420)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 12613)--牛郎的祝福状态消失后才能再次获得放牛绳
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	end

	if act.GetActIsStart(151) then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		for i = 1,4 do
			if m02_qixi_point[i].npc == 1 and (x < m02_qixi_point[i].x + 100 or x > m02_qixi_point[i].x - 100) and (z < m02_qixi_point[i].z + 100 or z > m02_qixi_point[i].z - 100) then
				local a = role.IsRoleHaveBuff(MapID,InstanceID,RoleID,50050)
				local b = role.IsRoleHaveBuff(MapID,InstanceID,DstRoleID,50050)
				if MotionID==14 then
					if a == false then
						unit.AddBuff(MapID, InstanceID, RoleID, 5005001, RoleID)
						role.AddRoleItem(MapID, InstanceID, RoleID, 3305001,1 , -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12140)--牛郎的祝福状态消失后才能再次获得放牛绳
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
					if b == false then
						unit.AddBuff(MapID, InstanceID, DstRoleID, 5005001, DstRoleID)
						role.AddRoleItem(MapID, InstanceID, DstRoleID, 3305001,1 , -1, 8, 420)
					else
						--提示
					    local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12140)
						msg.DispatchRoleMsgEvent(DstRoleID, MsgID)
					end
				end
			elseif m02_qixi_point[i].npc == 2 and (x < m02_qixi_point[i].x + 100 or x > m02_qixi_point[i].x - 100) and (z < m02_qixi_point[i].z + 100 or z > m02_qixi_point[i].z - 100) then
				local a = role.IsRoleHaveBuff(MapID,InstanceID,RoleID,50051)
				local b = role.IsRoleHaveBuff(MapID,InstanceID,DstRoleID,50051)
				if MotionID==14 then
					if a == false then
						unit.AddBuff(MapID, InstanceID, RoleID, 5005101, RoleID)
						role.AddRoleItem(MapID, InstanceID, RoleID, 3305002,1 , -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12141)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
					if b == false then
						unit.AddBuff(MapID, InstanceID, DstRoleID, 5005101, DstRoleID)
						role.AddRoleItem(MapID, InstanceID, DstRoleID, 3305002,1 , -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12141)
						msg.DispatchRoleMsgEvent(DstRoleID, MsgID)
					end
				end
			elseif m04_qixi_point[i].npc == 1 and (x < m04_qixi_point[i].x + 100 or x > m04_qixi_point[i].x - 100) and (z < m04_qixi_point[i].z + 100 or z > m04_qixi_point[i].z - 100) then
				local a = role.IsRoleHaveBuff(MapID,InstanceID,RoleID,50050)
				local b = role.IsRoleHaveBuff(MapID,InstanceID,DstRoleID,50050)
				if MotionID==14 then
					if a == false then
						unit.AddBuff(MapID, InstanceID, RoleID, 5005001, RoleID)
						role.AddRoleItem(MapID, InstanceID, RoleID, 3305001, 1, -1, 8, 420)
						else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12140)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
					if b == false then
						unit.AddBuff(MapID, InstanceID, DstRoleID, 5005001, DstRoleID)
						role.AddRoleItem(MapID, InstanceID, DstRoleID, 3305001, 1, -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12140)
						msg.DispatchRoleMsgEvent(DstRoleID, MsgID)
					end
				end
			elseif m04_qixi_point[i].npc == 2 and (x < m04_qixi_point[i].x + 100 or x > m04_qixi_point[i].x - 100) and (z < m04_qixi_point[i].z + 100 or z > m04_qixi_point[i].z - 100) then
				local a = role.IsRoleHaveBuff(MapID,InstanceID,RoleID,50051)
				local b = role.IsRoleHaveBuff(MapID,InstanceID,DstRoleID,50051)
				if MotionID==14 then
					if a == false then
						unit.AddBuff(MapID, InstanceID, RoleID, 5005101, RoleID)
						role.AddRoleItem(MapID, InstanceID, RoleID, 3305002, 1, -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12141)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
					if b == false then
						unit.AddBuff(MapID, InstanceID, DstRoleID, 5005101, DstRoleID)
						role.AddRoleItem(MapID, InstanceID, DstRoleID, 3305002, 1, -1, 8, 420)
					else
						--提示
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 12141)
						msg.DispatchRoleMsgEvent(DstRoleID, MsgID)
					end
				end
			end
		end
	end

	if MotionID==10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		if MapID == 3017298127 and math.abs(x-1207)< 50 and math.abs(z-3664)< 50 and role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20196) == true then
			role.RoleCompleteQuest(RoleID, 20196, -1)
		elseif MapID == 3017298383 and math.abs(x-1760)< 50 and math.abs(z-1833)< 50 and role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20197) == true then
			role.RoleCompleteQuest(RoleID, 20197, -1)
		elseif MapID == 3017299663 and math.abs(x-1821)< 50 and math.abs(z-1851)< 50 and role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20198) == true then
			role.RoleCompleteQuest(RoleID, 20198, -1)
		end
	end

--***************************************	 国庆活动在特殊区域内使用个性动作	******************************************************

	-- 巫水	触发区域 ObjID = 6307  --
	-- 涿鹿 触发区域 ObjID = 10994 --
	-- 鞠躬 2
	-- 大笑 6
	-- 行礼 7
	-- 挑衅 10
	-- 酷秀 13
	--1535410	校场杂兵	15
	--1535411	校场杂兵	30
	--1535412	校场杂兵	45
	--1535413	校场杂兵	60
	--1535414	校场杂兵	75
	--1535415	校场杂兵	90
	--9437201				挑衅等待
	--9437301				鞠躬等待
	--9437401				行礼·欢欣
	--9437501				酷秀等待
	--9437601				大笑等待

	--Tiaoxincishu = 0  --服务器内玩家进行挑衅次数

	if act.GetActIsStart(153) then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		local s = map.IsInArea(3017299919, -1, RoleID, 6307)	--返回true或false
		local t = map.IsInArea(3017299663, -1, RoleID, 10994)	--返回true或false
		local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local Bebuff_2 = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,94373) -- 记录玩家是否有鞠躬等待BUFF
		local Bebuff_6 = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,94376) -- 记录玩家是否有大笑等待BUFF
		local Bebuff_7 = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,94374) -- 记录玩家是否有行礼·欢欣BUFF
		local Bebuff_10 = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,94372) -- 记录玩家是否有大笑等待BUFF
		local Bebuff_13 = role.IsRoleHaveBuff(MapID, InstanceID, RoleID,94375) -- 记录玩家是否有酷秀等待BUFF

		if MotionID== 6 then -- 大笑动作 根据等级添加奖励
			if Guaiwuyuebing == 1 then -- 怪物阅兵开始
				if s == true then  -- 是否在区域中
					if Bebuff_6  == false then
						if Level >= 80 then
							role.AddRoleItem(MapID, InstanceID, RoleID, 6000113, 1, -1, 8, 420)	-- 添加国庆大礼包
						else
							role.AddRoleItem(MapID, InstanceID, RoleID, 3303031, 1, -1, 8, 420)	-- 添加盛世饺子
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 4015) -- 向怪物们大笑，士气得到提升，获得系统给予的奖励
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						end
						unit.AddBuff(MapID, InstanceID, RoleID, 9437601 ,RoleID)
					end
				end
			end
		end

		if MotionID == 10 then -- 挑衅动作 召唤杂兵
			if Guaiwuyuebing == 1 then
				if s == true then
					if Bebuff_10 == false then
						Tiaoxincishu = Tiaoxincishu +1  -- 服务器使用挑衅动作次数+1
						if Tiaoxincishu < 100 then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 4016) -- 你的挑衅动作，似乎令某只怪物愤怒了
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							if	Level >0 and Level <=15 then
								map.MapCreateColCreature(MapID, InstanceID, 1535410, x, y, z, 1)
							elseif
								Level >16 and Level <=30 then
								map.MapCreateColCreature(MapID, InstanceID, 1535411, x, y, z, 1)
							elseif
								Level >31 and Level <=45 then
								map.MapCreateColCreature(MapID, InstanceID, 1535412, x, y, z, 1)
							elseif
								Level >46 and Level <=60 then
								map.MapCreateColCreature(MapID, InstanceID, 1535413, x, y, z, 1)
							elseif
								Level >61 and Level <=75 then
								map.MapCreateColCreature(MapID, InstanceID, 1535414, x, y, z, 1)
							else
								map.MapCreateColCreature(MapID, InstanceID, 1535415, x, y, z, 1)
							end
						else
							if  Tiaoxincishu%100 == 0 then
								local MsgID = msg.BeginMsgEvent()
								msg.AddMsgEvent(MsgID, 100, 4017)--世界频道公布信息：“玩家xx对怪物阅兵进行了第xx次挑衅，愤怒的怪物们终于忍不住了，誓要与众位挑衅者进行殊死搏斗。
								msg.AddMsgEvent(MsgID, 2, RoleID)
								msg.AddMsgEvent(MsgID, 12, Tiaoxincishu)
								msg.DispatchWorldMsgEvent(MsgID)
								map.MapCreateColCreature(MapID, InstanceID, 1535410, 1683, 26847, 719, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535415, 1690, 26847, 723, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535411, 1700, 26847, 729, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535414, 1712, 26847, 735, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535412, 1723, 26847, 730, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535413, 1732, 26847, 727, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535413, 1741, 26847, 722, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535412, 1752, 26847, 714, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535414, 1754, 26847, 702, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535411, 1754, 26847, 696, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535415, 1755, 26847, 682, 1)
								map.MapCreateColCreature(MapID, InstanceID, 1535410, 1749, 26847, 668, 1)
							else
								local MsgID = msg.BeginMsgEvent()
								msg.AddMsgEvent(MsgID, 26, 4016) -- 你的挑衅动作，似乎令某只怪物愤怒了
								msg.DispatchRoleMsgEvent(RoleID, MsgID)
								if	Level >0 and Level <=15 then
									map.MapCreateColCreature(MapID, InstanceID, 1535410, x, y, z, 1)
								elseif
									Level >16 and Level <=30 then
									map.MapCreateColCreature(MapID, InstanceID, 1535411, x, y, z, 1)
								elseif
									Level >31 and Level <=45 then
									map.MapCreateColCreature(MapID, InstanceID, 1535412, x, y, z, 1)
								elseif
									Level >46 and Level <=60 then
									map.MapCreateColCreature(MapID, InstanceID, 1535413, x, y, z, 1)
								elseif
									Level >61 and Level <=75 then
									map.MapCreateColCreature(MapID, InstanceID, 1535414, x, y, z, 1)
								else
									map.MapCreateColCreature(MapID, InstanceID, 1535415, x, y, z, 1)
								end
							end
						end
						unit.AddBuff(MapID, InstanceID, RoleID, 9437201 ,RoleID)
					end
				end
			end
		end

		if MotionID == 2 then -- 鞠躬
			local exp = math.floor(activity_newexp[Level]*0.25)
			if Npcyuebing == 1 then -- NPC阅兵开始
				if t == true then
					if Bebuff_2 == false then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 4018) -- 向将士们鞠躬，士气得到提升，获得系统给予的奖励
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
						role.AddRoleExp(MapID, InstanceID, RoleID, exp)
						unit.AddBuff(MapID, InstanceID, RoleID, 9437301 ,RoleID)
					end
				end
			end
		end

		if MotionID == 7 then -- 行礼
			if Npcyuebing == 1 then -- NPC阅兵开始
				if t == true then
					if Bebuff_7 == false then
						if Level >= 30 then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 4019) -- 向将士们行礼，士气得到提升，获得系统给予的奖励
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							unit.AddBuff(MapID, InstanceID, RoleID, 9437401 ,RoleID)
						end
					end
				end
			end
		end

		if MotionID == 13 then -- 酷秀
			if Npcyuebing == 1 then -- NPC阅兵开始
				if t == true then
					if Bebuff_13 == false then
						if Level >= 60 then
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 26, 4020) -- 向将士们酷秀，士气得到提升，获得系统给予的奖励
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
							if role.GetSoarValue(RoleID) == 1 then -- 玩家角色已飞升
								role.AddRoleItem(MapID, InstanceID, RoleID, 3300104, 2, -1, 8, 420)	-- 仙界飞升令
							end
							if role.GetSoarValue(RoleID) == 0 then -- 玩家角色未飞升
								role.AddRoleItem(MapID, InstanceID, RoleID, 3300102, 1, -1, 8, 420) --仙界渡劫令
								role.AddRoleItem(MapID, InstanceID, RoleID, 3300103, 1, -1, 8, 420) --仙界天劫令
							end
							unit.AddBuff(MapID, InstanceID, RoleID, 9437501 ,RoleID)
						end
					end
				end
			end
		end
	end
--***************************************	 国庆活动在特殊区域内使用个性动作	******************************************************


end

-- 注册相应事件
aux.RegisterMotionEvent(0, "Motion_OnUse")

