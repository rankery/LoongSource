--ScriptData 索引0 触发器id：739 1008517刷新数量上限：2
--ScriptData 索引1 触发器id：786 1008517刷新数量上限：2
--ScriptData 索引2 触发器id：875 1008517刷新数量上限：3
--ScriptData 索引3 typeID为3060108的npc消失且typeID分别为1008121、1008122、1008123的怪物全部死亡时，在(m18,556,6870,624)位置刷新typeID为1008124的怪物；
--ScriptData 索引4 当玩家调查完typeID分别为3060109东极石、3060110西极石、3060111北极石、3060112南极石这四个物品时，停止物品特效，在(m18,256,15541,580)这个位置刷新boss青婴(typeID：1008106)

--ScriptData 索引5 青婴 怪物ID
--ScriptData 索引6 叱涅 怪物ID
--ScriptData 索引7 地图区域1105 记录触发时间
--ScriptData 索引8 地图区域1106 记录触发时间
--ScriptData 索引9 地图区域1104 记录触发时间

--ScriptData 索引10 记录进入副本后是否刷新了 燃金宝箱 等

--ScriptData 索引11 触发器id：1081 是否已激活
--ScriptData 索引12 触发器id：1084 是否已激活
--ScriptData 索引13 触发器id：1090 是否已激活

--ScriptData 索引14 当前副本是否已使用过 上古妖壶钥匙
--ScriptData 索引15 当前副本是否已刷新过 1008124

--ScriptData 索引16 当前副本是否已出过摧元阵消失提示

m18_Pos = {}
m18_Pos[1] = {x=290,y=9099,z=702}
m18_Pos[2] = {x=292,y=9099,z=704}
m18_Pos[3] = {x=345,y=8644,z=684}
m18_Pos[4] = {x=347,y=8644,z=686}
m18_Pos[5] = {x=450,y=7444,z=673}
m18_Pos[6] = {x=452,y=7444,z=675}
m18_Pos[7] = {x=520,y=6994,z=673}
m18_Pos[8] = {x=522,y=6994,z=675}
m18_Pos[9] = {x=555,y=5849,z=728}
m18_Pos[10] = {x=557,y=5849,z=730}

m18_ChestPos = {}
m18_ChestPos[1] = {x=586,y=18056,z=647}
m18_ChestPos[2] = {x=698,y=20056,z=499}
m18_ChestPos[3] = {x=580,y=22848,z=379}
m18_ChestPos[4] = {x=658,y=24050,z=474}
m18_ChestPos[5] = {x=410,y=27034,z=666}
m18_ChestPos[6] = {x=714,y=16056,z=652}

m18_CurChestPos = {}
--与刷新出的NPC匠心鬼 对话 删除两个 散愁琼浆[2614709],之后NPC消失
function n3060108_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 272)	-- "过不了我这关，大王可是不会出现的。要求很简单，带两个散愁琼浆来，我立马放行。"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		if role.GetRoleItemNum(RoleID, 2614709) > 1 then
			role.RemoveFromRole(MapID, InstanceID, RoleID, 2614709, 2, 101)
			map.MapDeleteCreature(MapID, InstanceID, TargetID)
			local a = map.GetMapScriptData(MapID,InstanceID,1,3) --副本Data索引3
			local b = map.GetMapScriptData(MapID,InstanceID,1,15)
			if a == 1 and b ~= 1 then
				map.MapCreateColCreature(MapID, InstanceID, 1008124, 556, 6870, 624, 1)
				map.SetMapScriptData(MapID,InstanceID,1,15,1)

				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1806)	--元殇魔祖已出现于坐标[556, 624]！
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			else
				map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
			end
		TalkIndex = -1
		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 273)	-- "散愁琼浆呢？还没拿来？"
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		TalkIndex = -1
		end
	end
end
aux.RegisterCreatureEvent(3060108, 7, "n3060108_OnTalk")


function m18_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
--触发器id：739    刷新怪物：滚地魔     怪物typeID：1008517       刷新坐标：(m18,916,13068,596)     怪物类型：碰撞怪         刷新数量上限：2
	if ObjID == 739 then
		local Activated0 = map.GetMapScriptData(MapID,InstanceID,1,0) --索引0数值为0则刷怪
		if Activated0 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,0,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 910, 13068, 596, 1, "B03")
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 922, 13068, 596, 1, "B03")
		end
--触发器id：786    刷新怪物：滚地魔     怪物typeID：1008517       刷新坐标：(m18,680,20050,534)     怪物类型：碰撞怪         刷新数量上限：2
	elseif ObjID == 786 then
		local Activated1 = map.GetMapScriptData(MapID,InstanceID,1,1) -----索引1数值为0则刷怪
		if Activated1 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,1,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 675, 20050, 534, 1, "B03")
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 685, 20050, 534, 1, "B03")
		end
--触发器id：875    刷新怪物：滚地魔     怪物typeID：1008517       刷新坐标：(m18,506,33046,391)     怪物类型：碰撞怪         刷新数量上限：3
	elseif ObjID == 875 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,2) -----索引2数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,2,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 500, 33046, 391, 1, "B03")
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 512, 33046, 391, 1, "B03")
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 506, 33046, 396, 1, "B03")
		end
--[[爆炸触发器 1079 1086 1092 玩家进入触发器后产生爆炸失血效果。
--刷怪触发器 1081 1084 1090 进入后刷怪
1081         698  20050 512         689  20050 500
1084         570  22866 389         571  22866 369      589  22866  389      589    22866   370
1090         410  27052 675         411  27052 658
]]
	elseif ObjID == 1079 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1086 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1092 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1081 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,11)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,11,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 698, 20050, 512, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 689, 20050, 500, 1)
		end
	elseif ObjID == 1084 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,12)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,12,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 570, 22866, 389, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 571, 22866, 369, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 589, 22866, 389, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 589, 22866, 370, 1)
		end
	elseif ObjID == 1090 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,13)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,13,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 410, 27052, 675, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 411, 27052, 658, 1)
		end
	end
end
aux.RegisterMapEvent("m18",6,"m18_OnEnterTrigger")


--物品使用------------------------
--RWY271：魔王深殿
--需求1使用时限道具[深殿令符][2614119]，使用后召唤NPC深殿幽魄3050212
function I2614119_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 3050212, x+2, y, z+2, 1)
end

-- 时限道具[深殿令符][2614119]只能在魔王殿副本内使用
function I2614119_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3000523471 then
			bRet = 43
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2614119, 1, "I2614119_QuestUsable")
aux.RegisterItemEvent(2614119, 0, "I2614119_CanUse")




--"脊骨钥匙"2614118使用后 刷新 3050211	古殿守魂
function I2614118_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 3050211, x+2, y, z+2, 1)
end

-- 时限道具["脊骨钥匙"][2614118]只能在魔王殿副本内使用
function I2614118_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3000523471 then
	bRet = 43
	end
	return bRet, bIgnore
end
aux.RegisterItemEvent(2614118, 1, "I2614118_QuestUsable")
aux.RegisterItemEvent(2614118, 0, "I2614118_CanUse")


--与深殿幽魄 对话 传送至二层 ：魔王深殿 (m18,250,15048,272
function n3050212_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 274)	-- "若英雄想去第二层，我可以助你一臂之力"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3000523471, 250, 15048, 272)
		TalkIndex = -1
	end
end
aux.RegisterCreatureEvent(3050212, 7, "n3050212_OnTalk")




--与刷新出的NPC 3050211	古殿守魂 对话 传送至三层：上古魔殿 776,14068,550
function n3050211_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 275)	-- "若英雄想去第三层，我可以助你一臂之力"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3000523471, 776, 14068, 550)
		TalkIndex = -1
	end
end
aux.RegisterCreatureEvent(3050211, 7, "n3050211_OnTalk")


--玩家可使用BOSS2泯渊掉落的上古妖壶钥匙[2614705]刷新出
--在(m18,555,6849,661)位置刷新typeID为3060108的npc
--在(m18,290，9099，702)、(m18,345,8644,684)、(m18,450,7444,673)、(m18,520,6994,673)、(m18,555,5849,728)五个位置随机刷新typeID分别为1008121、1008122、1008123的怪物，刷新的数量2个(若能出现不同typeID的组合更佳)；

function I2614705_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.MapCreateColCreature(MapID, InstanceID, 3060108, 555, 6849, 661, 1)
	for i = 1,10 do
		local a = math.random(1,3)
		map.MapCreateColCreature(MapID, InstanceID, 1008120+a, m18_Pos[i].x, m18_Pos[i].y, m18_Pos[i].z, 1)
	end

	map.SetMapScriptData(MapID,InstanceID,1,3,11)--------------------添加完副本兑换后修改为11
	map.SetMapScriptData(MapID,InstanceID,1,14,1)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1807)	--无数匠心妖魔从壶中呼啸而出，飞往了跳台处！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

--上古妖壶钥匙[2614705]只能在上古妖壶附近[320,13700,784]使用
function I2614705_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3000523471 then
			bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
        if x<=300 or x>=340 or z<=764 or z>=800 then  --必须在该范围内使用
            bRet = 43
		else
			local canuse = map.GetMapScriptData(MapID,InstanceID,1,14)
			if canuse == 1 then
			bRet = 32
			end
        end
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2614705, 1, "I2614705_QuestUsable")
aux.RegisterItemEvent(2614705, 0, "I2614705_CanUse")

function c3060115_On_Disappear(MapID, InstanceID, CreatureID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1808)	--摧元阵已经消失...
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

aux.RegisterCreatureEvent(3060115, 13, "c3060115_On_Disappear")

--使用物品 缚鬼索[2614707] 为 [青婴][1008106]添加BUFF[3304601]
--使用物品 捆魔绳[2614708] 为 [叱涅][1008105]添加BUFF[3304601]
function I261470x_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	if TypeID == 2614707 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,5)
		unit.AddBuff(MapID, InstanceID, a, 3304601, a)
	elseif TypeID == 2614708 then
		local b = map.GetMapScriptData(MapID,InstanceID,1,6)
		unit.AddBuff(MapID, InstanceID, b, 3304601, b)
	end
end

--缚鬼索[2614707],捆魔绳[2614708]只能在m18副本内使用
function I261470x_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3000523471 then
			bRet = 43
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2614707, 1, "I261470x_QuestUsable")
aux.RegisterItemEvent(2614707, 0, "I261470x_CanUse")
aux.RegisterItemEvent(2614708, 1, "I261470x_QuestUsable")
aux.RegisterItemEvent(2614708, 0, "I261470x_CanUse")



--[蚀偻]1008101死亡后 添加BUFF[9344601] 给击杀玩家
function c1008101_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 9344601, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1800)	--魔蛛瞳的减速效果已解除，您现在已可通过旁边的跳台！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end
aux.RegisterCreatureEvent(1008101,4,"c1008101_OnDie")

--1008121死亡计数
function c1008121_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,3) --副本Data索引3
	local b = map.GetMapScriptData(MapID,InstanceID,1,15) -- 索引15
	if a == 1 and b~= 1 then
		map.MapCreateColCreature(MapID, InstanceID, 1008124, 556, 6870, 624, 1)
		map.SetMapScriptData(MapID,InstanceID,1,15,1)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1806)	--元殇魔祖已出现于坐标[556, 624]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	else
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	end
end
aux.RegisterCreatureEvent(1008121,4,"c1008121_OnDie")

--1008122死亡计数
function c1008122_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,3) --副本Data索引3
	local b = map.GetMapScriptData(MapID,InstanceID,1,15) --索引15
	if a == 1 and b ~= 1 then
		map.MapCreateColCreature(MapID, InstanceID, 1008124, 556, 6870, 624, 1)
		map.SetMapScriptData(MapID,InstanceID,1,15,1)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1806)	--元殇魔祖已出现于坐标[556, 624]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	else
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	end
end
aux.RegisterCreatureEvent(1008122,4,"c1008122_OnDie")

--1008123死亡计数
function c1008123_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,3) --副本Data索引3
	local b = map.GetMapScriptData(MapID,InstanceID,1,15) --索引15

	if a == 1 and b ~= 1 then
		map.MapCreateColCreature(MapID, InstanceID, 1008124, 556, 6870, 624, 1)
		map.SetMapScriptData(MapID,InstanceID,1,15,1)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1806)	--元殇魔祖已出现于坐标[556, 624]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	else
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	end
end
aux.RegisterCreatureEvent(1008123,4,"c1008123_OnDie")


--当玩家调查完typeID分别为3060109东极石、3060110西极石、3060111北极石、3060112南极石这四个物品
--在(m18,256,15541,580)这个位置刷新boss青婴(typeID：1008106)，

--调查东极石
function c3060109_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		if a == 3 then
			local b = map.MapCreateColCreature(MapID, InstanceID, 1008106, 256, 15541, 580, 1)
			map.SetMapScriptData(MapID,InstanceID,1,5,b)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1801)	--青婴已出现于坐标[256, 580]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		else
			if a == nil then
				a = 0
			end
			map.SetMapScriptData(MapID,InstanceID,1,4,a+1)

		end

end
aux.RegisterCreatureEvent(3060109, 6, "c3060109_OnInvest")

--调查西极石
function c3060110_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		if a == 3 then
			local b = map.MapCreateColCreature(MapID, InstanceID, 1008106, 256, 15541, 580, 1)
			map.SetMapScriptData(MapID,InstanceID,1,5,b)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1801)	--青婴已出现于坐标[256, 580]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		else
			if a == nil then
				a = 0
			end
			map.SetMapScriptData(MapID,InstanceID,1,4,a+1)

		end

end
aux.RegisterCreatureEvent(3060110, 6, "c3060110_OnInvest")

--调查北极石
function c3060111_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		if a == 3 then
			local b = map.MapCreateColCreature(MapID, InstanceID, 1008106, 256, 15541, 580, 1)
			map.SetMapScriptData(MapID,InstanceID,1,5,b)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1801)	--青婴已出现于坐标[256, 580]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		else
			if a == nil then
				a = 0
			end
			map.SetMapScriptData(MapID,InstanceID,1,4,a+1)

		end

end
aux.RegisterCreatureEvent(3060111, 6, "c3060111_OnInvest")

--调查南极石
function c3060112_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)

		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		if a == 3 then
			local b = map.MapCreateColCreature(MapID, InstanceID, 1008106, 256, 15541, 580, 1)
			map.SetMapScriptData(MapID,InstanceID,1,5,b)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1801)	--青婴已出现于坐标[256, 580]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		else
			if a == nil then
				a = 0
			end
			map.SetMapScriptData(MapID,InstanceID,1,4,a+1)

		end

end
aux.RegisterCreatureEvent(3060112, 6, "c3060112_OnInvest")

--青婴1008106死亡后，在(m18,308,15541,579)位置刷新typeID为 1008105的怪物，并在(m18,319,15541,571)位置刷新typeID为3060116 （青婴宝箱）；
function c1008106_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.MapCreateColCreature(MapID, InstanceID, 1008105, 308, 15541, 579, 1)
	map.SetMapScriptData(MapID,InstanceID,1,6,a)
	map.MapCreateColCreature(MapID, InstanceID, 3060116, 319, 15541, 571, 1)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1802)	--叱涅已出现于坐标[308, 579]！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end
aux.RegisterCreatureEvent(1008106,4,"c1008106_OnDie")


--咆哮天尊（1008302）死后刷新鬼斧战神（1008306,--567 5754 203
function c1008302_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1008306, 567, 5754, 203, 1)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008302,4,"c1008302_OnDie")


--音杀魔君（1008303）死后刷新魔音天君（1008308）--729 11753  240
function c1008303_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1008308, 729, 11753, 240, 1)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008303,4,"c1008303_OnDie")


--怒焰猎手（1008304）死后刷新怒焰天君（1008307）--806 14748 349
function c1008304_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 1008307, 808, 14748, 347, 1)
	Increase_shili(MapID, InstanceID, RoleID, 1, 3, "jinjieshili")

end
aux.RegisterCreatureEvent(1008304,4,"c1008304_OnDie")


--调查镇魔石A
function c3060117_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 9345401, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1803)	--咆哮天尊/鬼斧战神的战魂状态已失效！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(3060117, 6, "c3060117_OnInvest")

--调查镇魔石B
function c3060118_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 9345501, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1804)	--怒焰猎手/怒焰天君的战魂状态已失效！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(3060118, 6, "c3060118_OnInvest")

--调查镇魔石C
function c3060119_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	unit.AddBuff(MapID, InstanceID, RoleID, 9345601, RoleID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1805)	--音杀魔君/魔音天君的啸浪状态已失效！
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(3060119, 6, "c3060119_OnInvest")




--失血  进入1105 1106区域内，每3秒获得一次失血DEBUFF，失血50点。
--心智伤害加强  玩家进入区域1104范围内，每3秒获得一次心智伤害加强DEBUFF，收到心智攻击增加50点额外伤害。

function m18_OnEnterArea(MapID, InstanceID, RoleID, ObjID)
	--当角色进入objID为1033的地图区域时，将角色传送到(m18,798,4554,784)这个位置
	if ObjID == 1033 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3000523471, 798, 4554, 784)
	elseif ObjID == 1105 then
		local lTime = map.GetMapScriptData(MapID,InstanceID,1,7) --副本Data索引 7
		if lTime == nil then
			lTime = 0
		end
		local cTime = os.time() --当前时间
		local dTime = cTime - lTime --上次触发距现在时间
		if dTime > 7 then --判断触发间隔是否大于7秒钟
			unit.AddBuff(MapID, InstanceID, RoleID, 9345101, RoleID)
			map.SetMapScriptData(MapID,InstanceID,1,7,cTime)
		end
	elseif ObjID ==1106 then
		local lTime = map.GetMapScriptData(MapID,InstanceID,1,8) --副本Data索引 8
		if lTime == nil then
			lTime = 0
		end
		local cTime = os.time() --当前时间
		local dTime = cTime - lTime --上次触发距现在时间
		if dTime > 7 then --判断触发间隔是否大于7秒钟
			unit.AddBuff(MapID, InstanceID, RoleID, 9345101, RoleID)
			map.SetMapScriptData(MapID,InstanceID,1,8,cTime)
		end
	elseif ObjID ==1104 then
		local lTime = map.GetMapScriptData(MapID,InstanceID,1,9) --副本Data索引 9
		if lTime == nil then
			lTime = 0
		end
		local cTime = os.time() --当前时间
		local dTime = cTime - lTime --上次触发距现在时间
		if dTime > 7 then --判断触发间隔是否大于7秒钟
			unit.AddBuff(MapID, InstanceID, RoleID, 9345201, RoleID)
			map.SetMapScriptData(MapID,InstanceID,1,9,cTime)
		end
	end
end

aux.RegisterMapEvent("m18", 7, "m18_OnEnterArea")

--[[爆炸触发器 1079 1086 1092 玩家进入触发器后产生爆炸失血效果。
--刷怪触发器 1081 1084 1090 进入后刷怪
1081         698  20050 512         689  20050 500
1084         570  22866 389         571  22866 369      589  22866  389      589    22866   370
1090         410  27052 675         411  27052 658
]]
--[[function m18_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)

	if ObjID == 1079 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1086 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1092 then
		unit.AddBuff(MapID, InstanceID, RoleID, 9345301, RoleID)
	elseif ObjID == 1081 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,11)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,11,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 698, 20050, 512, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 689, 20050, 500, 1)
		end

	elseif ObjID == 1084 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,12)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,12,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 570, 22866, 389, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 571, 22866, 369, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 589, 22866, 389, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 589, 22866, 370, 1)
		end
	elseif ObjID == 1090 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,13)
		if a == 0 then
			map.SetMapScriptData(MapID,InstanceID,1,13,1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 410, 27052, 675, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1008514, 411, 27052, 658, 1)
		end
	end
end
aux.RegisterMapEvent("m18",6,"m18_OnEnterTrigger")]]



--[[宝箱位置    六个点三种宝箱随机刷出   燃金宝箱（3060105）一个  阴暗铁箱（3060106）两个  伪玄冰宝箱（3060120）三个
586  18056 647
698  20056 499
580  22848 379
658  24050 474
410  27034 666
714  16056 652
]]

function m18_OnPlayerEnter(MapID, InstanceID,RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,10)
	if a == 0 or nil then
		map.SetMapScriptData(MapID,InstanceID,1,10,1)
		m18_CurChestPos[InstanceID] = deepcopy(m18_ChestPos)

		for i = 1,3 do
			local Index1 = math.random(1,7-i)
			local Temp1 = table.remove(m18_CurChestPos[InstanceID],m18_CurChestPos[InstanceID].Index1)
			map.MapCreateColCreature(MapID, InstanceID, 3060120, Temp1.x, Temp1.y, Temp1.z, 1)
		end

		for i = 1,2 do
			local Index2 = math.random(1,4-i)
			local Temp2 = table.remove(m18_CurChestPos[InstanceID],m18_CurChestPos[InstanceID].Index2)
			map.MapCreateColCreature(MapID, InstanceID, 3060120, Temp2.x, Temp2.y, Temp2.z, 1)
		end

		local Index3 = 1
		local Temp3 = table.remove(m18_CurChestPos[InstanceID],m18_CurChestPos[InstanceID].Index3)
		map.MapCreateColCreature(MapID, InstanceID, 3060120, Temp3.x, Temp3.y, Temp3.z, 1)
	end
--进入副本时删除玩家的坐骑buff
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m18", 2, "m18_OnPlayerEnter")

--当角色进入副本后，无法使用部分技能
--m18_SkillLimit = {}
--m18_SkillLimit[11005] = true
--m18_SkillLimit[11009] = true
--m18_SkillLimit[11012] = true
--m18_SkillLimit[11014] = true
--m18_SkillLimit[90012] = true

--function m18_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m18_SkillLimit[SkillTypeID] then
--        return false
--    end
 --   return true
--end

--aux.RegisterMapEvent("m18", 21, "m18_CanUseSkill")



