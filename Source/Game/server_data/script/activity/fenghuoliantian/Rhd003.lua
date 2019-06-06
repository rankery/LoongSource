
--统计烽火连天活动中交付“伪情报”的数量
function x20003_Oncomplete(MapID, InstanceID, QuestID, OwnerID, NPCID)
	--累加“伪情报”的数量
	Act4_Gan_Num = Act4_Gan_Num + 1

	--当交付“伪情报”的数量达到50次时，触发“诱敌成功”事件
	if Act4_Gan_Num==50 then
		--提示全体玩家“诱敌之计成功！蛮族军队现已聚集于龙城野外！大家可尽力歼灭！”
		local MsgInfoID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgInfoID, 100, 15)
		msg.DispatchMapMsgEvent(MsgInfoID, MapID, InstanceID)

		--触发事件后，刷新出怪物40只“蛮族援军”
		for i=1, 40 do
			local randPoint = math.random(1, 100)
			local x = map_list[3017298127].map[randPoint].x
			local y = map_list[3017298127].map[randPoint].y
			local z = map_list[3017298127].map[randPoint].z

			--刷出相应怪物
			map.MapCreateCreature(3017298127, -1, 1003163, x, y, z)
		end
	end
	--增加进阶实力值
	Increase_shili(MapID, InstanceID, OwnerID, 0, 1, "jinjieshili_A")

end

--记录“蛮族援军”被击杀数
function c1003163_OnDie(MapID, InstanceID, CreatureID, TypeID, KillerID)
	Act4_Dead_Creature_Num = Act4_Dead_Creature_Num + 1

	--当“蛮族援军”的被击杀数达到35只时，服务器在指定的导航点刷新BOSS级怪物“蛮族统帅”
	if Act4_Dead_Creature_Num == 35 then
		--提示：“蛮族统帅于####、####（坐标）出现！大家可协力除之！”
		local randPoint = math.random(131, 139)
		local x = map_list[3017298127].map[randPoint].x
		local y = map_list[3017298127].map[randPoint].y
		local z = map_list[3017298127].map[randPoint].z

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 16)
		msg.AddMsgEvent(MsgID, 9, x)
		msg.AddMsgEvent(MsgID, 9, z)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		--刷出“蛮族统帅"
		map.MapCreateCreature(MapID, InstanceID, 1003164, x, y, z)
	end
end

--当“蛮族统帅”被击杀后，服务器重新开始记录“伪情报”交付任务的次数以及“蛮族援军”被击杀数。
function c1003164_OnDie(MapID, InstanceID, CreatureID, TypeID, KillerID)
	--初始化诱敌深入任务中交付的伪情报数量
	Act4_Gan_Num = 0
	--初始化蛮族援军被击杀数
	Act4_Dead_Creature_Num = 0
end





--aux.RegisterQuestEvent(20003, 1, "x20003_Oncomplete")
--aux.RegisterCreatureEvent(1003163, 4, "c1003163_OnDie")
--aux.RegisterCreatureEvent(1003164, 4, "c1003164_OnDie")
