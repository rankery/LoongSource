--¹Ì¶¨»î¶¯¡°³¾ÔµÎ´ÁË¡°

act8_monster = {}
act8_monster[1] = {TypeID=1004621}
act8_monster[2] = {TypeID=1004622}
act8_monster[3] = {TypeID=1004623}
act8_monster[4] = {TypeID=1004624}
act8_monster[5] = {TypeID=1004625}
act8_monster[6] = {TypeID=1004626}
act8_monster[7] = {TypeID=1004627}
act8_monster[8] = {TypeID=1004628}
act8_monster[9] = {TypeID=1004629}
act8_monster[10] = {TypeID=1004630}

act8_npc = {}
act8_npc[1] = {TypeID=3020501, x=888, y=10464, z=2022}
act8_npc[2] = {TypeID=3020502, x=866, y=10464, z=2036}
act8_npc[3] = {TypeID=3020503, x=910, y=10464, z=2036}
act8_npc[4] = {TypeID=3020504, x=906, y=10464, z=2066}
act8_npc[5] = {TypeID=3020505, x=870, y=10464, z=2066}
act8_npc[6] = {TypeID=3020506, x=840, y=10200, z=1034}
act8_npc[7] = {TypeID=3020507, x=498, y=14018, z=835}
act8_npc[8] = {TypeID=3020508, x=2168, y=10960, z=2605}
act8_npc[9] = {TypeID=3020509, x=2426, y=12560, z=1561}
act8_npc[10] = {TypeID=3020510, x=2036, y=7480, z=871}

act8_TimerMin = 0


function act8_OnStart(actID)
	--·þÎñÆ÷·ïÏèÈ«³¡¾°Í¨Öª[ÆÁÄ»¶¥²¿¹ö¶¯ÎÄ×ÖÌáÊ¾]£º¡°Å®æ´ÔªÀÏÑ°·ÃÏÉÔµ£¡´ó¼Ò¿É¸Ï½ôÑ°ÕÒ£¡¡±
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 30)
	msg.DispatchMapMsgEvent(MsgID, 3017298383, -1)

	--Ë¢³ö10¸öÅ®æ´ÔªÀÏNPC
	for k=1, 10 do
		map.MapCreateCreature(3017298383, -1, act8_npc[k].TypeID, act8_npc[k].x, act8_npc[k].y, act8_npc[k].z)
	end

	--¸üÐÂÊ±¼äÖØÖÃÎª0
	act8_TimerMin = 0

	--Ëæ»úË¢³ö30¸öÅ®æ´ÔªÀÏ¹ÖÎï
	for k=1, 30 do
		local Index = math.random(1, 10)
		local x = map_list[3017298383].map[k].x
		local y = map_list[3017298383].map[k].y
		local z = map_list[3017298383].map[k].z

		--Éú³É¹ÖÎï
		local success = map.MapCreateCreature(3017298383, -1, act8_monster[Index].TypeID, x, y, z)

	end
end

function act8_OnTimerMin(actID)

	act8_TimerMin = act8_TimerMin + 1

	local broadmin={}
	broadmin[1] = 9
	broadmin[2] = 19
	broadmin[3] = 29
	broadmin[4] = 39
	broadmin[5] = 49

	local respawnmin={}
	respawnmin[1] = 10
	respawnmin[2] = 20
	respawnmin[3] = 30
	respawnmin[4] = 40
	respawnmin[5] = 50

	for i=1, 5 do
		if act8_TimerMin == broadmin[i] then
			--·þÎñÆ÷·ïÏèÈ«³¡¾°Í¨Öª[ÆÁÄ»¶¥²¿¹ö¶¯ÎÄ×ÖÌáÊ¾]£º¡°·þÎñÆ÷½«ÓÚ1·ÖÖÓºóÖØÐÂË¢ÐÂÅ®æ´ÔªÀÏ¡±
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 32)
			msg.DispatchMapMsgEvent(MsgID, 3017298383, -1)
		end
	end

	for i=1, 5 do
		if act8_TimerMin == respawnmin[i] then
			--Ëæ»úË¢³ö20¸öÅ®æ´ÔªÀÏ¹ÖÎï
			for k=1, 20 do
				local Index = math.random(1, 10)
				local randPoint = math.random(1, 33)
				local x = map_list[3017298383].map[randPoint].x
				local y = map_list[3017298383].map[randPoint].y
				local z = map_list[3017298383].map[randPoint].z

				--Éú³É¹ÖÎï
				local success = map.MapCreateCreature(3017298383, -1, act8_monster[Index].TypeID, x, y, z)

			end
		end
	end
end

--×¢²á¸Ã»î¶¯µÄÏà¹ØÊÂ¼þ
aux.RegisterActEvent(8, 2, "act8_OnStart")
aux.RegisterActEvent(9, 2, "act8_OnStart")
aux.RegisterActEvent(10, 2, "act8_OnStart")
aux.RegisterActEvent(11, 2, "act8_OnStart")

aux.RegisterActEvent(8, 4, "act8_OnTimerMin")
aux.RegisterActEvent(9, 4, "act8_OnTimerMin")
aux.RegisterActEvent(10, 4, "act8_OnTimerMin")
aux.RegisterActEvent(11, 4, "act8_OnTimerMin")






function YuanLao_Dead(MapID, InstanceID, CreatureID, TypeID, KillerID)
	--¼ì²âKillerÊÇ·ñÓÐÐ¡¶Ó
	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, KillerID)

	--Èç¹ûÃ»ÓÐÐ¡¶Ó
	if 4294967295 == TeamID  then
		--¼ÆËã»î¶¯ÈÎÎñ½±Àø
		local sex = role.GetRoleSex(KillerID)


		if sex == 0 then
			-- Å®ÐÔÌí¼Ó¡°°ÙºÏÊ¢¿ª¡±Buff
			unit.AddBuff(MapID, InstanceID, KillerID, 2012401, KillerID)
		elseif sex == 1 then
			-- ÄÐÐÔÌí¼Ó¡°ÉñÏÉ¾ìÂÂ¡±Buff
			unit.AddBuff(MapID, InstanceID, KillerID, 2012301, KillerID)
		end

	else
		--»ñµÃÐ¡¶ÓÍæ¼ÒID
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

		for k in pairs(Role) do
			if 4294967295 ~= Role[k] then

				--¼ÆËã»î¶¯ÈÎÎñ½±Àø
				local sex = role.GetRoleSex(Role[k])
				if sex == 0 then
					-- Å®ÐÔÌí¼Ó¡°°ÙºÏÊ¢¿ª¡±Buff
					unit.AddBuff(MapID, InstanceID, Role[k], 2012401, Role[k])
				elseif sex == 1 then
					-- ÄÐÐÔÌí¼Ó¡°ÉñÏÉ¾ìÂÂ¡±Buff
					unit.AddBuff(MapID, InstanceID, Role[k], 2012301, Role[k])
				end
			end
		end

	end


	--·þÎñÆ÷¹ã²¥¡°###³É¹¦ÌôÕ½Å®æ´ÔªÀÏ###¡±
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 31)
	msg.AddMsgEvent(MsgID, 2, KillerID)
	msg.AddMsgEvent(MsgID, 5, TypeID)
	msg.DispatchMapMsgEvent(MsgID, 3017298383, -1)

	--Ë¢ÐÂÅ®æ´ÔªÀÏ¶ÔÓ¦µÈ¼¶µÄÊÌÎÀºÍËæ´

	local k=math.random(1000)

	if k>15 then

		if TypeID==1004621 or TypeID==1004626 then
			local x,y,z = unit.GetPosition(MapID, InstanceID, CreatureID)
			map.MapCreateCreature(3017298383, -1, 1004631, x+2, y, z+2)
			map.MapCreateCreature(3017298383, -1, 1004632, x-2, y, z-2)
			map.MapCreateCreature(3017298383, -1, 1004632, x+3, y, z-3)
		elseif TypeID==1004622 or TypeID==1004627 or TypeID==1004623 or TypeID==1004628 then
			local x,y,z = unit.GetPosition(MapID, InstanceID, CreatureID)
			map.MapCreateCreature(3017298383, -1, 1004633, x+2, y, z+2)
			map.MapCreateCreature(3017298383, -1, 1004634, x-2, y, z-2)
			map.MapCreateCreature(3017298383, -1, 1004634, x+3, y, z-3)
		elseif TypeID==1004624 or TypeID==1004629 or TypeID==1004625 or TypeID==1004630 then
			local x,y,z = unit.GetPosition(MapID, InstanceID, CreatureID)
			map.MapCreateCreature(3017298383, -1, 1004635, x+2, y, z+2)
			map.MapCreateCreature(3017298383, -1, 1004636, x-2, y, z-2)
			map.MapCreateCreature(3017298383, -1, 1004636, x+3, y, z-3)
		end

	--Ë¢ÐÂÊØ»¤ÉñÊÞ
	else
		local x,y,z = unit.GetPosition(MapID, InstanceID, CreatureID)
		map.MapCreateCreature(3017298383, -1, 1004637, x, y, z)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 1)
		msg.AddMsgEvent(MsgID, 1, 33)
		msg.AddMsgEvent(MsgID, 2, KillerID)
		msg.DispatchMapMsgEvent(MsgID, 3017298383, -1)

	end

end


--×¢²áÐÂËÞËÀÍöÊÂ¼þ
aux.RegisterCreatureEvent(1004621, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004622, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004623, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004624, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004625, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004626, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004627, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004628, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004629, 4, "YuanLao_Dead")
aux.RegisterCreatureEvent(1004630, 4, "YuanLao_Dead")

function YuanLao_CompleteQuest(MapID, InstanceID, questID, ownerID, enderID)
	local sex = role.GetRoleSex(ownerID)
	if sex == 0 then
		-- Å®ÐÔÌí¼Ó¡°°ÙºÏÊ¢¿ª¡±Buff
		unit.AddBuff(MapID, InstanceID, ownerID, 2012401, ownerID)
	elseif sex == 1 then
		-- ÄÐÐÔÌí¼Ó¡°ÉñÏÉ¾ìÂÂ¡±Buff
		unit.AddBuff(MapID, InstanceID, ownerID, 2012301, ownerID)
	end

	local addexp = {}
	addexp[30] = 50
	addexp[31] = 390
	addexp[32] = 780
	addexp[33] = 1170
	addexp[34] = 1560
	addexp[35] = 1950
	addexp[36] = 2340
	addexp[37] = 2730
	addexp[38] = 3120
	addexp[39] = 3510
	addexp[40] = 3900
	addexp[41] = 4290
	addexp[42] = 4680
	addexp[43] = 5070
	addexp[44] = 5460
	addexp[45] = 5850


	local level = role.GetRoleLevel(MapID, InstanceID, ownerID)

	if level>45 then
		level=45
	end

	role.AddRoleExp(MapID, InstanceID, ownerID, addexp[level])
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 29)
	msg.AddMsgEvent(MsgID, 9, addexp[level])	--¾­ÑéÖµ
	msg.DispatchRoleMsgEvent(ownerID, MsgID)


	if questID == 20011 or questID == 20012 or questID == 20013 or questID == 20014 or questID == 20015 then
		Increase_shili(MapID, InstanceID, ownerID, 0, 4, "jinjieshili_A")
	else
		Increase_shili(MapID, InstanceID, ownerID, 0, 5, "jinjieshili_A")
	end
end


--Íê³ÉÈÎÎñÊÂ¼þ
aux.RegisterQuestEvent(20011, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20012, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20013, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20014, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20015, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20016, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20017, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20018, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20019, 1, "YuanLao_CompleteQuest")
aux.RegisterQuestEvent(20020, 1, "YuanLao_CompleteQuest")


