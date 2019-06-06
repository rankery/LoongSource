
--ScriptData 第0位为记录击杀了几个机关师和财宝守卫
--ScriptData 第1位为记录各个地图区域是否已触发过
--ScriptData 第2位为记录最终BOSSID
--ScriptData 第3位为记录大门1527是否打开
--ScriptData 第4位为是否刷过器魂BOSS。
--ScriptData 第5位为BOSS是否血低于40%
--ScriptData 第6位为记录另一扇门是否开过
--ScriptData 第7位为记录杀了多少怨念幽魂
--ScriptData 第8位为记录杀了多少潜地妖魔，开了多少墓碑
--ScriptData 第9位为求图看守1id
--ScriptData 第10位为求图看守2id
--ScriptData 第11位为左门是否已封
--ScriptData 第12位为右门是否已封
--ScriptData 第13位为记录杀掉了多少个小怪和BOSS

function m46_OnPlayerEnter(MapID, InstanceID, RoleID)
    local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	end

	local Kanshou1_id = map.GetMapScriptData(MapID, InstanceID, 1, 9)
	local Kanshou2_id = map.GetMapScriptData(MapID, InstanceID, 1, 10)

	if Kanshou1_id==0 and Kanshou2_id==0 then
		Kanshou1_id = map.MapCreateCreature(MapID, InstanceID, 1010408, 261, 3840, 1506)
		Kanshou2_id = map.MapCreateCreature(MapID, InstanceID, 1010409, 969, 3840, 1372)
		map.SetMapScriptData(MapID, InstanceID, 1, 9, Kanshou1_id)
		map.SetMapScriptData(MapID, InstanceID, 1, 10, Kanshou2_id)
	end
end

--注册
aux.RegisterMapEvent("m46", 2, "m46_OnPlayerEnter")



--有钥匙2615703，走到触发器1596处开门1527,进入1530删掉9422001，进入1531删掉9422101
function m46_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
	if ObjID == 1596 then
		local BOOL_EnterTrigger1596 = map.GetMapScriptData(MapID,InstanceID,1,3)
		if BOOL_EnterTrigger1596 == 0 then --判断是否被触发过
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 525)	-- "你必须拥有钥匙才能通过此门。"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
	--[[elseif ObjID == 1530 then
	    local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 94220)
	    if bhave == true then
	        unit.CancelBuff(MapID, InstanceID, RoleID, 9422001)
		end
	elseif ObjID == 1531 then
	    local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 94221)
	    if bhave == true then
	        unit.CancelBuff(MapID, InstanceID, RoleID, 9422101)
		end]]
	end
end
aux.RegisterMapEvent("m46",6,"m46_OnEnterTrigger")

--可否使用函数
function i2615703_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在昆仑密道，则不能使用
	if MapID ~= 3084408015 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x <= 728 or x >= 772 or z <= 338 or z >= 367 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--钥匙使用效果
function i2615703_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.SetMapScriptData(MapID,InstanceID,1,3,1)--记录已经触发过1596
	map.OpenCloseDoor(MapID, InstanceID, 1527, 1)
end

--注册
aux.RegisterItemEvent(2615703, 1, "i2615703_QuestUsable")
aux.RegisterItemEvent(2615703, 0, "i2615703_CanUse")

--完成任务15095刷出宝箱
function q15095_OnComplete(MapID, InstanceID, questID, ownerID, enderID)
	map.MapCreateCreature(MapID, InstanceID, 3075208, 499, 5664, 1759)
end

--注册
aux.RegisterQuestEvent(15095, 1, "q15095_OnComplete")


function m46_OnRoleEnterArea(MapID, InstanceID, RoleID, ObjAreaID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local ADD_record = map.GetMapScriptData(MapID, InstanceID, 1, 1)

	if ObjAreaID==1610 then --进门第一个，刷三个僵尸
		local temp = ADD_record % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010402, 751, 3840, 185)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 727, 3840, 185)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 774, 3840, 185)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+1)
		end

	elseif ObjAreaID==1632 then --进门第二个，刷僵尸
		local temp = math.floor(ADD_record/2) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010402, 736, 3840, 273)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 768, 3840, 273)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 752, 3840, 287)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+2)
		end

	elseif ObjAreaID==1662 then --区域四，刷僵尸小队

		local temp = math.floor(ADD_record/8) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010402, 736, 3840, 617)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 749, 3840, 609)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 761, 3840, 590)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 744, 3840, 593)
			map.MapCreateCreature(MapID, InstanceID, 1010420, 748, 3840, 604)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+8)
		end

	elseif ObjAreaID==1665 then --区域四，刷僵尸小队

		local temp = math.floor(ADD_record/4) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010402, 732, 3840, 679)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 768, 3840, 679)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 751, 3840, 692)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+4)
		end

	elseif ObjAreaID==1692 then --通道五，刷捉脚怪
		local temp = math.floor(ADD_record/16) % 2
		local temp2 = map.GetMapScriptData(MapID, InstanceID, 1, 6)
		if temp == 0 and temp2==1 then
			map.MapCreateCreature(MapID, InstanceID, 1010425, 643, 3840, 710)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 643, 3840, 704)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 643, 3840, 716)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+16)
		end

	elseif ObjAreaID==1693 then --6门前，开门，刷怪
		local temp = math.floor(ADD_record/32) % 2
		local temp2 = map.GetMapScriptData(MapID, InstanceID, 1, 6)
		if temp == 0 and temp2==1 then
			map.OpenCloseDoor(MapID, InstanceID, 1526, 1)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 394, 3840, 700)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 394, 3840, 718)
			map.MapCreateCreature(MapID, InstanceID, 1010402, 394, 3840, 709)
			map.MapCreateCreature(MapID, InstanceID, 1010420, 384, 3840, 705)
			map.MapCreateCreature(MapID, InstanceID, 1010420, 384, 3840, 714)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+32)
		end

	elseif ObjAreaID==1702 then --区域7，刷一堆小怪

		local temp = math.floor(ADD_record/256) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010418, 141, 3840, 683)
			map.MapCreateCreature(MapID, InstanceID, 1010418, 149, 3840, 688)
			map.MapCreateCreature(MapID, InstanceID, 1010418, 146, 3840, 677)
			map.MapCreateCreature(MapID, InstanceID, 1010418, 135, 3840, 676)
			map.MapCreateCreature(MapID, InstanceID, 1010418, 134, 3840, 691)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+256)
		end

	elseif ObjAreaID==1703 then --区域7，刷一堆小怪


	elseif ObjAreaID==1715 then --通道13
		local temp = math.floor(ADD_record/64) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010425, 551, 3840, 1010)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 546, 3840, 1014)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 546, 3840, 1003)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+64)
		end

	elseif ObjAreaID==1716 then --通道13
		local temp = math.floor(ADD_record/128) % 2
		if temp == 0 then
			map.MapCreateCreature(MapID, InstanceID, 1010425, 617, 3840, 1014)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 615, 3840, 1006)
			map.MapCreateCreature(MapID, InstanceID, 1010425, 620, 3840, 1001)
			map.MapCreateCreature(MapID, InstanceID, 1010426, 635, 3840, 1002)
			map.MapCreateCreature(MapID, InstanceID, 1010426, 609, 3840, 1004)
			map.SetMapScriptData(MapID, InstanceID, 1, 1, ADD_record+128)
		end

	elseif ObjAreaID==1754 then --囚徒门

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1748)	--在此处使用破元魔障即可封住囚徒看守逃走的路口！
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif ObjAreaID==1795 then --囚徒门

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1748)	--在此处使用破元魔障即可封住囚徒看守逃走的路口！
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	end


end

aux.RegisterMapEvent("m46", 7, "m46_OnRoleEnterArea")

--破元魔障
function i2615704_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在昆仑密道，则不能使用
	if MapID ~= 3084408015 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		local left_door=0
		local right_door=0

	    if x >= 319 and x <= 349 and z >= 1386 and z <= 1433 then  --如果不在这个坐标范围，则不能使用
	        left_door = 1
	    end

	    if x >= 620 and x <= 642 and z >= 1285 and z <= 1333 then  --如果不在这个坐标范围，则不能使用
	        right_door = 1
	    end

		if left_door==0 and right_door==0 then
			bRet = 43
		end
	end

	--返回
	return bRet, bIgnore
end

function i2615704_OnUse(MapID, InstanceID, TypeID, TargetID)
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	    if x >= 319 and x <= 349 and z >= 1386 and z <= 1433 then
	        map.SetMapScriptData(MapID, InstanceID, 1, 11, 1)
	        map.MapCreateCreature(MapID, InstanceID, 3075220, x, y, z)
	    end

	    if x >= 620 and x <= 642 and z >= 1285 and z <= 1333 then
	        map.SetMapScriptData(MapID, InstanceID, 1, 12, 1)
	        map.MapCreateCreature(MapID, InstanceID, 3075220, x, y, z)
	    end
end

--注册
aux.RegisterItemEvent(2615704, 1, "i2615704_OnUse")
aux.RegisterItemEvent(2615704, 0, "i2615704_CanUse")
