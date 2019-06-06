-------------------------------------------------------------------------------
-- Copyright (c) 2004 TENGWU Entertainment All rights reserved.
-- filename: GodMiracleCpatureDefine.h
-- author: chdeng
-- actor:
-- data: 2010-01-25
-- last:
-- brief: 神迹争夺结构定义
-------------------------------------------------------------------------------
-- \breif: 所有地图初始化时,设置神迹关联状态，并刷新指定点怪物
function GodMiracleCpature_OnActiveStart(ActiveID)
	if OpenGodMiracleFlag == 0 then
		return
	end

	InitAllGodMiracle()
end


ResultbroadcastFlag = 0

CalFlag = 0

-- 是否已经结束过标志
FinishedFlag = 0

-- \breif : 地图适时更新函数
--  param1: 活动ID
--  param2：传入时间
--  ramark: 依据时间适时更新神迹的状态
function GodMiracleCpature_OnActiveTimer(ActiveID, Sec)

	if OpenGodMiracleFlag == 0 then
		return
	end

	-- 周二，周四，周六开启
	local day = os.date("%A")
	if day ~= "Tuesday" and day ~= "Thursday" and day ~= "Saturday" then
		return
	end


	-- 内层适时更新操作
	BroadcastGodMsgTick()

	-- 外层适时更新操作
	guild.UpdateGodMiracle()

	---------------------------------------------------------------------------------------------------- 时间到结束战斗
	--检测时迹，结束战斗
	local Hour = tonumber(os.date("%H"))
	local Min  = tonumber(os.date("%M"))
	local Sec   = tonumber(os.date("%S"))


	if Hour == 20 and (Min == 30 or Min == 31) and FinishedFlag == 0 then
		CalFlag = 1
	end

	if  FinishedFlag == 1 and Hour == 20 and Min > 31 then
		FinishedFlag = 0
	end

	-- 若时间到30秒,记录一次积分
	for k,v in pairs(ClanGodMiracle_CurInfo) do
		---------------------------------------------------------------------------------------------------- 帮派解散
		-- 休战状态检查帮派是否解散
		local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(ClanGodMiracle_CurInfo[k].MapID))
		if godCurState ==  1 then

			local GuildIDDiss = GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID)
			if GuildIDDiss ~= 0 then

				-- 查看占领者帮派是否解散
				if guild.IsHaveGuild(GuildIDDiss) == false then

					local CaptureGodNum = 0
					-- 查看本帮派占领了多少个神迹
					for k1, v in pairs(ClanGodMiracle_CurInfo) do
						if GetDefendGuildID(ClanGodMiracle_CurInfo[k1].MapID) == GuildIDDiss then
							CaptureGodNum = CaptureGodNum + 1

							-- 针对世界广播,神迹坐标可争夺
							-- ###（神迹名称）保护期已过，可被各大帮派进行争夺
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 1)
							msg.AddMsgEvent(MsgID, 1, 1409)
							msg.AddMsgEvent(MsgID, 5 , ClanGodMiracle_CurInfo[k1].GdMiracleID)
							msg.DispatchWorldMsgEvent(MsgID)

							-- 帮派解散，柱子变成银色
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998701)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998701)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998701)

							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998801)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998801)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998801)

							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998901)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998901)
							unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998901)

							unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998601, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1)
							unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998601, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2)
							unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998601, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3)

							-- 针对C++和Lua做清理工作
							guild.InitGodMiracle(k1-1)
							ClearGodMiracle(k1)

							-- 设置神迹为未占领状态
							guild.SetGodCurState(k1-1, 0)


						end
					end

					-- 帮派解散,移除所有在线玩家的增益BUFF
					-- 得到帮派的所有成员
					--[[local tmpRoleIDTab = guild.GetAllGuildMember(GuildIDDiss)
					if tmpRoleIDTab ~= nil then
						for k, v in pairs(tmpRoleIDTab) do
							if (RoleTable[k] ~= nil) then
								unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k], GetBuffIDbyGodMiracleNum(CaptureGodNum))
							end
						end
					end--]]

					guild.AddBuffToAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(CaptureGodNum))
				end
			end
		end

		---------------------------------------------------------------------------------------------------- 检测战斗是否结束
		-- 非争夺状态不做处理
		local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(ClanGodMiracle_CurInfo[k].MapID))
		if godCurState == 2  then

			--BUG
			--if GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID) ~= 0 then

				------------------------------------------------------------------------------------------------- 30争夺双方积分
				if CalFlag == 0 then
					ModTick(GetCurInfoMapID(k), 1)
					if  Gettick(GetCurInfoMapID(k)) == GodEnum.StdScoresTick then
						-- 守方
						local pillarNum = GuildCapturePillar(GetCurInfoMapID(k), GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
						if pillarNum > 0 then
							ModScores(GetCurInfoMapID(k), GodEnum.Defend, pillarNum)
							guild.SetGodGuildPoint(k-1, GodEnum.Defend, pillarNum)
						end

						-- 攻方
						local pillarNum1 = GuildCapturePillar(GetCurInfoMapID(k), GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID))
						if pillarNum1 > 0 then
							ModScores(GetCurInfoMapID(k), GodEnum.Capture, pillarNum1)
							guild.SetGodGuildPoint(k-1,  GodEnum.Capture, pillarNum1)
						end
						SetTick(ClanGodMiracle_CurInfo[k].MapID, 0)
					end
				end

				if CalFlag == 1  then
					--  无争夺特殊处理
					if GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID) == 0 then

						-- 记录战斗结果
						Capture_Result[k].WinOrLoseFlag = 0
						Capture_Result[k].DefendGuild   = GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID)
						Capture_Result[k].CaptureGuild  = 0

						-- 设置处于空闲状态
						guild.SetGodCurState(k-1, 1)
						if GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID) == 0 then
							guild.SetGodCurState(k-1, 0)
						end

						-- 双方帮派解除战斗状态
						guild.UnsetGuildStatus(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)
						guild.UnsetGuildStatus(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)

						aux.WriteLog("begin:Cal(Capture==0 ):CalCaptureContributionAndExploit")
						--外层贡献和功勋计算
						guild.CalCaptureContributionAndExploit(k-1, GodEnum.Defend)
						aux.WriteLog("end:Cal(Capture==0 ):CalCaptureContributionAndExploit")

						-- 保住神迹,不添加任何BUFF
						-- 本帮派成功的捍卫了自己的神迹！
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 13, 1)
						msg.AddMsgEvent(MsgID, 1, 1430)
						msg.AddMsgEvent(MsgID, 5, ClanGodMiracle_CurInfo[k].GdMiracleID)
						msg.DispatchGuildMsgEvent(MsgID, GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))

						--ResetGodMiracle(k)

					elseif GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID) ~= 0 then

						if  GetScores(GetCurInfoMapID(k), GodEnum.Defend) >= GetScores(GetCurInfoMapID(k), GodEnum.Capture) then -- 守方胜

							-- 去掉敌我判断buff
							guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024801)
							guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024901)

							-- 去掉敌我判断buff
							guild.RemoveBuffFromAllGuildMember(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024801)
							guild.RemoveBuffFromAllGuildMember(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024901)

							aux.WriteLog("defend win: Remove Friendly or Enemy buff\r\n")

							-- 记录战斗结果
							Capture_Result[k].WinOrLoseFlag = 1
							Capture_Result[k].DefendGuild   = GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID)
							Capture_Result[k].CaptureGuild  = GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID)

							-- 设置处于空闲状态
							guild.SetGodCurState(k-1, 1)
							aux.WriteLog("defend win: SetGodCurState(k-1, 1)\r\n")

							-- 双方帮派解除战斗状态
							guild.UnsetGuildStatus(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)
							guild.UnsetGuildStatus(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)
							aux.WriteLog("defend win: UnsetGuildStatus\r\n")

							--外层贡献和功勋计算
							aux.WriteLog("begin:Cal(Defend Win):CalCaptureContributionAndExploit\r\n")
							guild.CalCaptureContributionAndExploit(k-1, GodEnum.Defend)
							aux.WriteLog("end:Cal(Defend Win):CalCaptureContributionAndExploit\r\n")

							-- 保住神迹,不添加任何BUFF
							-- 本帮派成功的捍卫了自己的神迹！
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 13, 1)
							msg.AddMsgEvent(MsgID, 1, 1430)
							msg.AddMsgEvent(MsgID, 5, ClanGodMiracle_CurInfo[k].GdMiracleID)
							msg.DispatchGuildMsgEvent(MsgID, GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))

							--ResetGodMiracle(k)

						elseif GetScores(GetCurInfoMapID(k), GodEnum.Defend) < GetScores(GetCurInfoMapID(k), GodEnum.Capture)  then -- 攻方胜

							-- 去掉敌我判断buff
							guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024801)
							guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024901)

							-- 去掉敌我判断buff
							guild.RemoveBuffFromAllGuildMember(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024801)
							guild.RemoveBuffFromAllGuildMember(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 2024901)

							aux.WriteLog("capture win: Remove Friendly or Enemy buff\r\n")

							-- 记录战斗结果
							Capture_Result[k].WinOrLoseFlag = 2
							Capture_Result[k].DefendGuild   = GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID)
							Capture_Result[k].CaptureGuild  = GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID)

							-- 设置处于空闲状态
							guild.SetGodCurState(k-1, 1)
							aux.WriteLog("capture win: SetGodCurState(k-1, 1)\r\n")

							-- 双方帮派解除战斗状态
							guild.UnsetGuildStatus(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)
							guild.UnsetGuildStatus(GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID), 8)
							aux.WriteLog("capture win: UnsetGuildStatus\r\n")

							--外层贡献和功勋计算
							aux.WriteLog("begin:Cal(Capture Win):CalCaptureContributionAndExploit\r\n")
							guild.CalCaptureContributionAndExploit(k-1, GodEnum.Capture)
							aux.WriteLog("end:Cal(Capture Win):CalCaptureContributionAndExploit\r\n")

							---------------------------------------------------------------------------------------------------- BUFF交换操作

							-- 丢失神迹，神佑降一级
							local DefendGodNum = 0
							DefendGodNum = GetGuildCaptureNum(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
							if DefendGodNum == 1 then
								--[[local tmpRoleIDTab = guild.GetAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
								if tmpRoleIDTab ~= nil then
									for k5, v in pairs(tmpRoleIDTab) do
										if tmpRoleIDTab[k5] ~= nil then
											unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k5], GetBuffIDbyGodMiracleNum(1))
										end
									end
								end--]]
								guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(1))
								aux.WriteLog("defend lose: 1 Remove buff\r\n")
							elseif DefendGodNum > 1 then

								-- 先移除高阶BUF
								--[[local tmpRoleIDTab = guild.GetAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
								if tmpRoleIDTab ~= nil then
									for k4, v in pairs(tmpRoleIDTab) do
										if tmpRoleIDTab[k4] ~= nil then
											unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k4], GetBuffIDbyGodMiracleNum(DefendGodNum))
											unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k4], GetBuffIDbyGodMiracleNum(DefendGodNum-1), tmpRoleIDTab[k4])
										end
									end
								end--]]

								guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(DefendGodNum))
								-- 增加低价BUF
								guild.AddBuffToAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(DefendGodNum - 1))
							end
							aux.WriteLog("defend lose: > 1 Remove buff\r\n")

							-- 初始数据
							SetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID, GetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID))
							SetCaptureGuildID(ClanGodMiracle_CurInfo[k].MapID, 0)

							-- 夺下神迹方,增加一级神迹
							local CaptureGodNum1 = 0
							CaptureGodNum1 = GetGuildCaptureNum(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
							if CaptureGodNum1  > 1 then
								-- 先移除低价BUFF
								--[[local tmpRoleIDTab = guild.GetAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
								if tmpRoleIDTab ~= nil then
									for k3, v in pairs(tmpRoleIDTab) do
										if (tmpRoleIDTab[k3] ~= nil) then
											unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k3], GetBuffIDbyGodMiracleNum(CaptureGodNum1-1))
											unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k3], GetBuffIDbyGodMiracleNum(CaptureGodNum1), tmpRoleIDTab[k3])
										end
									end
								end--]]

								guild.RemoveBuffFromAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(CaptureGodNum1-1))
								-- 增加高阶BUFF
								guild.AddBuffToAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(CaptureGodNum1))

								aux.WriteLog("capture win: > 1 Add buff\r\n")
							elseif CaptureGodNum1  == 1 then
								-- 先移除高阶BUF
								--[[local tmpRoleIDTab = guild.GetAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))
								if tmpRoleIDTab ~= nil then
									for k2, v in pairs(tmpRoleIDTab) do
										if (tmpRoleIDTab[k2] ~= nil) then
											unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, tmpRoleIDTab[k2], GetBuffIDbyGodMiracleNum(1), tmpRoleIDTab[k2])
										end
									end
								end--]]
								guild.AddBuffToAllGuildMember(GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID), GetBuffIDbyGodMiracleNum(1))
							end
							aux.WriteLog("capture win: 1 Add buff\r\n")


							-- 获取的神佑广播
							local MsgID1 = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID1, 13, 1)

							if CaptureGodNum == 1 then
								msg.AddMsgEvent(MsgID1, 1, 1421)
							elseif CaptureGodNum == 2 then
								msg.AddMsgEvent(MsgID1, 1, 1422)
							elseif CaptureGodNum == 3 then
								msg.AddMsgEvent(MsgID1, 1, 1423)
							elseif CaptureGodNum == 4 then
								msg.AddMsgEvent(MsgID1, 1, 1424)
							elseif CaptureGodNum == 5 then
								msg.AddMsgEvent(MsgID1, 1, 1435)
							end
							msg.AddMsgEvent(MsgID1, 5  , ClanGodMiracle_CurInfo[k].GdMiracleID)
							msg.AddMsgEvent(MsgID1, 19,  guildID)
							msg.DispatchWorldMsgEvent(MsgID1)

							---------------------------------------------------------------------------------------------------- BUFF交换操作

							-- 本帮通过艰苦的争夺获得了××神迹的占领权，所有成员获得神佑状态！同时大家可前往神迹购买所需物品
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 101, 1415)
							msg.AddMsgEvent(MsgID, 5, ClanGodMiracle_CurInfo[k].GdMiracleID)
							msg.DispatchGuildMsgEvent(MsgID, GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID))

							--ResetGodMiracle(k)

						end
					end

					-- 不论哪方胜,将柱子设置为绿色
					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998701)
					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998701)
					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998701)

					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998801)
					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998801)
					unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998801)

					if GetDefendGuildID(ClanGodMiracle_CurInfo[k].MapID) ~= 0  then

						unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998601)
						unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998601)
						unit.CancelBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998601)

						unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1, 9998901, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj1)
						unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2, 9998901, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj2)
						unit.AddBuff(ClanGodMiracle_CurInfo[k].MapID, 0, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3, 9998901, ClanGodMiracle_Proto[ClanGodMiracle_CurInfo[k].MapID].NPCIDObj3)

					end

					aux.WriteLog("Setting Pillar corlor\r\n")

					ResetGodMiracle(k)

					ClearBroaCastGlag()
					ResultbroadcastFlag = 1

				end
			--end
		end
	end

	if ResultbroadcastFlag == 1 then
		aux.WriteLog("Broadcast cal result!\r\n")
		CalFlag = 0
		FinishedFlag	= 1
		BroadcastCaptureResult()
		ResultbroadcastFlag  = 0
		EnterFlag = 0

	end
end

-- \breif : 活动每分钟刷新
--  param1: 活动ID
function GodMiracleCpature_OnActiveTimeMin(ActiveID)

	if OpenGodMiracleFlag == 0 then
		return
	end
	--  更新所有神迹,记录活跃时间
	guild.UpdateGodMiracleMin()

	BroadcastGodMsgMin()
end


-- \breif : 玩家被杀
--  param1:
function GodMiracleCpature_OnRoleDie(MapID, InstanceID, BeKilledID, KillerID)
    --葵灵宝树活动的相关脚本
	if unit.IsPlayer(BeKilledID) and not unit.IsPlayer(KillerID) then
	    local TypeID = cre.GetCreatureTypeID(MapID,InstanceID, KillerID)
		if TypeID ==  1535307 then
		    cre.MonsterSay(MapID, InstanceID, KillerID, 50081)
		    unit.AddBuff(MapID, InstanceID, KillerID, 9431401, KillerID)
            local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 3168)    --蚩尤妖魂杀掉了XXX！蚩尤妖魂变得更加凶残了！
			msg.AddMsgEvent(MsgID, 2, BeKilledID)
			msg.DispatchMapMsgEvent(MsgID, 3017299407, -1)
		end
	end

	if OpenGodMiracleFlag == 0 then
		return
	end

	-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return
	end

	-- 周二，周四，周六开启
	local day = os.date("%A")
	if day ~= "Tuesday" and day ~= "Thursday" and day ~= "Saturday" then
		return
	end


	-- 是否在神迹区域内
	local IsInArea = map.IsInArea(MapID, InstanceID, KillerID, GetAreaID(MapID))
	if IsInArea == 0 then
		return
	end

	-- 非争夺状态不做处理
	local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
	if godCurState ~= 2  and GetCaptureGuildID(MapID) ~= 0 then
		return
	end

	-- 得到杀人者帮派ID
	local killerGuildID = guild.GetRoleGuildID(KillerID)
	if killerGuildID ~= 4294967295 then
		local DOrCFlag = GetRoleGuildDefendOrCapture(MapID, killerGuildID)
			if DOrCFlag ~= GodEnum.Null then
				guild.SetGuildRoleDie(GetGodMiracleIDMinOne(MapID), KillerID, BeKilledID, DOrCFlag)
			end
	end

end



local EnterFlag = 0

-- \breif : 玩家进入神迹
--  param3: 进入脚本区域的玩家ID
--  param4: 进入的脚本区域ID
function GodMiracleCpature_OnRoleEnterArea(MapID, InstanceID, OwnerID, ObjAreaID)

	if OpenGodMiracleFlag == 0 then
		return
	end

	-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return
	end

	-- 周二，周四，周六开启
	local day = os.date("%A")
	if day ~= "Tuesday" and day ~= "Thursday" and day ~= "Saturday" then
		return
	end


	-- 是否在神迹区域内
	local IsInArea = map.IsInArea(MapID, InstanceID, OwnerID, GetAreaID(MapID))
	if IsInArea == false then
		return
	end

	-- 非争夺状态不做处理
	local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
	if godCurState ~= 2 or GetCaptureGuildID(MapID) == 0 then
		return
	end

	-- 设置玩家进入神迹区域，上层创建并神迹进入神迹区域标志
	if GetAreaID(MapID) == ObjAreaID then
		-- 在C++层创建玩家并设置玩家可以记录活跃时间
		local guildID = guild.GetRoleGuildID(OwnerID)
		if guildID ~= 4294967295 then
			local DOrCFlag = GetRoleGuildDefendOrCapture(MapID, guildID)
			if DOrCFlag ~= GodEnum.Null then

				-- 设置角色进入神迹标志
				guild.GuildRoleEnterGodArea(GetGodMiracleIDMinOne(MapID), OwnerID, DOrCFlag)

				-- 设置活跃时间
				guild.SetRoleStartActiveTimes(GetGodMiracleIDMinOne(MapID), OwnerID, DOrCFlag, 1)



				if GetDefendGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024801)
					unit.AddBuff(MapID, 0, OwnerID, 2024901, OwnerID)
				elseif GetCaptureGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024901)
					unit.AddBuff(MapID, 0, OwnerID, 2024801, OwnerID)
				end

				if EnterFlag == 0 then

				--  进入区域进行广播
				-- 您的帮派正在参与此神迹争夺战，敌友判断已变为争夺战模式！
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1416)
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)

				-- 关闭死亡保护
				role.UnSetRoleState(OwnerID, 1, 1)

				EnterFlag =  1

				end
			end
		end
	end

end

-- \breif : 玩家离开神迹区域
--  param3: 离开脚本区域的玩家ID
--  param4: 离开的脚本区域ID
function GodMiracleCpature_OnRoleLeaveArea(MapID, InstanceID, OwnerID, ObjAreaID)

	if OpenGodMiracleFlag == 0 then
		return
	end

	-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return
	end

	-- 周二，周四，周六开启
	local day = os.date("%A")
	if day ~= "Tuesday" and day ~= "Thursday" and day ~= "Saturday" then
		return
	end


	-- 非争夺状态不做处理
	local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
	if godCurState ~= 2  or  GetCaptureGuildID(MapID) == 0 then
		return
	end

	-- 设置玩家离开神迹区域
	if GetAreaID(MapID) == ObjAreaID then
		local guildID = guild.GetRoleGuildID(OwnerID)
		if guildID ~= 4294967295 then

			local DOrCFlag = GetRoleGuildDefendOrCapture(MapID, guildID)
			if DOrCFlag ~= GodEnum.Null then

				guild.SetRoleStartActiveTimes(GetGodMiracleIDMinOne(MapID), OwnerID, DOrCFlag, 0)

				-- 取消玩家颜色区别BUFF
				--if GetDefendGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024901)
				--elseif GetCaptureGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024801)
				--end

				--您已离开神迹区域或争夺战已结束，敌友判断已变为通常模式！
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1417)
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)--]]

				--通知客户端
				guild.RoleLeaveArea(GetGodMiracleIDMinOne(MapID), OwnerID, GetRoleGuildDefendOrCapture(MapID, guildID))

				EnterFlag = 0
			end
		end
	else
		local guildID = guild.GetRoleGuildID(OwnerID)
		if guildID ~= 4294967295 then
			if guildID == GetDefendGuildID(MapID) or guildID == GetCaptureGuildID(MapID) then

				guild.SetRoleStartActiveTimes(GetGodMiracleIDMinOne(MapID), OwnerID, DOrCFlag, 0)

				-- 取消玩家颜色区别BUFF
				--if GetDefendGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024901)
				--elseif GetCaptureGuildID(MapID) == guildID then
					unit.CancelBuff(MapID, 0, OwnerID, 2024801)
				--end

				--您已离开神迹区域或争夺战已结束，敌友判断已变为通常模式！
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1417)
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)--]]

				--通知客户端
				guild.RoleLeaveArea(GetGodMiracleIDMinOne(MapID), OwnerID, GetRoleGuildDefendOrCapture(MapID, guildID))

				EnterFlag = 0
			end
		end
	end
end

-- \breif : 检测指定技能是否能被释放
--  param3: SkillID(技能ID)
--  param4: 释放技能者ID
--  param5: 目标ID
--  return: 错误标示信息以脚本信息发给客户端,本函数只要不返回0就为错误,c++层则不做处理
function GodMiracleCpature_CanCast(MapID, InstanceID, SkillID, OwnerID, TargetID)

	-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return
	end

	-- 周二，周四，周六开启
	local day = os.date("%A")
	if day ~= "Tuesday" and day ~= "Thursday" and day ~= "Saturday" then
		return
	end


	-- 神迹在空闲状态,不允许开启
	local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
	if godCurState  == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID,26, 1427)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end


	-- 获取帮派ID(eroor:接口函数应该在没有帮派的情况下返回0不应该返回无限大数值)
	local GuildID
	GuildID = guild.GetRoleGuildID(OwnerID);
	-- 获取玩家帮派职位(GuildPos==nil 则帮派不存在)
	local GuildPos = -1
	GuildPos = guild.GetRoleGuildPos(GuildID , OwnerID);
	if GuildPos == nil then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID,26, 1419)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	-- 玩家没有开启行凶状态,不可开启
		-- 您尚未开启行凶状态，无法进行开启神迹操作！
	if role.IsRoleInStatus(OwnerID, 4) == false and role.IsRoleInStatus(OwnerID, 1048576) == false then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1403)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	if godCurState  == 2 and GetDefendGuildID(MapID) ~= 0 then
		-- 玩家不在争夺帮派范围!
		if GetDefendGuildID(MapID) ~= GuildID and GetCaptureGuildID(MapID) ~= GuildID then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1429)
			msg.DispatchRoleMsgEvent(OwnerID, MsgID)

			return 999
		end
	end

	-- 未参战，无人争夺，不用开启
	if godCurState  == 2 and GetDefendGuildID(MapID) ~= 0 and GetCaptureGuildID(MapID) == 0 then
		return 999
	end

	-- 神迹柱已属于本帮派，不用开启
	local TargetTypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
	local NpcIndex = GetGodStackeIndex(MapID, TargetTypeID)
	if NpcIndex == GodEnum.Null then return 999 end
	local guildID1 = GetCapGodStacke(MapID, NpcIndex)
	if guildID1 ~= 0 and guildID1 == GuildID then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1425)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 999
	end

	--等级不够不可开启
	local OwnerLevel = role.GetRoleLevel(MapID, InstanceID, OwnerID)
	if OwnerLevel < 30 then
		return
	end

	-- 增加开启效果
	unit.AddBuff(MapID, 0, TargetID, 9998501, TargetID)
end

-- \breif : 技能释放作用效果
--  param3: SkillID(技能ID)
--  param4: 释放技能者ID
--  return: 1:通知上层不要作用此技能效果 0:则相反
function GodMiracleCpature_CastSkill(MapID, InstanceID, SkillID, OwnerID, TargetID, TargetTypeID)

	-- 地图限制判断
	if IsInGodAreaMap(MapID) == 0 then
		return
	end

	-- 得到占领柱那更柱子
	local GodStackeIndex = GetGodStackeIndex(MapID, TargetTypeID)
	if GodStackeIndex == GodEnum.Null then return 1 end

	-- 玩家帮派解散,不可开启
	local guildID = 0
	guildID = guild.GetRoleGuildID(OwnerID)
	local GuildPos
	GuildPos  = guild.GetRoleGuildPos(guildID, OwnerID);
	if GuildPos == nil then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1404)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 1
	end


	-- 玩家没有开启行凶状态,不可开启
	if role.IsRoleInStatus(OwnerID, 1) == true then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1406)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 1
	end
	if role.IsRoleInStatus(OwnerID, 4) == false and role.IsRoleInStatus(OwnerID, 1048576) == false then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1406)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 1
	end

	-- 神迹柱已属于本帮派，不用开启
	local guildID1 = GetCapGodStacke(MapID, TargetTypeID)
	if guildID1 ~= 0 and guildID1  == guildID then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1415)
		msg.DispatchRoleMsgEvent(OwnerID, MsgID)
		return 1
	end

	--等级不够不可开启
	local OwnerLevel = role.GetRoleLevel(MapID, InstanceID, OwnerID)
	if OwnerLevel < 30 then
		return
	end


	-- 状态0和1是用来处理第一次
	local godCurState = guild.GetGodMiracleCurState(GetGodMiracleIDMinOne(MapID))
	if godCurState  == 0 or (godCurState == 2 and GetDefendGuildID(MapID) == 0 ) then
		-- 设置帮派获取神迹柱
		SetCapGodStacke(MapID, GodStackeIndex, guildID)

		-- 取消柱子效果BUFF
		unit.CancelBuff(MapID, 0, TargetID, 9998501)

		-- 移除银色效果
		unit.CancelBuff(MapID, 0, TargetID, 9998601)

		-- 增加是攻方效果
		unit.AddBuff(MapID, 0, TargetID, 9998701, TargetID)

		-- 神迹区域范围内广播
		-- ×××××××（帮派名称）帮派的×××××××（玩家名称）已开启了×××××（结柱名称）！
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1415)
		msg.AddMsgEvent(MsgID, 19, guildID)
		msg.AddMsgEvent(MsgID, 2, OwnerID)
		msg.AddMsgEvent(MsgID, 5, TargetTypeID)
		msg.DispatchWorldMsgEvent(MsgID)

		local piller = GuildCapturePillar(MapID, guildID)
		if piller == 2 then
			-- 设置c++神迹占领帮派
			guild.SetCaptureGodGuildID(GetGodMiracleIDMinOne(MapID), guildID)


			if godCurState == 0 then
				-- 设置处于空闲状态
				guild.SetGodCurState(GetGodMiracleIDMinOne(MapID), 1)
			end

			ClearGodMiracle(ClanGodMiracle_Proto[MapID].ClanGodMiracleID)

			--设置lua此帮派占领神迹
			SetDefendGuildID(MapID, guildID)


			-- 获取神迹.神佑增加一级
			local DefendGodNum = 0
			DefendGodNum = GetGuildCaptureNum(guildID)
			if 	DefendGodNum == 1 then
				guild.AddBuffToAllGuildMember(guildID, GetBuffIDbyGodMiracleNum(1))
			elseif DefendGodNum > 1 then
				-- 先移除高阶BUF
				--[[local tmpRoleIDTab = guild.GetAllGuildMember(guildID)
				if tmpRoleIDTab ~= nil then
					for k, v in pairs(tmpRoleIDTab) do
						if (tmpRoleIDTab[k] ~= nil) then
							unit.CancelBuff(MapID, 0, tmpRoleIDTab[k], GetBuffIDbyGodMiracleNum(DefendGodNum-1))
						end
					end
				end--]]
				guild.RemoveBuffFromAllGuildMember(guildID, GetBuffIDbyGodMiracleNum(DefendGodNum-1))
				guild.AddBuffToAllGuildMember(guildID, GetBuffIDbyGodMiracleNum(DefendGodNum))
			end

			-- ###（神迹名称）被####（帮派名称）占领！！帮派中所有成员获得了####（神迹状态名称）的状态奖励！！
			local MsgID1 = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID1, 13, 1)

			if DefendGodNum == 1 then
				msg.AddMsgEvent(MsgID1, 1, 1421)
			elseif DefendGodNum == 2 then
				msg.AddMsgEvent(MsgID1, 1, 1422)
			elseif DefendGodNum == 3 then
				msg.AddMsgEvent(MsgID1, 1, 1423)
			elseif DefendGodNum == 4 then
				msg.AddMsgEvent(MsgID1, 1, 1424)
			elseif DefendGodNum == 5 then
				msg.AddMsgEvent(MsgID1, 1, 1435)
			end
			msg.AddMsgEvent(MsgID1, 5  , ClanGodMiracle_CurInfo[ClanGodMiracle_Proto[MapID].ClanGodMiracleID].GdMiracleID)
			msg.AddMsgEvent(MsgID1, 19,  guildID)
			msg.DispatchWorldMsgEvent(MsgID1)

			-- 帮派内大字体广播
			-- 本帮通过艰苦的争夺获得了××神迹的占领权，所有成员获得神佑状态！同时大家可前往神迹购买所需物品！
			local MsgID2 = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID2, 101, 1414)
			msg.AddMsgEvent(MsgID2, 5, ClanGodMiracle_CurInfo[ClanGodMiracle_Proto[MapID].ClanGodMiracleID].GdMiracleID)
			msg.DispatchGuildMsgEvent(MsgID2, guildID)


			-- 第一次占领神迹并进入空闲状态,神迹柱变绿
			--ChangePPillarColor(MapID, 9998801)
			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj1, 9998601)
			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj2, 9998601)
			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj3, 9998601)

			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj1, 9998701)
			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj2, 9998701)
			unit.CancelBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj3, 9998701)

			unit.AddBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj1, 9998901, ClanGodMiracle_Proto[MapID].NPCIDObj1)
			unit.AddBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj2, 9998901, ClanGodMiracle_Proto[MapID].NPCIDObj2)
			unit.AddBuff(MapID, 0, ClanGodMiracle_Proto[MapID].NPCIDObj3, 9998901, ClanGodMiracle_Proto[MapID].NPCIDObj3)

			ResetGodMiracle(ClanGodMiracle_Proto[MapID].ClanGodMiracleID)

		end

	elseif godCurState == 2 then

		-- 争夺时间到,不可开启
		local Hour = tonumber(os.date("%H"))
		local Min  = tonumber(os.date("%M"))
		if Hour == 20 and Min >= 30  then
			return
		end

		-- 设置帮派获取神迹柱
		SetCapGodStacke(MapID, GodStackeIndex, guildID)

		-- 神迹区域范围内广播
		-- ×××××××（帮派名称）帮派的×××××××（玩家名称）已开启了×××××（结柱名称）！
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1415)
		msg.AddMsgEvent(MsgID, 19, guildID)
		msg.AddMsgEvent(MsgID, 2, OwnerID)
		msg.AddMsgEvent(MsgID, 5, TargetTypeID)
		msg.DispatchGuildMsgEvent(MsgID, GetCaptureGuildID(MapID))

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1415)
		msg.AddMsgEvent(MsgID, 19, guildID)
		msg.AddMsgEvent(MsgID, 2, OwnerID)
		msg.AddMsgEvent(MsgID, 5, TargetTypeID)
		msg.DispatchGuildMsgEvent(MsgID, GetDefendGuildID(MapID))


		-- 取消柱子效果BUFF
		unit.CancelBuff(MapID, 0, TargetID, 9998501)

		-- 依据攻方或守方变化柱子的颜色
		if guildID ==GetDefendGuildID(MapID) then
			unit.CancelBuff(MapID, 0, TargetID, 9998601)
			unit.CancelBuff(MapID, 0, TargetID, 9998901)

			unit.CancelBuff(MapID, 0, TargetID, 9998701)
			unit.AddBuff(MapID, 0, TargetID, 9998801, TargetID)
		elseif guildID == GetCaptureGuildID(MapID) then
			unit.CancelBuff(MapID, 0, TargetID, 9998601)
			unit.CancelBuff(MapID, 0, TargetID, 9998901)

			unit.CancelBuff(MapID, 0, TargetID, 9998801)
			unit.AddBuff(MapID, 0, TargetID, 9998701, TargetID)
		end

	end

	return 0
end

--- breif: 对具有传送功能的道具做限制
function GodMiracleCapture_ItemCanUse(MapID, InstanceID, TypeID, TargetID)

	local Hour = tonumber(os.date("%H"))
	local Min  = tonumber(os.date("%M"))
	local Sec   = tonumber(os.date("%S"))

	if Hour == 20 and (Min <= 30 and Min >= 0)then
		-- 玩家在神迹争夺范围内
		local guildID = 0
		guildID = guild.GetRoleGuildID(TargetID)
		local GuildPos
		GuildPos  = guild.GetRoleGuildPos(guildID, TargetID);
		if GuildPos ~= nil then
			if GetDefendGuildID(MapID) == guildID or GetCaptureGuildID(MapID) == guildID then
				--是否在神迹争夺区域内
				local IsInArea = map.IsInArea(MapID, InstanceID, TargetID, GetAreaID(MapID))
				if IsInArea == 0 then
					return 0, 0
				else
					return -1, 1
				end
			end
		end
	end
	return 0, 0
end

------------------------------------------------------------------------------
-- register

-- 向活动注册初始化和适时更新操作
aux.RegisterActEvent(100, 1, "GodMiracleCpature_OnActiveTimer")
aux.RegisterActEvent(100, 2, "GodMiracleCpature_OnActiveStart")
aux.RegisterActEvent(100, 4, "GodMiracleCpature_OnActiveTimeMin")

-- map
aux.RegisterMapEvent("m03", 5, "GodMiracleCpature_OnRoleDie")
aux.RegisterMapEvent("m04", 5, "GodMiracleCpature_OnRoleDie")
aux.RegisterMapEvent("m05", 5, "GodMiracleCpature_OnRoleDie")
aux.RegisterMapEvent("m06", 5, "GodMiracleCpature_OnRoleDie")
aux.RegisterMapEvent("m07", 5, "GodMiracleCpature_OnRoleDie")

aux.RegisterMapEvent("m03", 7, "GodMiracleCpature_OnRoleEnterArea")
aux.RegisterMapEvent("m04", 7, "GodMiracleCpature_OnRoleEnterArea")
aux.RegisterMapEvent("m05", 7, "GodMiracleCpature_OnRoleEnterArea")
aux.RegisterMapEvent("m06", 7, "GodMiracleCpature_OnRoleEnterArea")
aux.RegisterMapEvent("m07", 7, "GodMiracleCpature_OnRoleEnterArea")

aux.RegisterMapEvent("m03", 23, "GodMiracleCpature_OnRoleLeaveArea")
aux.RegisterMapEvent("m04", 23, "GodMiracleCpature_OnRoleLeaveArea")
aux.RegisterMapEvent("m05", 23, "GodMiracleCpature_OnRoleLeaveArea")
aux.RegisterMapEvent("m06", 23, "GodMiracleCpature_OnRoleLeaveArea")
aux.RegisterMapEvent("m07", 23, "GodMiracleCpature_OnRoleLeaveArea")


-- skill
-- 调查技能注册检测和使用事件务
aux.RegisterSkillEvent(9001701, 0, "GodMiracleCpature_CanCast")
aux.RegisterSkillEvent(9001701, 2, "GodMiracleCpature_CastSkill")

-- 使用丈地云幡|丈地神符|丈地神符|万里追踪
aux.RegisterItemEvent(1310001, 0, "GodMiracleCapture_ItemCanUse")
aux.RegisterItemEvent(1360003, 0, "GodMiracleCapture_ItemCanUse")
aux.RegisterItemEvent(1360102, 0, "GodMiracleCapture_ItemCanUse")
aux.RegisterItemEvent(1360110, 0, "GodMiracleCapture_ItemCanUse")



