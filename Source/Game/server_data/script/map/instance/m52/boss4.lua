--[[
MapScriptData 1： 10秒计时器使用，用于计算数量达到标准传送寒冰世界中的玩家回来
MapScriptData 2：记录双极领主的ID
MapScriptData 3：记录1分钟传送门ID
MapScriptData 4：记录狼房
MapScriptData 5：记录豹子房
MapScriptData 6：记录老虎房
MapScriptData 7：记录蜂窝
MapScriptData 8：记录进栅栏次数
MapScriptData 9：记录双极领主是否进入了战斗，进入了=1，脱战/死亡=0
MapScriptData 10:记录动物园的ID
MapScriptData 11:记录格鲁尔的ID
MapScriptData 12:记录老大的ID
MapScriptData 13:记录老2的ID
MapScriptData 14:记录老3的ID
MapScriptData 15:记录跗骨毒蝎的ID
MapScriptData 16:记录跗骨毒蝎的ID
MapScriptData 17:记录跗骨毒蝎的ID
MapScriptData 18:记录跗骨毒蝎的ID
MapScriptData 19:记录跗骨毒蝎的ID
MapScriptData 20:记录跗骨毒蝎的ID
MapScriptData 21:记录跗骨毒蝎的ID
MapScriptData 22:记录跗骨毒蝎的ID
MapScriptData 23:记录跗骨毒蝎的ID
MapScriptData 24:记录跗骨毒蝎的ID
MapScriptData 25:记录传送特效1的ID
MapScriptData 26:记录传送特效2的ID
MapScriptData 27:记录传送特效3的ID
MapScriptData 28:记录三宝是否活着
MapScriptData 29:记录是已经设置默认值
MapScriptData 30:记录里面的玩家是否出来
MapScriptData 31:记录刷门的时间
]]--

--开门NPC怪
--npc id还未指定
function m52_OnPlayerEnter(MapID, InstanceID, RoleID)

	--进入副本时删除玩家的坐骑buff
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	end

    if map.GetMapScriptData(MapID, InstanceID, 1, 29) ~= 1 then
		map.OpenCloseDoor(MapID, InstanceID, 1072, 1)  --动物
		map.OpenCloseDoor(MapID, InstanceID, 1087, 1)  --三宝
		map.OpenCloseDoor(MapID, InstanceID, 1073, 1)   --格鲁尔
		map.OpenCloseDoor(MapID, InstanceID, 1074, nil)

		local c_table = {}
		c_table[1] = {Cretype=1535463,x=357,y=1000,z=346}
		c_table[2] = {Cretype=1535463,x=351,y=1000,z=340}
		c_table[3] = {Cretype=1535464,x=328,y=1000,z=303}
		c_table[4] = {Cretype=1535464,x=322,y=1000,z=297}
		c_table[5] = {Cretype=1535465,x=320,y=1000,z=372}
		c_table[6] = {Cretype=1535465,x=314,y=1000,z=366}
		c_table[7] = {Cretype=1535466,x=289,y=1000,z=330}
		c_table[8] = {Cretype=1535466,x=283,y=1000,z=324}
		c_table[9] = {Cretype=1535466,x=289,y=1000,z=324}
		c_table[10] = {Cretype=1535462,x=331,y=1000,z=325}  --动物园
		c_table[11] = {Cretype=1535478,x=448,y=2110,z=193}  --格鲁尔
		c_table[12] = {Cretype=1535482,x=383,y=1050,z=118}  --老大
		c_table[13] = {Cretype=1535483,x=368,y=1050,z=106}   --老2
		c_table[14] = {Cretype=1535484,x=370,y=1050,z=118}   --老3
		c_table[15] = {Cretype=1535479,x=439,y=2110,z=165}   --小怪1
		c_table[16] = {Cretype=1535480,x=439,y=2110,z=180}   --小怪2
		c_table[17] = {Cretype=1535479,x=439,y=2110,z=195}    --小怪3
		c_table[18] = {Cretype=1535480,x=452,y=2110,z=165}    --小怪4
		c_table[19] = {Cretype=1535479,x=452,y=2110,z=180}    --小怪5
		c_table[20] = {Cretype=1535480,x=452,y=2110,z=195}    --小怪6
		c_table[21] = {Cretype=1535479,x=465,y=2110,z=165}    --小怪7
		c_table[22] = {Cretype=1535480,x=465,y=2110,z=180}    --小怪8
		c_table[23] = {Cretype=1535479,x=465,y=2110,z=195}    --小怪9
		c_table[24] = {Cretype=1535480,x=460,y=2110,z=187}    --小怪10
		c_table[25] = {Cretype=4902025,x=226,y=1250,z=138}    --传送1  动物园
		c_table[26] = {Cretype=4902026,x=246,y=1250,z=109}    --传送2  格鲁尔
		c_table[27] = {Cretype=4902027,x=268,y=1250,z=138}    --传送3  三宝


		for i = 1,27 do
			local ID = map.MapCreateColCreature(MapID, InstanceID, c_table[i].Cretype, c_table[i].x, c_table[i].y, c_table[i].z, "")
			if i >= 1 and i <= 9 then
			    cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			end
			if i >= 10 and i <= 27 then
				map.SetMapScriptData(MapID, InstanceID, 1, i, ID)
				if i >= 15 and i <= 24 then
					cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, i-14)
				end
			end
		end

		map.SetMapScriptData(MapID, InstanceID, 1, 28, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 29, 1)
	end

end

--注册
aux.RegisterMapEvent("m52", 2, "m52_OnPlayerEnter")

--BOOS死亡前可以传送
function m52_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
	local Trigger_ObjID = {}
	Trigger_ObjID[1248] = {data=10,x=434,y=20,z=360} --动物园
	Trigger_ObjID[1249] = {data=11,x=216,y=44,z=38} --格鲁尔
	Trigger_ObjID[1250] = {data=28,x=439,y=20,z=328} --三宝
	if map.GetMapScriptData(MapID, InstanceID, 1, Trigger_ObjID[ObjID].data) ~= 0 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067629775, Trigger_ObjID[ObjID].x, Trigger_ObjID[ObjID].y, Trigger_ObjID[ObjID].z)
	end
end
aux.RegisterMapEvent("m52",6,"m52_OnEnterTrigger")

function x4902029_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    --获取队伍玩家等级
	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20,	12701)		--“进入这个奇妙的时空幻象的人能到达传说中的上古洞穴。（只有100级以上的玩家可以进入）”
		msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
		msg.AddMsgEvent(MsgID, 1,	12702)			--“进入上古洞穴”
		msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1,	22)			--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 6 then
		if TeamID == 4294967295 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 	2812)	--您没有组队，不能进入该副本
			msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
			msg.AddMsgEvent(MsgID, 1, 	22)		--取消
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			local Num = 0
			for i=1, 6 do
				if Role[i] and Role[i]~= 4294967295 then
					Num = Num + 1
				end
			end
			if rolelevel < 100 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 	2813)	--您等级不足，不能进入该副本
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			elseif Num < 4 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 12703)	--您需要凑够4个队友才能进入上古洞穴
				msg.AddMsgEvent(MsgID, 21, 	5)		--取消按钮
				msg.AddMsgEvent(MsgID, 1, 	22)		--取消
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067629775, 61, 46, 450)
			end
		end
	end
end
--
aux.RegisterCreatureEvent(4902029, 7, "x4902029_OnTalk")


----------------------------------------动物园-----------------------------------------------------------

--怪力莽猿进入战斗关门
function c1535462_OnEnterCombat(MapID, InstanceID, CreatureID)
	map.OpenCloseDoor(MapID, InstanceID, 1072, nil)
end
aux.RegisterCreatureEvent(1535462, 2, "c1535462_OnEnterCombat")

--boss脱战门开启
function c1535462_OnLeaveCombat(MapID, InstanceID, CreatureID)
	map.OpenCloseDoor(MapID, InstanceID, 1072, 1)
end
aux.RegisterCreatureEvent(1535462, 3, "c1535462_OnLeaveCombat")

--BOSS死亡开门
function c1535462_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    map.OpenCloseDoor(MapID, InstanceID, 1072, 1)
	map.SetMapScriptData(MapID, InstanceID, 1, 10, 0)
	map.MapDeleteCreature(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 25))

	if map.GetMapScriptData(MapID, InstanceID, 1, 11) == 0 and map.GetMapScriptData(MapID, InstanceID, 1, 28) == 0 then
        map.OpenCloseDoor(MapID, InstanceID, 1074, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12704)  --通往双极领主巢穴的大门已经开启！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
    end

end
aux.RegisterCreatureEvent(1535462, 4, "c1535462_OnDie")


--使用技能禁闭刑罚后,随机传送一个玩家按顺序进入一个围栏
function s2432701_Cast(MapID, InstanceID, SkillID, OwnerID)
	local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
	local Role = {}
	local k = 0
	for u,v in pairs(RoleTbl) do
		if v ~= nil then
			k = k + 1
			Role[k] = v
		end
	end
	if k > 0 then
		local r = math.random(1,k)
		if unit.IsDead(MapID, InstanceID, Role[r]) then
		    for i = 1,k do
			    if not unit.IsDead(MapID, InstanceID, Role[i]) then
				    r = i
				end
			end
		end
		if not unit.IsDead(MapID, InstanceID, Role[r]) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12705)  --:让我的宠物们陪你玩玩！
			msg.AddMsgEvent(MsgID, 2, Role[r])
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
			local Place = {}
			Place[1] = {x = 354,y = 20,z = 343} --狼房
			Place[2] = {x = 325,y = 20,z = 300} --豹子房
			Place[3] = {x = 317,y = 20,z = 369} --老虎房
			Place[4] = {x = 286,y = 20,z = 327} --蜂窝

			if map.GetMapScriptData(MapID, InstanceID, 1, 8) == 0 then
				map.SetMapScriptData(MapID, InstanceID, 1, 8, 1)
			end

			local p = map.GetMapScriptData(MapID, InstanceID, 1, 8) % 4 + 1
			role.RoleGotoNewMap(MapID, InstanceID, Role[r], 3067629775, Place[p].x, Place[p].y, Place[p].z)
			--map.SetMapScriptData(MapID, InstanceID, 1, 1, Role1[r])
		end
	end
end
aux.RegisterSkillEvent(2432701, 1, "s2432701_Cast")


--围栏中怪物死亡,达到数量,传送玩家回到场地中央
function c1535463_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 4, map.GetMapScriptData(MapID, InstanceID, 1, 4) + 1)
		if(map.GetMapScriptData(MapID, InstanceID, 1, 4) == 2) then
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if v ~= nil then
					if map.IsInArea(MapID, InstanceID, v, 1188) then  --1188狼房的区域
						role.RoleGotoNewMap(MapID, InstanceID, v, 3067629775, 331, 20, 325)
					end
				end
			end
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535463, 357, 1000, 346, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535463, 351, 1000, 340, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			map.SetMapScriptData(MapID, InstanceID, 1, 4, 0)
		end
	end
end

function c1535464_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 5, map.GetMapScriptData(MapID, InstanceID, 1, 5) + 1)
		if(map.GetMapScriptData(MapID, InstanceID, 1, 5) == 2) then
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if v ~= nil then
					if map.IsInArea(MapID, InstanceID, v, 1189) then  --1189豹子房的区域
						role.RoleGotoNewMap(MapID, InstanceID, v, 3067629775, 331, 20, 325)
					end
				end
			end
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535464, 328, 1000, 303, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535464, 322, 1000, 297, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			map.SetMapScriptData(MapID, InstanceID, 1, 5, 0)
		end
	end
end

function c1535465_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 6, map.GetMapScriptData(MapID, InstanceID, 1, 6) + 1)
		if(map.GetMapScriptData(MapID, InstanceID, 1, 6) == 2) then
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if v ~= nil then
					if map.IsInArea(MapID, InstanceID, v, 1190) then  --1190老虎房的区域
						role.RoleGotoNewMap(MapID, InstanceID, v, 3067629775, 331, 20, 325)
					end
				end
			end
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535465, 320, 1000, 372, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535465, 314, 1000, 366, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			map.SetMapScriptData(MapID, InstanceID, 1, 6, 0)
		end
	end
end

function c1535466_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    if cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0) == 1 then
		map.SetMapScriptData(MapID, InstanceID, 1, 7, map.GetMapScriptData(MapID, InstanceID, 1, 7) + 1)
		if(map.GetMapScriptData(MapID, InstanceID, 1, 7) == 3) then
			local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
			for u,v in pairs(RoleTbl) do
				if v ~= nil then
					if map.IsInArea(MapID, InstanceID, v, 1191) then  --1191蜂窝的区域
						role.RoleGotoNewMap(MapID, InstanceID, v, 3067629775, 331, 20, 325)
					end
				end
			end
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535466, 289, 1000, 330, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535466, 283, 1000, 324, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			local ID = map.MapCreateColCreature(MapID, InstanceID, 1535466, 289, 1000, 324, "")
			cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, 1)
			map.SetMapScriptData(MapID, InstanceID, 1, 7, 0)
		end
	end
end
aux.RegisterCreatureEvent(1535463, 4, "c1535463_OnDie")
aux.RegisterCreatureEvent(1535464, 4, "c1535464_OnDie")
aux.RegisterCreatureEvent(1535465, 4, "c1535465_OnDie")
aux.RegisterCreatureEvent(1535466, 4, "c1535466_OnDie")


--刷出的怪定时消失
--栅栏中玩家死亡传出
function m52_OnRoleDie(MapID, InstanceID, RoleID, KillID)
	if map.IsInArea(MapID, InstanceID, RoleID, 1188) or map.IsInArea(MapID, InstanceID, RoleID, 1189) or map.IsInArea(MapID, InstanceID, RoleID, 1190) or map.IsInArea(MapID, InstanceID, RoleID, 1191) then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067629775, 331, 1000, 325)
	end
end
aux.RegisterMapEvent("m52", 5, "m52_OnRoleDie")

--召唤百兽,技能使用后,场地中间4各点随机刷一定数量的怪物,同时也给当前BOSS目标身上刷毒气NPC

function s2432801_Cast(MapID, InstanceID, SkillID, OwnerID)

	local RanC = {1535467, 1535468, 1535469, 1535470}
	local PlaceCenter = {}
	PlaceCenter[1] = {x = 330, y = 1000, z = 349}
	PlaceCenter[2] = {x = 330, y = 1000, z = 324}
	PlaceCenter[3] = {x = 311, y = 1000, z = 319}
	PlaceCenter[4] = {x = 302, y = 1000, z = 344}

	for i = 1,4 do
		local rc = math.random(1,4)
		map.MapCreateColCreature(MapID, InstanceID, RanC[rc], PlaceCenter[i].x, PlaceCenter[i].y, PlaceCenter[i].z, "")
	end
	local x1, y1, z1 = unit.GetPosition(MapID,InstanceID,OwnerID)
	local CreatureID = map.MapCreateCreature(MapID,InstanceID, 4902024,x1,y1,z1)
	--unit.AddBuff(MapID, InstanceID, CreatureID, 9441501, CreatureID)

	return 0
end

aux.RegisterSkillEvent(2432801, 1, "s2432801_Cast")

-------------------------------------------格鲁尔------------------------------------------------------------------------
--格鲁尔进入战斗关门
function c1535478_OnEnterCombat(MapID, InstanceID, CreatureID)
	map.OpenCloseDoor(MapID, InstanceID, 1073, nil)
	unit.AddBuff(MapID, InstanceID, CreatureID, 9443401, CreatureID)
end
aux.RegisterCreatureEvent(1535478, 2, "c1535478_OnEnterCombat")

--boss脱战门开启
function c1535478_OnLeaveCombat(MapID, InstanceID, CreatureID)
	map.OpenCloseDoor(MapID, InstanceID, 1073, 1)
	unit.CancelBuff(MapID, InstanceID, CreatureID, 9443401)
end
aux.RegisterCreatureEvent(1535478, 3, "c1535478_OnLeaveCombat")

--BOSS死亡开门
function c1535478_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    for i = 1,10 do
	    for k = 1,10 do
		    local a = math.random(2)
			local p = math.random(100)
			if p > 80 then
			    map.MapCreateCreature(MapID,InstanceID, 1535478+a,218+i*5,1250,96+k*7)
			end
		end
	end

	map.OpenCloseDoor(MapID, InstanceID, 1073, 1)
	map.SetMapScriptData(MapID, InstanceID, 1, 11, 0)
	map.MapDeleteCreature(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 26))

	if map.GetMapScriptData(MapID, InstanceID, 1, 10) == 0 and map.GetMapScriptData(MapID, InstanceID, 1, 28) == 0 then
        map.OpenCloseDoor(MapID, InstanceID, 1074, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12704)  --通往双极领主巢穴的大门已经开启！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
    end
end
aux.RegisterCreatureEvent(1535478, 4, "c1535478_OnDie")

--随机唤醒几只毒蝎
function s2433001_Cast(MapID, InstanceID, SkillID, OwnerID)
    for i=15,24 do
	    if math.random(10) > 6 and map.GetMapScriptData(MapID, InstanceID, 1, i) ~= 0 then
		    unit.AddBuff(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, i), 9443401, map.GetMapScriptData(MapID, InstanceID, 1, i))
			local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, OwnerID)
			cre.AddEnmity(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, i), TargetID, 1)

		end
	end

	return 0
end

aux.RegisterSkillEvent(2433001, 1, "s2433001_Cast")

function c1535479_OnEnterCombat(MapID, InstanceID, CreatureID)
	unit.AddBuff(MapID, InstanceID, CreatureID, 9442801, CreatureID)
end
aux.RegisterCreatureEvent(1535479, 2, "c1535479_OnEnterCombat")
aux.RegisterCreatureEvent(1535480, 2, "c1535479_OnEnterCombat")

--
function c1535479_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local k = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)

    if map.GetMapScriptData(MapID, InstanceID, 1, 11) ~= 0 and k ~= 0 then
	    local pos = {}
	    pos[1] = {Cretype=1535479,x=439,y=2110,z=165}   --小怪1
		pos[2] = {Cretype=1535480,x=439,y=2110,z=180}   --小怪2
		pos[3] = {Cretype=1535479,x=439,y=2110,z=195}    --小怪3
		pos[4] = {Cretype=1535480,x=452,y=2110,z=165}    --小怪4
		pos[5] = {Cretype=1535479,x=452,y=2110,z=180}    --小怪5
		pos[6] = {Cretype=1535480,x=452,y=2110,z=195}    --小怪6
		pos[7] = {Cretype=1535479,x=465,y=2110,z=165}    --小怪7
		pos[8] = {Cretype=1535480,x=465,y=2110,z=180}    --小怪8
		pos[9] = {Cretype=1535479,x=465,y=2110,z=195}    --小怪9
		pos[10] = {Cretype=1535480,x=460,y=2110,z=187}    --小怪10
		local monid = map.MapCreateColCreature(MapID, InstanceID, pos[k].Cretype, pos[k].x, pos[k].y, pos[k].z, "")
		cre.SetCreatureScriptData(MapID, InstanceID, monid, 1, 0, k)
		map.SetMapScriptData(MapID, InstanceID, 1, k+14, monid)
	end

end
aux.RegisterCreatureEvent(1535479, 4, "c1535479_OnDie")
aux.RegisterCreatureEvent(1535480, 4, "c1535479_OnDie")
---------------------------------------------------------三宝-------------------------------------------------------------------------------------------

--三宝
function c1535482_OnEnterCombat(MapID, InstanceID, CreatureID)

	map.OpenCloseDoor(MapID, InstanceID, 1087, nil)

	local Creature = {}
	Creature[1535482] = {13,14}
	Creature[1535483] = {12,14}
	Creature[1535484] = {12,13}

	local pos = {}
	pos[12] = {Cretype=1535482,x=383,y=1050,z=118}  --老大
	pos[13] = {Cretype=1535483,x=368,y=1050,z=106}   --老2
	pos[14] = {Cretype=1535484,x=370,y=1050,z=118}   --老3

	local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, CreatureID)
	local Boss1 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][1])
	local Boss2 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][2])
	if Boss1 == 0 then
		local mon = map.MapCreateColCreature(MapID, InstanceID, pos[Creature[TypeID][1]].Cretype, pos[Creature[TypeID][1]].x, pos[Creature[TypeID][1]].y, pos[Creature[TypeID][1]].z, "")
		map.SetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][1], mon)
	end
	if Boss2 == 0 then
		local mon = map.MapCreateColCreature(MapID, InstanceID, pos[Creature[TypeID][2]].Cretype, pos[Creature[TypeID][2]].x, pos[Creature[TypeID][2]].y, pos[Creature[TypeID][2]].z, "")
		map.SetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][2], mon)
	end

	local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为3个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 20, 20)

end
aux.RegisterCreatureEvent(1535482, 2, "c1535482_OnEnterCombat")
aux.RegisterCreatureEvent(1535483, 2, "c1535482_OnEnterCombat")
aux.RegisterCreatureEvent(1535484, 2, "c1535482_OnEnterCombat")

--boss脱战门开启
function c1535482_OnLeaveCombat(MapID, InstanceID, CreatureID)

    local Creature = {}
	Creature[1535482] = {13,14}
	Creature[1535483] = {12,14}
	Creature[1535484] = {12,13}
	local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, CreatureID)
	local Boss1 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][1])
	local Boss2 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][2])
	local bool1 = false
	local bool2 = false
    if Boss1 == 0 then
	    bool1 = true
	elseif not unit.IsAttack(MapID, InstanceID, Boss1) then
	    bool1 = true
	end
	if Boss2 == 0 then
	    bool2 = true
	elseif not unit.IsAttack(MapID, InstanceID, Boss2) then
	    bool2 = true
	end
	if bool1 and bool2 then
		map.OpenCloseDoor(MapID, InstanceID, 1087, 1)
		local pos = {}
		pos[12] = {Cretype=1535482,x=383,y=1050,z=118}  --老大
		pos[13] = {Cretype=1535483,x=368,y=1050,z=106}   --老2
		pos[14] = {Cretype=1535484,x=370,y=1050,z=118}   --老3
		if Boss1 == 0 then
		    local mon = map.MapCreateColCreature(MapID, InstanceID, pos[Creature[TypeID][1]].Cretype, pos[Creature[TypeID][1]].x, pos[Creature[TypeID][1]].y, pos[Creature[TypeID][1]].z, "")
			map.SetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][1], mon)
		end
		if Boss2 == 0 then
		    local mon = map.MapCreateColCreature(MapID, InstanceID, pos[Creature[TypeID][2]].Cretype, pos[Creature[TypeID][2]].x, pos[Creature[TypeID][2]].y, pos[Creature[TypeID][2]].z, "")
			map.SetMapScriptData(MapID, InstanceID, 1, Creature[TypeID][2], mon)
		end
		for i = 1,3 do
			unit.CancelBuff(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 11+i), 9442301)
			unit.CancelBuff(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 11+i), 9442401)
			unit.CancelBuff(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 11+i), 9442501)
		end
	end

end
aux.RegisterCreatureEvent(1535482, 3, "c1535482_OnLeaveCombat")
aux.RegisterCreatureEvent(1535483, 3, "c1535482_OnLeaveCombat")
aux.RegisterCreatureEvent(1535484, 3, "c1535482_OnLeaveCombat")


--BOSS死亡开门
function c1535484_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	map.SetMapScriptData(MapID, InstanceID, 1, TargetTypeID-1535470, 0)

	local Creature = {}
	Creature[1535482] = {13,14,9442301}
	Creature[1535483] = {12,14,9442401}
	Creature[1535484] = {12,13,9442501}
	local Boss1 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TargetTypeID][1])
	local Boss2 = map.GetMapScriptData(MapID, InstanceID, 1, Creature[TargetTypeID][2])
	if Boss1 ~= 0 then
	    unit.AddBuff(MapID, InstanceID, Boss1,  Creature[TargetTypeID][3], Boss1)
	    unit.AddBuff(MapID, InstanceID, Boss1,  9443001, Boss1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12706)  --岩灵兄弟继承了死去的岩灵的特殊能力！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	end
	if Boss2 ~= 0 then
		unit.AddBuff(MapID, InstanceID, Boss2,  Creature[TargetTypeID][3], Boss2)
	    unit.AddBuff(MapID, InstanceID, Boss2,  9443001, Boss2)
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 12706)  --岩灵兄弟继承了死去的岩灵的特殊能力！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
	end

	if Boss1 == 0 and Boss2 == 0 then

		map.SetMapScriptData(MapID, InstanceID, 1, 28, 0)

		map.OpenCloseDoor(MapID, InstanceID, 1073, 1)
		map.MapDeleteCreature(MapID, InstanceID, map.GetMapScriptData(MapID, InstanceID, 1, 27))

		map.MapCreateColCreature(MapID, InstanceID, 4902028, 383, 1050, 118, "")

		if map.GetMapScriptData(MapID, InstanceID, 1, 10) == 0 and map.GetMapScriptData(MapID, InstanceID, 1, 11) == 0 then
			map.OpenCloseDoor(MapID, InstanceID, 1074, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12704)  --通往双极领主巢穴的大门已经开启！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
		end

	end

end
aux.RegisterCreatureEvent(1535482, 4, "c1535484_OnDie")
aux.RegisterCreatureEvent(1535483, 4, "c1535484_OnDie")
aux.RegisterCreatureEvent(1535484, 4, "c1535484_OnDie")

---------------------------------------------------------双极领主----------------------------------------------------------


--双极领主进入战斗关门B
function c1535455_OnEnterCombat(MapID, InstanceID, CreatureID)

	map.OpenCloseDoor(MapID, InstanceID, 1075, nil)
    map.MapCreateColCreature(MapID, InstanceID, 4902021, 1, 1, 1, "")

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12707)  --双极领主：你们这群不知死活的东西
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)

	--记录BOSS进入战斗了
	map.SetMapScriptData(MapID,InstanceID,1,9, 1)
end

aux.RegisterCreatureEvent(1535455, 2, "c1535455_OnEnterCombat")



--双机领主使用技能：末日咆哮后，刷出传送门
function s2432201_Cast(MapID, InstanceID, SkillID, OwnerID)

	if map.GetMapScriptData(MapID,InstanceID,1,30) ~= 1 then

	    local cTime = os.time() --当前时间
		local lTime = map.GetMapScriptData(MapID,InstanceID,1,31)
		if lTime == 0 or os.difftime(cTime,lTime) > 60 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 12708)  --[场景]一阵猛烈的地动山摇后，场地中出现了一座空间传送门！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)

			--随机刷传送门
			local door_30s = {}
			door_30s[1] = {x=127, y=1000, z=362 }
			door_30s[2] = {x=125, y=1000, z=399 }
			door_30s[3] = {x=104, y=1000, z=385 }
			door_30s[4] = {x=107, y=1000, z=359 }
			local k = math.random(4)  -- <=1,<=4

			local door = map.MapCreateColCreature(MapID, InstanceID, 4902020, door_30s[k].x, door_30s[k].y, door_30s[k].z, "")
			--记录传送门ID
			map.SetMapScriptData(MapID,InstanceID,1,3, door)
			map.SetMapScriptData(MapID,InstanceID,1,31,cTime)
		end
    end
	return 0
end

aux.RegisterSkillEvent(2432201, 1, "s2432201_Cast")




--传送门对话传送玩家
function x4902020_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 12709)		--“传送门：\n    双机领主的内心世界是任何生机都没有的死亡之境，你要进入吗？”
		msg.AddMsgEvent(MsgID, 24, TargetID)	--npcid
		msg.AddMsgEvent(MsgID, 21, 4)			--确认按钮
		msg.AddMsgEvent(MsgID, 1, 12710)		--“是的,我要进入它的内心世界。”
		msg.AddMsgEvent(MsgID, 21, 5)			--取消按钮
		msg.AddMsgEvent(MsgID, 1, 22)		--“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then			--确认后传送，给传送的玩家上空buff

		--判断BOSS是否在战斗
		local Boss4_InBattle = map.GetMapScriptData(MapID,InstanceID,1,9)
		if Boss4_InBattle == 1 then
			--local door = map.GetMapScriptData(MapID,InstanceID,1,3)
			map.MapDeleteCreature(MapID, InstanceID, TargetID)  --传送完毕删除传送门
			map.MapCreateColCreature(MapID, InstanceID, 4902030, 1, 1, 1, "")  --刷新10s计时器
			local k=role.GetRoleAttValue(MapID, InstanceID, RoleID, 65)
			role.ModRoleAttValue(MapID, InstanceID, RoleID, 65, -k)
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3067629775, 65, 21, 42)
			map.SetMapScriptData(MapID,InstanceID,1,30, 1)

		end

	end
end

aux.RegisterCreatureEvent(4902020, 7, "x4902020_OnTalk")


--10秒计时器，定时播放“XX秒后将会离开双极领主的内心世界”
function c4902030_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
	local k1 = map.GetMapScriptData(MapID,InstanceID,1,1)
	map.SetMapScriptData(MapID,InstanceID,1,1, k1+1)
	local k2 = map.GetMapScriptData(MapID,InstanceID,1,1)
	if k2 == 1 or k2 == 2 then
		map.MapCreateColCreature(MapID, InstanceID, 4902030, 1, 1, 1, "")  --刷新10s计时器

	elseif k2 == 3 then
	    local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
	    for u,v in pairs(RoleTbl) do
			if v ~= nil then
				if map.IsInArea(MapID, InstanceID, v, 1092) then  --1092寒冰世界的区域
					local MsgID = msg.BeginMsgEvent() --30秒后将会离开双极领主的内心世界
					msg.AddMsgEvent(MsgID, 101, 12711)
					msg.AddMsgEvent(MsgID, 9, 30)
					msg.DispatchRoleMsgEvent(v, MsgID)
				end
			end
		end

		map.MapCreateColCreature(MapID, InstanceID, 4902030, 1, 1, 1, "")  --刷新10s计时器

	elseif k2 == 4 then

		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
	    for u,v in pairs(RoleTbl) do
			if v ~= nil then
				if map.IsInArea(MapID, InstanceID, v, 1092) then  --1092寒冰世界的区域
					local MsgID = msg.BeginMsgEvent() --20秒后将会离开双极领主的内心世界
					msg.AddMsgEvent(MsgID, 101, 12711)
					msg.AddMsgEvent(MsgID, 9, 20)
					msg.DispatchRoleMsgEvent(v, MsgID)
				end
			end
		end

		map.MapCreateColCreature(MapID, InstanceID, 4902030, 1, 1, 1, "")  --刷新10s计时器

	elseif k2 == 5 then

		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
	    for u,v in pairs(RoleTbl) do
			if v ~= nil then
				if map.IsInArea(MapID, InstanceID, v, 1092) then  --1092寒冰世界的区域
					local MsgID = msg.BeginMsgEvent() --10秒后将会离开双极领主的内心世界
					msg.AddMsgEvent(MsgID, 101, 12711)
					msg.AddMsgEvent(MsgID, 9, 10)
					msg.DispatchRoleMsgEvent(v, MsgID)
				end
			end
		end

		map.MapCreateColCreature(MapID, InstanceID, 4902030, 1, 1, 1, "")  --刷新10s计时器

	elseif k2 >= 6 then --6次=60秒，正好是1分钟，此时寒冰世界里如果有人就被传回来
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		map.SetMapScriptData(MapID,InstanceID,1,1, 0)
		map.SetMapScriptData(MapID,InstanceID,1,30, 0)
		for u,v in pairs(RoleTbl) do
			if v ~= nil then
				if map.IsInArea(MapID, InstanceID, v, 1092) then  --1092寒冰世界的区域
					role.RoleGotoNewMap(MapID, InstanceID, v, 3067629775, 113, 20, 369)
				end
			end
		end
	end
end

aux.RegisterCreatureEvent(4902030, 13, "c4902030_OnDisappear")




--20s计时器消失时刷火堆。数量随机
function c4902021_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if map.GetMapScriptData(MapID,InstanceID,1,9) == 1 then
		local k = math.random(4, 9)
		for i = 1, k do
			local x = math.random(76, 162)
			local z = math.random(334, 424)
			local m52_HuoDui = map.MapCreateColCreature(MapID, InstanceID, 4902023, x, 1010, z, "")
			--unit.AddBuff(MapID, InstanceID, m52_HuoDui, 9440601, m52_HuoDui)     --刷出来的npc火柱自己身上的buff
		end
		map.MapCreateColCreature(MapID, InstanceID, 4902021, 1, 1, 1, "")
	end
end

aux.RegisterCreatureEvent(4902021, 13, "c4902021_OnDisappear")


--玩家不论战斗非战斗时间进入岩浆区会掉血
function aYanJiangQu_OnEnterArea(MapID, InstanceID, RoleID, ObjID)
	if ObjID==1084 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9441101, RoleID)
	end
end

aux.RegisterMapEvent("m52", 7, "aYanJiangQu_OnEnterArea")


--领主死亡后门B打开
function c1535455_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	map.OpenCloseDoor(MapID, InstanceID, 1075, 1)
	--释放全局变量
	map.SetMapScriptData(MapID,InstanceID,1,2, 0)

	--记录BOSS死亡/脱战了
	map.SetMapScriptData(MapID,InstanceID,1,9, 0)

	--删除传送门
	local del_door = map.GetMapScriptData(MapID,InstanceID,1,3)
	map.MapDeleteCreature(MapID, InstanceID, del_door)  --传送完毕删除传送门

end

aux.RegisterCreatureEvent(1535455, 4, "c1535455_OnDie")


--双极领主离开战斗，喊话D，幸存全员强制传送到门B外，门B打开
function c1535455_OnLeaveCombat(MapID, InstanceID, CreatureID)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12712)  --“哈哈哈——怕死的蛆虫，既然怕死就不要再踏入我的领土！”
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	if TeamID ~= nil then
		local Role = {}
			Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
			for i=1, 6 do
				role.RoleGotoNewMap(MapID, InstanceID, Role[i], 3373741864, 116, 1043, 320)
			end
	end
	map.OpenCloseDoor(MapID, InstanceID, 1075, 1)

	--记录BOSS死亡/脱战了
	map.SetMapScriptData(MapID,InstanceID,1,9, 0)

	--删除传送门
	local del_door = map.GetMapScriptData(MapID,InstanceID,1,3)
	map.MapDeleteCreature(MapID, InstanceID, del_door)  --传送完毕删除传送门

end
aux.RegisterCreatureEvent(1535455, 3, "c1535455_OnLeaveCombat")




--锻魂师死亡，刷出双击领主，门B开启
function c1535461_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	map.OpenCloseDoor(MapID, InstanceID, 1075, 1)

	--全局变量，小龙死亡给领主上buff用
	local ObjID = map.MapCreateColCreature(MapID, InstanceID, 1535455, 110, 1000, 385, "")
	map.SetMapScriptData(MapID,InstanceID,1,2, ObjID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 12713)  --”双极领主：这群没用的东西！来吧，我要亲手捏碎你们——“
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID, -1)
end

aux.RegisterCreatureEvent(1535461, 4, "c1535461_OnDie")



--守护幼龙死亡，双机领主得到韧性降低buff一层
function c1535456_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--判断BOSS是否在战斗
	local Boss4_InBattle = map.GetMapScriptData(MapID,InstanceID,1,9)
	if Boss4_InBattle == 1 then
		local Boss4_ID = map.GetMapScriptData(MapID,InstanceID,1,2)
		unit.AddBuff(MapID, InstanceID, Boss4_ID, 9440901, Boss4_ID)
	end

end

aux.RegisterCreatureEvent(1535456, 4, "c1535456_OnDie")


--守护幼龙死亡，双机领主得到韧性降低buff一层
function c1535457_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	--判断BOSS是否在战斗
	local Boss4_InBattle = map.GetMapScriptData(MapID,InstanceID,1,9)
	if Boss4_InBattle == 1 then
		local Boss4_ID = map.GetMapScriptData(MapID,InstanceID,1,2)
		unit.AddBuff(MapID, InstanceID, Boss4_ID, 9440901, Boss4_ID)
	end

end

aux.RegisterCreatureEvent(1535457, 4, "c1535457_OnDie")



