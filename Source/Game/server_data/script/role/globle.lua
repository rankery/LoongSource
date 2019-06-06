--玩家脚本
--第一次上线和第一次进入游戏世界

--新手男时装套装
table_1stOnline_man = { {9000001, 9000002, 9000003, 9000004},
			{9000101, 9000102, 9000103, 9000104},
			{9000201, 9000202, 9000203, 9000204}}

--新手女时装套装
table_1stOnline_woman = {{9000011, 9000012, 9000013, 9000014},
			 {9000111, 9000112, 9000113, 9000114},
			 {9000211, 9000212, 9000213, 9000214}}


--玩家第一次上线
function x_RoleFirstOnline(roleid)
	role.AddSkill(roleid, 9001001)  --给玩家可调查地物的技能
	role.AddQuest(roleid, 1)        --给玩家初始任务
end

--玩家第一次进入游戏世界
function x_RoleFirstIntoWorld(mapid, instanceid, roleid)
	local sex = role.GetRoleSex(roleid)
	if(sex==nil) then return end

	local dressid = role.GetRoleDressid(roleid)
	if(dressid ==nil) then return else dressid = dressid + 1 end

	local table_1stOnline = {}
	if sex == 0 then
		table_1stOnline = table_1stOnline_woman --女时装
	elseif sex == 1 then
		table_1stOnline = table_1stOnline_man   --男时装
	end

	--生成时装并装备上
	for i=1,4 do
		serial_high, serial_low, equip_pos = role.AddRoleItem(mapid, instanceid, roleid, table_1stOnline[dressid][i], 1, 0, -1, 380)
		if serial_high and serial_low and equip_pos then
			role.RoleEquip(mapid, instanceid, roleid, serial_high, serial_low, equip_pos)
		end
	end

	-- 第一次进入游戏给与玩家2个体力药和2个真气药
	local n64ID1, n64ID2 = role.AddRoleItem(mapid, instanceid, roleid, 1000003, 2, -1, 0, 380)
	local n64ID3, n64ID4 = role.AddRoleItem(mapid, instanceid, roleid, 1000103, 2, -1, 0, 380)
	role.VirginOnlineSendClient(roleid, n64ID1, n64ID2, n64ID3, n64ID4)

	-- 第次进入游戏给玩家夺取神迹的技能
	role.AddSkill(roleid, 9001701)
end

-- 玩家每次进入游戏世界
function x_RoleIntoWorld(MapID, Instanceid, RoleID)

--------------------------------------神迹相关---------------------------------------
	-- 查看玩家的帮派是否夺取了神迹
	local guildID = 0
	guildID = guild.GetRoleGuildID(RoleID)
	local GuildPos
	GuildPos  = guild.GetRoleGuildPos(guildID, RoleID);
	if GuildPos ~= nil then
		local CapGuildNum = GetGuildCaptureNum(guildID)
		if CapGuildNum > 0 then
			local BuffId = GetBuffIDbyGodMiracleNum(CapGuildNum)
			unit.AddBuff(MapID, Instanceid, RoleID, BuffId, RoleID)
		end
	end

	-- 增加夺取神迹的技能
	--if role.IsLearnedSkill(RoleID, 90017) == nil then
		role.AddSkill(RoleID, 9001701)
	--end
--------------------------------------神迹END---------------------------------------

--------------------------------------清空排名数据---------------------------------------
    local curyear = tonumber(os.date("%Y"))
	if curyear ~= role.GetRoleScriptData(RoleID, 1, RoleDataType["YinYuanJie_Year"]) then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["YinYuanJie"], 0)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["LeiFengMao"], 0)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["YinYuanJie_Year"], curyear)
    end
--------------------------------------清空数据END---------------------------------------

--------------------------------------法宝相关---------------------------------------
	local level = role.GetRoleLevel(MapID, Instanceid, RoleID)
	local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
	if level >= 30 and k < 99 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3101)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
--------------------------------------法宝END---------------------------------------

--------------------------------------飞升相关---------------------------------------
	if  FeiSheng_trigger == 1 and level >= 80 and role.GetSoarValue(RoleID) == 0 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3130)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
--------------------------------------飞升相关---------------------------------------

---------------------------------------经脉内容------------------------------------

	local a = role.IsLearnedSkill(RoleID,11201)
	local b = role.IsLearnedSkill(RoleID,11217)
	local level = role.GetRoleLevel(MapID, Instanceid, RoleID)
	if level >= 50 then
		if a == nil and b == nil then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 71, 3516)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end
	end

---------------------------------------经脉内容------------------------------------

--------------------------------------试炼相关--------------------------------------
    local level = role.GetRoleLevel(MapID, Instanceid, RoleID)

	if level >= 5 then
		role.OpenTrain(RoleID, 78)
	end
	if level >= 10 then
		role.OpenTrain(RoleID, 79)
	end
	if level >= 15 then
		role.OpenTrain(RoleID, 12)
	end
	if level >= 20 then
		role.OpenTrain(RoleID, 80)
		role.OpenTrain(RoleID, 39)
		role.OpenTrain(RoleID, 47)
	end
	if level >= 25 then
		role.OpenTrain(RoleID, 1)
		role.OpenTrain(RoleID, 7)
	end
	if level >= 30 then
		role.OpenTrain(RoleID, 30)
		role.OpenTrain(RoleID, 48)
	end
	if level >= 35 then
		role.OpenTrain(RoleID, 63)
	end
	if level >= 40 then
		role.OpenTrain(RoleID, 37)
		role.OpenTrain(RoleID, 38)
		role.OpenTrain(RoleID, 49)
	end
	if level >= 45 then
		role.OpenTrain(RoleID, 18)
		role.OpenTrain(RoleID, 23)
		role.OpenTrain(RoleID, 28)
	end
	if level >= 50 then
		role.OpenTrain(RoleID, 50)
		--role.OpenTrain(RoleID, 71)
	end
	if level >= 55 then
		role.OpenTrain(RoleID, 51)
		role.OpenTrain(RoleID, 73)
	end
	if level >= 60 then
		role.OpenTrain(RoleID, 52)
	end
	if level >= 65 then
		role.OpenTrain(RoleID, 53)
		role.OpenTrain(RoleID, 74)
	end
	if level >= 70 then
		role.OpenTrain(RoleID, 54)
	end
	if level >= 75 then
		role.OpenTrain(RoleID, 55)
		role.OpenTrain(RoleID, 75)
	end
	if level >= 80 then
		role.OpenTrain(RoleID, 56)
	end
	if level >= 85 then
		role.OpenTrain(RoleID, 59)
		role.OpenTrain(RoleID, 76)
    end
	if level >= 90 then
		role.OpenTrain(RoleID, 60)
    end
	if level >= 95 then
		role.OpenTrain(RoleID, 61)
		role.OpenTrain(RoleID, 77)
    end
	if level >= 100 then
		role.OpenTrain(RoleID, 62)
	end
--------------------------------------试炼END---------------------------------------

end

-- 玩家等级改变
function x_RoleLevelChange(MapID, InstanceID, RoleID)

-------------------------------------师徒相关--------------------------------------------------

	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local MasterID = role.GetMasterID(MapID, InstanceID, RoleID)
	local bHaveGraduate = role.GetRoleScriptData(RoleID, 1, RoleDataType["HaveGraduate"])	-- 是否已经出师

	if MasterID ~= 4294967295  and level < 50 and bHaveGraduate ~= 1 then	-- 有师傅且未出师
		role.LevelUpAddJingWuPoint(MapID, InstanceID, RoleID)		-- 为师门增加精武点数

		-- 等级大于10级的徒弟升级可以为师父增加相应积分
		if level > 10 then
			role.LevelUpAddBonus(MapID, InstanceID, RoleID)
		end

		-- 等级为10的整数倍，且为使用精武点数升级，全服广播
		local UseJingWuPoint = role.GetRoleScriptData(RoleID, 1, RoleDataType["UseJingWuPoint"])
		if math.fmod(level, 10) == 0 and UseJingWuPoint == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 860)
			msg.AddMsgEvent(MsgID, 2, RoleID)
			msg.AddMsgEvent(MsgID, 9, level)
			msg.AddMsgEvent(MsgID, 2, MasterID)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	end

	if MasterID ~= 4294967295 and level == 50 and bHaveGraduate ~= 1 then	-- 出师
		role.GraduateNotice(MapID, InstanceID, RoleID)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["GraduateReward"], 1)	-- 设置未领取出师奖励
		role.SetRoleScriptData(RoleID, 1, RoleDataType["HaveGraduate"], 1)		-- 设置为已出师
	end

	-- 重置使用精武点数升级
	role.SetRoleScriptData(RoleID, 1, RoleDataType["UseJingWuPoint"], 0)

---------------------------------------师徒END----------------------------------------------------------


---------------------------------------经脉内容----------------------------------------------------------

	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if level == 50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 71, 3516)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end

---------------------------------------经脉END----------------------------------------------------------



---------------------------------------服务器广播相关-----------------------------------------------
	--math.randomseed(os.time());						--设置随机种子
	local n_RoleLv = role.GetRoleLevel(MapID, InstanceID, RoleID);		--获取玩家升级后的等级
	if ( n_RoleLv >= 80 ) then						--判断玩家升级后等级是否达到80级，若未达到则不予广播
		if ( math.mod(n_RoleLv,5) == 0 ) then				--判断玩家升级后的等级是否可被5整除，若时则进行大字屏显广播
			local r = math.random(1,100);				--取随机数，在四条大字屏显广播中随机选取一条进行播放
			if ( r<26 ) then
				local MsgID = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID,102,100009);
				msg.AddMsgEvent(MsgID,2,RoleID);
				msg.AddMsgEvent(MsgID,9,n_RoleLv);
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
			elseif ( r<51 ) then
				local MsgID = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID,102,100010);
				msg.AddMsgEvent(MsgID,2,RoleID);
				msg.AddMsgEvent(MsgID,9,n_RoleLv);
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
			elseif ( r<76 ) then
				local MsgID = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID,102,100011);
				msg.AddMsgEvent(MsgID,2,RoleID);
				msg.AddMsgEvent(MsgID,9,n_RoleLv);
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
			else
				local MsgID = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID,102,100012);
				msg.AddMsgEvent(MsgID,2,RoleID);
				msg.AddMsgEvent(MsgID,9,n_RoleLv);
				msg.DispatchBroadcast(MsgID,-1,-1,-1);
			end
		else
			if ( n_RoleLv>=90 ) then				--若玩家等级为80-90级则有60%几率触发广播，其中5%几率为全服广播；若玩家等级为90级以上，则有80%几率触发广播，其中10%几率为全服广播；
				local r1 = math.random(1,100);
				if ( r1<81 ) then
					local r2 = math.random(1,100);
					if ( r2<26 ) then					--随机选择一条小字跑马灯广播内容
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100013);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					elseif ( r2<51 ) then
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100014);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					elseif ( r2<76 ) then
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100015);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					else
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100016);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					end
				end
			else
				local r1 = math.random(1,100);
				if ( r1<61 ) then
					local r2 = math.random(1,100);
					if ( r2<26 ) then					--随机选择一条小字跑马灯广播内容
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100013);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					elseif ( r2<51 ) then
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100014);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					elseif ( r2<76 ) then
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100015);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<6 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					else
						local MsgID = msg.BeginMsgEvent();
						msg.AddMsgEvent(MsgID,102,100016);
						msg.AddMsgEvent(MsgID,2,RoleID);
						msg.AddMsgEvent(MsgID,9,n_RoleLv);
						if ( r1<11 ) then
							msg.DispatchBroadcast(MsgID,-1,-1,-1);
						else
							msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
						end
					end
				end
			end
		end
	end
----------------------------------服务器广播END------------------------------------------------


---------------------------------------试炼相关---------------------------------------------------------

	if level == 5 then
		role.OpenTrain(RoleID, 78)
	elseif level == 10 then
		role.OpenTrain(RoleID, 79)
	elseif level == 15 then
		role.OpenTrain(RoleID, 12)
	elseif level == 20 then
		role.OpenTrain(RoleID, 80)
		role.OpenTrain(RoleID, 39)
		role.OpenTrain(RoleID, 47)
	elseif level == 25 then
		role.OpenTrain(RoleID, 1)
		role.OpenTrain(RoleID, 7)
	elseif level == 30 then
		role.OpenTrain(RoleID, 30)
		role.OpenTrain(RoleID, 48)
	elseif level == 35 then
		role.OpenTrain(RoleID, 63)
	elseif level == 40 then
		role.OpenTrain(RoleID, 37)
		role.OpenTrain(RoleID, 38)
		role.OpenTrain(RoleID, 49)
	elseif level == 45 then
		role.OpenTrain(RoleID, 18)
		role.OpenTrain(RoleID, 23)
		role.OpenTrain(RoleID, 28)
	elseif level == 50 then
		role.OpenTrain(RoleID, 50)
		--role.OpenTrain(RoleID, 71)
	elseif level == 55 then
		role.OpenTrain(RoleID, 51)
		role.OpenTrain(RoleID, 73)
	elseif level == 60 then
		role.OpenTrain(RoleID, 52)
	elseif level == 65 then
		role.OpenTrain(RoleID, 53)
		role.OpenTrain(RoleID, 74)
	elseif level == 70 then
		role.OpenTrain(RoleID, 54)
	elseif level == 75 then
		role.OpenTrain(RoleID, 55)
		role.OpenTrain(RoleID, 75)
	elseif level == 80 then
		role.OpenTrain(RoleID, 56)
	elseif level == 85 then
		role.OpenTrain(RoleID, 59)
		role.OpenTrain(RoleID, 76)
	elseif level == 90 then
		role.OpenTrain(RoleID, 60)
	elseif level == 95 then
		role.OpenTrain(RoleID, 61)
		role.OpenTrain(RoleID, 77)
	elseif level == 100 then
		role.OpenTrain(RoleID, 62)
	end

---------------------------------------试炼END---------------------------------------------------------

---------------------------------------赠点相关--------------------------------------------------------

	if Salary_NPC_On==1 then

		local Have_FreePoint = role.GetRoleScriptData(RoleID, 1, RoleDataType["Have_FreePoint"])

		if level == 35 and Have_FreePoint==0 then
			role.AddRoleGiftPoints(MapID, InstanceID, RoleID, 3000)
			role.SetRoleScriptData(RoleID, 1, RoleDataType["Have_FreePoint"], 1)
		end

	end
---------------------------------------赠点END---------------------------------------------------------

end

-- 玩家拜师
function x_RoleHandleMaster(MasterID, ApprenticeID)
	local MsgID = msg.BeginMsgEvent()		-- 全服跑马灯...
	msg.AddMsgEvent(MsgID, 100, 2330)
	msg.AddMsgEvent(MsgID, 2, MasterID)
	msg.AddMsgEvent(MsgID, 2, ApprenticeID)
	msg.DispatchWorldMsgEvent(MsgID)
end

-- 法宝相关
function x_RoleFAbaoLevelup(MapID, InstanceID, RoleID, FabaoID, Type, FBVal)
	if Type == 0 then
	    if FBVal >= 11 and FBVal < 21 then
		    local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
			if k % 10 < 2 then
			    k = math.floor(k/10) * 10 + 2
			end
			role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], k)
		elseif FBVal >= 21 and FBVal < 31 then
			local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
			if k % 10 < 3 then
			    k = math.floor(k/10) * 10 + 3
			end
			role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], k)
		elseif FBVal >= 31 and FBVal < 41 then
			local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
			if k % 10 < 4 then
			    k = math.floor(k/10) * 10 + 4
			end
			role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], k)
		elseif FBVal >= 41 then
		    local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
			if k % 10 < 5 then
			    k = math.floor(k/10) * 10 + 5
			end
			role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], k)
		end
	elseif Type == 1 or Type == 2 then
	    local k = role.GetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"])
		if k < 10 then
		    role.SetRoleScriptData(RoleID, 1, RoleDataType["FaBaoShiLian"], 10 + k)
		end
	end
end

function x_RoleLianQi(MapID, InstanceID, RoleID, ItemID)
    local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
	local k = b % 1000
	if k <= 200 then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], b + 10)
	end
end


function x_RoleRongZhuLevelup(MapID, InstanceID, RoleID, ItemID, Level)
    local b = role.GetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"])
	local k = b % 1000
	if Level == 20 and b < 4000 then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], 4000+k)
	elseif Level >= 10 and b < 3000 then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], 3000+k)
	elseif Level >= 5 and b < 2000 then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], 2000+k)
	elseif Level >= 1 and b < 1000 then
	    role.SetRoleScriptData(RoleID, 1, RoleDataType["FeiSheng_shilian"], 1000+k)
	end
end

--注册函数
aux.RegisterRoleEvent(1, "x_RoleFirstOnline")
aux.RegisterRoleEvent(2, "x_RoleIntoWorld")
aux.RegisterRoleEvent(3, "x_RoleFirstIntoWorld")
aux.RegisterRoleEvent(6, "x_RoleLevelChange")
aux.RegisterRoleEvent(27, "x_RoleHandleMaster")
aux.RegisterRoleEvent(35, "x_RoleFAbaoLevelup")
aux.RegisterRoleEvent(40, "x_RoleLianQi")
aux.RegisterRoleEvent(41, "x_RoleRongZhuLevelup")
