

--与玫瑰灵壤对话，玫瑰土
function RoseTu_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local seedowner = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1)--种植者ID
	local growtime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2)--已浇水次数
	local HaveDelete_growthitem = 0
--1.5新增内容////////////////////////////////////////////////up,所有材料▼▼▼▼▼▼
	local Role = {}
	local seedownerRole = {}
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local seedownerTeamID = role.IsRoleHaveTeam(MapID, InstanceID, seedowner)
	local IsInOwnerTeam = 0
	if TeamID ~= 4294967295 then
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for k = 1,6 do
			if seedowner == Role[k] then
				IsInOwnerTeam = 1
			end
		end
	end
	if seedownerTeamID ~= 4294967295 then
		seedownerRole[1], seedownerRole[2], seedownerRole[3], seedownerRole[4], seedownerRole[5], seedownerRole[6] = role.GetRoleTeamMemID(seedownerTeamID)
	end
--1.5新增内容/////////////////////////////////////////////////down,所有材料▲▲▲▲▲▲▲▲
	local farmer_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if farmer_level > 120 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1344)	-- "超过120级的玩家将无法参与种植！"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	else
		if TalkIndex == -1 then
			if seedowner ~= 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1333)	-- "###(玩家名)所种的玫瑰灵壤,灵种刚刚开始发芽，多多浇水可促进其生长！"
				msg.AddMsgEvent(MsgID, 2, seedowner)	--传入玩家名
				msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1302)	-- "★浇水"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1334)	-- "玫瑰灵壤刚刚开始发芽，多多浇水可促进其生长！\n它还没有主人，只要您浇过一次水即为它的新主人"
				msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1302)	-- "★浇水"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
			end

		elseif TalkIndex == 4 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1304)	-- "浇水需要消耗1份【生命之水】或1份【生命灵水】，两次浇水之间需间隔1分钟；\n【生命灵水】为仙家圣水，其功效是普通【生命之水】的5倍！"
				msg.AddMsgEvent(MsgID, 21, 6)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1305)	-- "★浇【生命之水】"
				msg.AddMsgEvent(MsgID, 21, 7)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1306)	-- "★浇【生命灵水】"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
				--1.5新增内容//////////////////////////////////////////////////////////////////////////////up，所有材料▼▼▼▼▼▼
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				if seedowner ~= 0 and seedowner ~= RoleID and IsInOwnerTeam == 0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 1342) --"位于XX（地图名）[X,Z]的植物正被[RoleID]觊觎，请前往应对。"
					msg.AddMsgEvent(MsgID, 6, MapID)
					msg.AddMsgEvent(MsgID, 9, x)
					msg.AddMsgEvent(MsgID, 9, z)
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.DispatchRoleMsgEvent(seedowner, MsgID)
					if seedownerTeamID ~= 4294967295 then
						for k in pairs(seedownerRole) do
							if seedownerRole[k] ~= 4294967295 then
								local MsgID = msg.BeginMsgEvent()
								msg.AddMsgEvent(MsgID, 101, 1342) --"位于XX（地图名）[X,Z]的植物正被[RoleID]觊觎，请前往应对。"
								msg.AddMsgEvent(MsgID, 6, MapID)
								msg.AddMsgEvent(MsgID, 9, x)
								msg.AddMsgEvent(MsgID, 9, z)
								msg.AddMsgEvent(MsgID, 2, RoleID)
								msg.DispatchRoleMsgEvent(seedownerRole[k], MsgID)
							end
						end
					end
				end
				--1.5新增内容///////////////////////////////////////////////////////////////////////////////down，所有材料▲▲▲▲▲▲▲
		elseif TalkIndex == 6 then
				local WaterNum = role.GetRoleItemNum(RoleID, 3303009)--生命之水数量
				if WaterNum > 0 then
					local cTime = os.time() --当前时间
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --上次浇水距现在时间

					if 	s > 60 then --间隔大于1分钟
	--1.5新增内容////////////////////////////////////////////////////////////////////////////uuuuuuuupppppppppppppp，低级材料▼▼▼▼▼▼

						if seedowner == 0 or seedowner == RoleID or IsInOwnerTeam == 1 then --种子无所属者或者为所属或者所属在队伍中
							role.RemoveFromRole(MapID, InstanceID, RoleID, 3303009, 1, 101)---------删除生命之水
							growtime = growtime + 1	-------------------------记录操作次数
							cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3, cTime)-----------高级材料不记录
							HaveDelete_growthitem = 1
							if TeamID == 4294967295 then
								local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
								local roleexp = 100*growth_exp[rolelevel].gaojipt-------------根据植物获得经验
								role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
								unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
							else
								local aroundmember = 0
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												aroundmember = aroundmember + 1
											end
										end
									end
								end
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												local rolelevel = role.GetRoleLevel(Role_MapID, Role_InstanceID, Role[k])
												local roleexp = 100*growth_exp[rolelevel].gaojipt-------------根据植物获得经验
												if aroundmember == 1 then
													roleexp = roleexp
												else
													roleexp = roleexp/aroundmember *1.2
												end
												role.AddRoleExp(MapID, InstanceID, Role[k], roleexp)
												unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
											end
										end
									end
								end
							end
						else --抢树，不在所属者队中
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
							msg.AddMsgEvent(MsgID, 21, 8)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
							msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
							msg.AddMsgEvent(MsgID, 24, TargetID)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						end
	--1.5新增内容////////////////////////////////////////////////////////////////////dddddooooooowwwwwwnnnnnn，低级材料▲▲▲▲▲
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1307)-- "对该植物的两次浇水间隔不足1分钟，为防止频繁浇水对植物造成的伤害，请您稍等片刻再来浇水。"
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
						msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				else

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1308) --"您没有生命之水。\n生命之水可在种植商人处进行购买，或通过完成大农丞处的“年货大采购”任务获得。"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
				TalkIndex = -1
		elseif TalkIndex == 7 then
				local WaterNum = role.GetRoleItemNum(RoleID, 3303010)--生命灵水数量
				if WaterNum > 0 then
					local cTime = os.time() --当前时间
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --上次浇水距现在时间

					if 	s > 60 then --间隔大于1分钟
	--1.5新增内容////////////////////////////////////////////////////////////////////////////uuuuuuuupppppppppppppp，高级材料▼▼▼▼▼▼
						if seedowner == 0 or seedowner == RoleID or IsInOwnerTeam == 1 then --种子无所属者或者为所属或者所属在队伍中
							role.RemoveFromRole(MapID, InstanceID, RoleID, 3303010, 1, 101)---------删除生命灵水
							growtime = 5	-------------------------记录操作次数
							HaveDelete_growthitem = 1
							if TeamID == 4294967295 then
								local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
								local roleexp = 100*growth_exp[rolelevel].gaojiim-------------根据植物获得经验
								role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
								unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
							else
								local aroundmember = 0
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												aroundmember = aroundmember + 1
											end
										end
									end
								end
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												local rolelevel = role.GetRoleLevel(Role_MapID, Role_InstanceID, Role[k])
												local roleexp = 100*growth_exp[rolelevel].gaojiim-------------根据植物获得经验
												if aroundmember == 1 then
													roleexp = roleexp
												else
													roleexp = roleexp/aroundmember *1.2
												end
												role.AddRoleExp(MapID, InstanceID, Role[k], roleexp)
												unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
											end
										end
									end
								end
							end
						else --抢树，不在所属者队中
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
							msg.AddMsgEvent(MsgID, 21, 9)	-- 选项一-------------------------------根据材料填，此处高级9
							msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
							msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
							msg.AddMsgEvent(MsgID, 24, TargetID)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						end
	--1.5新增内容////////////////////////////////////////////////////////////////////dddddooooooowwwwwwnnnnnn，高级材料▲▲▲▲▲
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1307)	-- "对该植物的两次浇水间隔不足1分钟，为防止频繁浇水对植物造成的伤害，请您稍等片刻再来浇水。"
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
						msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1309) --"您没有生命灵水。\n生命灵水乃是仙家圣水，可遇而不可求，听说在大农丞处完成“年货大采购”任务将有较小几率获得生命灵水，另外开启“金虎如意匣”或“新春红包”等均有一定几率获得生命灵水。\n或者您可以尝试使用普通生命之水对该植物进行浇水"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
				TalkIndex = -1
	--///////.///////////////////////////////////////////////////////////////////////////////////////1.5up,低级材料▼▼▼▼▼▼
		elseif TalkIndex == 8 then
			local Temp = 1
			if role.IsRoleInStatus(RoleID, 1048576) == false then --必须开启自由才可以抢树。
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				Temp = 0
			end
			if Temp == 1 then
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303009, 1, 101)--------------删除生命之水
				growtime = growtime + 1-------------------------记录操作次数
				local cTime = os.time() --当前时间
				local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3, cTime)------------高级材料不记录
				HaveDelete_growthitem = 1
				local roleexp = 100*growth_exp[rolelevel].gaojipt-----------------根据植物给经验
				role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
				unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
				unit.AddBuff(MapID, InstanceID, RoleID, 2025601, RoleID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1343) --"此植物不属于你或你的队友，因此你受到了虚弱效果的影响。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
	--/////////////////////////////////////////////////////////////////////////////////////////////////1.5down，低级材料▲▲▲▲▲
	--///////.///////////////////////////////////////////////////////////////////////////////////////1.5up,高级材料▼▼▼▼▼▼
		elseif TalkIndex == 9 then
			local Temp = 1
			if role.IsRoleInStatus(RoleID, 1048576) == false then --必须开启自由才可以抢树。
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				Temp = 0
			end
			if Temp == 1 then
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303010, 1, 101)--------------删除生命灵水
				growtime = 5-------------------------记录操作次数
				HaveDelete_growthitem = 1
				local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				local roleexp = 100*growth_exp[rolelevel].gaojiim-----------------根据植物给经验
				role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
				unit.AddBuff(MapID, InstanceID, TargetID, 5003801, TargetID)
				unit.AddBuff(MapID, InstanceID, RoleID, 2025601, RoleID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1343) --"此植物不属于你或你的队友，因此你受到了虚弱效果的影响。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
	--/////////////////////////////////////////////////////////////////////////////////////////////////1.5down，高级材料▲▲▲▲▲
		end
	end


	if 	HaveDelete_growthitem == 1 then
			--增加进阶实力值
			Increase_shili(MapID, InstanceID, RoleID, 0, 1,"jinjieshili_A")

		if growtime == 5 then
			local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
			local creatureID = map.MapCreateCreature(MapID, InstanceID, 4900415, x, y, z)
			if seedowner == 0 then
			seedowner = RoleID
			end
			cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 1, seedowner)--记录种植出【玫瑰灵树】玩家的ID

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, 1321)--您所种植物经过几番浇水开始茁壮成长，距离最终收获已经不远，恭喜恭喜！
			msg.DispatchRoleMsgEvent(seedowner, MsgID)

			map.MapDeleteCreature(MapID, InstanceID, TargetID)
		else
			if seedowner == 0 then
			cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1, RoleID)
			end
			cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2, growtime)--记录此土堆的浇水次数
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1310)	-- "使当前植物生长还需用【生命之水】浇灌XXX次，或者直接使用【生命灵水】浇灌一次。"
			msg.AddMsgEvent(MsgID, 9, 5-growtime)	--传入还可浇灌的次数
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end

	end
end

aux.RegisterCreatureEvent(4900414, 7, "RoseTu_OnTalk")






--与玫瑰灵树对话，玫瑰树
function RoseShu_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local seedowner = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1)--种植者ID
	local growtime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2)--已浇水次数
	local HaveDelete_growthitem = 0
--1.5新增内容////////////////////////////////////////////////up,所有材料▼▼▼▼▼▼
	local Role = {}
	local seedownerRole = {}
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local seedownerTeamID = role.IsRoleHaveTeam(MapID, InstanceID, seedowner)
	local IsInOwnerTeam = 0
	if TeamID ~= 4294967295 then
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for k = 1,6 do
			if seedowner == Role[k] then
				IsInOwnerTeam = 1
			end
		end
	end
	if seedownerTeamID ~= 4294967295 then
		seedownerRole[1], seedownerRole[2], seedownerRole[3], seedownerRole[4], seedownerRole[5], seedownerRole[6] = role.GetRoleTeamMemID(seedownerTeamID)
	end

--1.5新增内容/////////////////////////////////////////////////down,所有材料▲▲▲▲▲▲▲▲
	local farmer_level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if farmer_level > 120 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1344)	-- "超过120级的玩家将无法参与种植！"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	else
		if TalkIndex == -1 then
			if seedowner ~= 0 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1335)	-- "###(玩家名)所种的###(植物名),幼苗开始茁壮成长，请多多施肥保证养分！"
				msg.AddMsgEvent(MsgID, 2, seedowner)	--传入玩家名
				msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1312)	-- "★施肥"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1336)	-- "玫瑰灵树刚刚开始茁壮成长，请多多施肥保证养分！\n它还没有主人，只要您浇过一次水即为它的新主人"
				msg.AddMsgEvent(MsgID, 21, 4)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1312)	-- "★施肥"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
			end

		elseif TalkIndex == 4 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1313)-- "施肥需要消耗1份【培土农肥】或1份【培土灵肥】，两次施肥之间需间隔1分钟；\n【培土灵肥】功效是普通【培土农肥】的5倍！"
				msg.AddMsgEvent(MsgID, 21, 6)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1314)	-- "★施【培土农肥】"
				msg.AddMsgEvent(MsgID, 21, 7)	-- 选项二
				msg.AddMsgEvent(MsgID, 1, 1315)	-- "★施【培土灵肥】"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				TalkIndex = -1
				--1.5新增内容//////////////////////////////////////////////////////////////////////////////up，所有材料▼▼▼▼▼▼
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				if seedowner ~= 0 and seedowner ~= RoleID and IsInOwnerTeam == 0 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 1342) --"位于XX（地图名）[X,Z]的植物正被[RoleID]觊觎，请前往应对。"
					msg.AddMsgEvent(MsgID, 6, MapID)
					msg.AddMsgEvent(MsgID, 9, x)
					msg.AddMsgEvent(MsgID, 9, z)
					msg.AddMsgEvent(MsgID, 2, RoleID)
					msg.DispatchRoleMsgEvent(seedowner, MsgID)
					if seedownerTeamID ~= 4294967295 then
						for k in pairs(seedownerRole) do
							if seedownerRole[k] ~= 4294967295 then
								local MsgID = msg.BeginMsgEvent()
								msg.AddMsgEvent(MsgID, 101, 1342) --"位于XX（地图名）[X,Z]的植物正被[RoleID]觊觎，请前往应对。"
								msg.AddMsgEvent(MsgID, 6, MapID)
								msg.AddMsgEvent(MsgID, 9, x)
								msg.AddMsgEvent(MsgID, 9, z)
								msg.AddMsgEvent(MsgID, 2, RoleID)
								msg.DispatchRoleMsgEvent(seedownerRole[k], MsgID)
							end
						end
					end
				end
				--1.5新增内容///////////////////////////////////////////////////////////////////////////////down，所有材料▲▲▲▲▲▲▲
		elseif TalkIndex == 6 then
				local ManureNum = role.GetRoleItemNum(RoleID, 3303011)--培土农肥数量
				if ManureNum > 0 then
					local cTime = os.time() --当前时间
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --上次施肥距现在时间

					if 	s > 60 then --间隔大于1分钟
	--1.5新增内容////////////////////////////////////////////////////////////////////////////uuuuuuuupppppppppppppp，低级材料▼▼▼▼▼▼
						if seedowner == 0 or seedowner == RoleID or IsInOwnerTeam == 1 then --种子无所属者或者为所属或者所属在队伍中
							role.RemoveFromRole(MapID, InstanceID, RoleID, 3303011, 1, 101)---------删除普通肥
							growtime = growtime + 1	-------------------------记录操作次数
							cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3, cTime)-----------高级材料不记录时间
							HaveDelete_growthitem = 1
							if TeamID == 4294967295 then
								local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
								local roleexp = 100*growth_exp[rolelevel].gaojipt-------------根据植物获得经验
								role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
								unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
							else
								local aroundmember = 0
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												aroundmember = aroundmember + 1
											end
										end
									end
								end
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												local rolelevel = role.GetRoleLevel(Role_MapID, Role_InstanceID, Role[k])
												local roleexp = 100*growth_exp[rolelevel].gaojipt-------------根据植物获得经验
												if aroundmember == 1 then
													roleexp = roleexp
												else
													roleexp = roleexp/aroundmember *1.2
												end
												role.AddRoleExp(MapID, InstanceID, Role[k], roleexp)
												unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
											end
										end
									end
								end
							end
						else --抢树，不在所属者队中
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
							msg.AddMsgEvent(MsgID, 21, 8)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
							msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
							msg.AddMsgEvent(MsgID, 24, TargetID)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						end
	--1.5新增内容////////////////////////////////////////////////////////////////////dddddooooooowwwwwwnnnnnn，低级材料▲▲▲▲▲

					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1316)-- "对该植物的两次施肥间隔不足1分钟，为防止频繁施肥对植物造成的伤害，请您稍等片刻再来施肥。"
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
						msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				else

					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1317) --"您没有培土农肥。\n培土农肥可在种植商人处进行购买，或通过完成大农丞处的“年货大采购”任务获得。"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
				TalkIndex = -1
		elseif TalkIndex == 7 then
				local ManureNum = role.GetRoleItemNum(RoleID, 3303012)--培土灵肥数量
				if ManureNum > 0 then
					local cTime = os.time() --当前时间
					local lTime = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3)
					local s = os.difftime(cTime,lTime) --上次施肥距现在时间

					if 	s > 60 then --间隔大于1分钟
	--1.5新增内容////////////////////////////////////////////////////////////////////////////uuuuuuuupppppppppppppp，高级材料▼▼▼▼▼▼
						if seedowner == 0 or seedowner == RoleID or IsInOwnerTeam == 1 then --种子无所属者或者为所属或者所属在队伍中
							role.RemoveFromRole(MapID, InstanceID, RoleID, 3303012, 1, 101)---------删除高级肥
							growtime = 5	-------------------------记录操作次数
							HaveDelete_growthitem = 1
							if TeamID == 4294967295 then
								local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
								local roleexp = 100*growth_exp[rolelevel].gaojiim-------------根据植物获得经验
								role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
								unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
							else
								local aroundmember = 0
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												aroundmember = aroundmember + 1
											end
										end
									end
								end
								for k = 1,6 do
									if 4294967295 ~= Role[k] then
										local Role_MapID, Role_InstanceID = role.GetRoleMapID(Role[k])
										if Role_MapID == MapID or Role_InstanceID == InstanceID then
											local x1, y1, z1 = unit.GetPosition(MapID, InstanceID, TargetID)
											local x2, y2, z2 = unit.GetPosition(Role_MapID, Role_InstanceID, Role[k])
											if math.abs(x1-x2)<200 and math.abs(z1-z2)<200 then
												local rolelevel = role.GetRoleLevel(Role_MapID, Role_InstanceID, Role[k])
												local roleexp = 100*growth_exp[rolelevel].gaojiim-------------根据植物获得经验
												if aroundmember == 1 then
													roleexp = roleexp
												else
													roleexp = roleexp/aroundmember *1.2
												end
												role.AddRoleExp(MapID, InstanceID, Role[k], roleexp)
												unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
											end
										end
									end
								end
							end
						else --抢树，不在所属者队中
							local MsgID = msg.BeginMsgEvent()
							msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
							msg.AddMsgEvent(MsgID, 21, 9)	-- 选项一-------------------------------根据材料填，此处高级9
							msg.AddMsgEvent(MsgID, 1, 21)	-- "确定"
							msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
							msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
							msg.AddMsgEvent(MsgID, 24, TargetID)
							msg.DispatchRoleMsgEvent(RoleID, MsgID)
						end
	--1.5新增内容////////////////////////////////////////////////////////////////////dddddooooooowwwwwwnnnnnn，高级材料▲▲▲▲▲
					else
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1316)-- "对该植物的两次施肥间隔不足1分钟，为防止频繁施肥对植物造成的伤害，请您稍等片刻再来施肥。"
						msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
						msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
						msg.AddMsgEvent(MsgID, 24, TargetID)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)
					end
				else
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20, 1318) --"您没有培土灵肥。\n培土灵肥乃是仙家圣物，可遇而不可求，听说在大农丞处完成“年货大采购”任务将有较小几率获得培土灵肥，另外开启“金虎如意匣”或“新春红包”等均有一定几率获得。\n或者您可以尝试使用普通培土农肥对该植物进行施肥。"
					msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
					msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
				TalkIndex = -1
	--///////.///////////////////////////////////////////////////////////////////////////////////////1.5up,低级材料▼▼▼▼▼▼
		elseif TalkIndex == 8 then
			local Temp = 1
			if role.IsRoleInStatus(RoleID, 1048576) == false then --必须开启自由才可以抢树。
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				Temp = 0
			end
			if Temp == 1 then
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303011, 1, 101)--------------删除低级肥
				growtime = growtime + 1-------------------------记录操作次数
				local cTime = os.time() --当前时间
				local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 3, cTime)------------高级材料不记录
				HaveDelete_growthitem = 1
				local roleexp = 100*growth_exp[rolelevel].gaojipt-----------------根据植物给经验
				role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
				unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
				unit.AddBuff(MapID, InstanceID, RoleID, 2025601, RoleID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1343) --"此植物不属于你或你的队友，因此你受到了虚弱效果的影响。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
	--/////////////////////////////////////////////////////////////////////////////////////////////////1.5down，低级材料▲▲▲▲▲
	--///////.///////////////////////////////////////////////////////////////////////////////////////1.5up,高级材料▼▼▼▼▼▼
		elseif TalkIndex == 9 then
			local Temp = 1
			if role.IsRoleInStatus(RoleID, 1048576) == false then --必须开启自由才可以抢树。
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1341) --"此植物不属于你或你的小队，你必须开启自由模式才可以浇种，并准备接受各种虚弱惩罚。"
				msg.AddMsgEvent(MsgID, 21, 5)	-- 选项一
				msg.AddMsgEvent(MsgID, 1, 1303)	-- "离开"
				msg.AddMsgEvent(MsgID, 24, TargetID)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				Temp = 0
			end
			if Temp == 1 then
				role.RemoveFromRole(MapID, InstanceID, RoleID, 3303012, 1, 101)--------------删除高级肥
				growtime = 5-------------------------记录操作次数
				HaveDelete_growthitem = 1
				local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
				local roleexp = 100*growth_exp[rolelevel].gaojiim-----------------根据植物给经验
				role.AddRoleExp(MapID, InstanceID, RoleID, roleexp)
				unit.AddBuff(MapID, InstanceID, TargetID, 5003901, TargetID)-----浇水或是施肥
				unit.AddBuff(MapID, InstanceID, RoleID, 2025601, RoleID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1343) --"此植物不属于你或你的队友，因此你受到了虚弱效果的影响。"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
	--/////////////////////////////////////////////////////////////////////////////////////////////////1.5down，高级材料▲▲▲▲▲
		end
    end

	if 	HaveDelete_growthitem == 1 then
			--增加进阶实力值
			Increase_shili(MapID, InstanceID, RoleID, 0, 1,"jinjieshili_A")

		if growtime == 5 then
			local i = math.random(1,200)--0.5%几率
			if i ~= 99 then
				local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
				local creatureID = map.MapCreateCreature(MapID, InstanceID, 4900416, x, y, z)
				if seedowner == 0 then
				seedowner = RoleID
				end
				cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 1, seedowner)--记录玩家的ID

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 1320)--您所种植的植物在几次施肥后终于开花结果，请您尽快前往收获，以防其枯萎！
				msg.DispatchRoleMsgEvent(seedowner, MsgID)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 1339)--"您在耕种过程中意外惊醒绝情婆婆，一无所获，还不快去凤翔南门报仇！"
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				local MsgID1 = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID1, 26, 1339)	-- "您在耕种过程中意外惊醒绝情婆婆，一无所获，还不快去凤翔南门报仇！"
				msg.DispatchRoleMsgEvent(RoleID, MsgID1)

				map.MapCreateCreature(3017298383, InstanceID, 1534111,854,10000,1836)

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 100, 1340)--"<角色名>在耕种过程中意外放出绝情婆婆，请天下有情之人速往凤翔南门驱赶，抢夺宝物。绝情婆婆有几率掉落虎妞灵兽，情人节时装等珍贵道具。"
				msg.AddMsgEvent(MsgID, 2, RoleID)	--传入玩家名
				msg.DispatchWorldMsgEvent(MsgID)
			end


			map.MapDeleteCreature(MapID, InstanceID, TargetID)

		else
			if seedowner == 0 then
			cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 1, RoleID)
			end
			cre.SetCreatureScriptData(MapID, InstanceID, TargetID, 1, 2, growtime)--记录此土堆的浇水次数
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1319)-- "使当前植物结果还需用【培土农肥】施肥XXX次，或者直接使用【培土灵肥】施肥一次。"
			msg.AddMsgEvent(MsgID, 9, 5-growtime)--传入还可浇灌的次数
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end

	end
end

aux.RegisterCreatureEvent(4900415, 7, "RoseShu_OnTalk")

