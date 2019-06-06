
--RoleDataType["WaBao"]               = 1		  --挖宝任务环数
--RoleDataType["WaBao_Total"]         = 2         --挖宝今日总任务环数
--RoleDataType["WaBao_Update"]        = 3         --挖宝任务上次更新时间
--RoleDataType["WaBao_Index"]         = 4         --当前接到的挖宝任务的Index
--RoleDataType["IsDoWaBao"]           = 5         --目前是否接有寻宝类任务

--RoleDataType["IsDoWaBao"]           = 30        --目前是否接有寻宝类任务 0没有 1有
--RoleDataType["PositionA"]           = 31        --挖宝地点坐标a
--RoleDataType["PositionB"]           = 32        --挖宝地点坐标b
--RoleDataType["PositionC"]           = 33        --挖宝地点坐标c
--RoleDataType["PositionMap"]         = 34        --挖宝地点所在场景

--RoleDataType["LongMaiXunBao_Do"]	= 72	--玩家是否触发了龙脉寻宝任务，即同意进入龙脉副本
--RoleDataType["LongMaiXunBao_NPC_X"] = 73
--RoleDataType["LongMaiXunBao_NPC_Y"] = 74
--RoleDataType["LongMaiXunBao_NPC_Z"] = 75

--LUA函数：在可选地图中随机选取一个坐标点,将该坐标点存为全局变量a,b,c
function WaBao_GetPoint(mapID, instanceID, ownerID)
	local level = role.GetRoleLevel(mapID, instanceID, ownerID)

	local temp = math.random(1,3)

	if level>=40 and level<=55 then
		--玄天龙城、凤翔、涿鹿
		if temp==1 then
            local k = math.random(1, 50)	------------------------------------------待修改
			local localA = map_list[3017298127].map[k].x
			local localB = map_list[3017298127].map[k].y
			local localC = map_list[3017298127].map[k].z
			local localMap = 3017298127
           	role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==2 then
			local k = math.random(1, 20)	------------------------------------------待修改
            local localA = map_list[3017298383].map[k].x
			local localB = map_list[3017298383].map[k].y
			local localC = map_list[3017298383].map[k].z
			local localMap = 3017298383
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==3 then
		    local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299663].map[k].x
			local localB = map_list[3017299663].map[k].y
			local localC = map_list[3017299663].map[k].z
			local localMap = 3017299663
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		end
	elseif level>=56 and level<=70 then
		--凤翔、涿鹿、巫水
		if temp==1 then
			local k = math.random(1, 20)	------------------------------------------待修改
            local localA = map_list[3017298383].map[k].x
			local localB = map_list[3017298383].map[k].y
			local localC = map_list[3017298383].map[k].z
			local localMap = 3017298383
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==2 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299663].map[k].x
			local localB = map_list[3017299663].map[k].y
			local localC = map_list[3017299663].map[k].z
			local localMap = 3017299663
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==3 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299919].map[k].x
			local localB = map_list[3017299919].map[k].y
			local localC = map_list[3017299919].map[k].z
			local localMap = 3017299919
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		end
	elseif level>=71 and level<=85 then
		--涿鹿、巫水、武夷
		if temp==1 then
			--武夷共分2个场景m06,m06_1
			local j = math.random(1, 2)
			if j == 1 then
			    local k = math.random(1, 20)	------------------------------------------待修改
			    local localA = map_list[3017299151].map[k].x
			    local localB = map_list[3017299151].map[k].y
			    local localC = map_list[3017299151].map[k].z
			    local localMap = 3017299151
			    role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
			elseif j == 2 then
			    local k = math.random(1, 20)	------------------------------------------待修改
				local localA = map_list[1146339967].map[k].x
			    local localB = map_list[1146339967].map[k].y
			    local localC = map_list[1146339967].map[k].z
			    local localMap = 1146339967
			    role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
			end
		elseif temp==2 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299663].map[k].x
			local localB = map_list[3017299663].map[k].y
			local localC = map_list[3017299663].map[k].z
			local localMap = 3017299663
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==3 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299919].map[k].x
			local localB = map_list[3017299919].map[k].y
			local localC = map_list[3017299919].map[k].z
			local localMap = 3017299919
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		end
	elseif level>=86 then
		--巫水、武夷、昆仑
		if temp==1 then
			--武夷共分2个场景m06,m06_1
			local j = math.random(1, 2)
			if j == 1 then
			    local k = math.random(1, 20)	------------------------------------------待修改
			    local localA = map_list[3017299151].map[k].x
			    local localB = map_list[3017299151].map[k].y
			    local localC = map_list[3017299151].map[k].z
			    local localMap = 3017299151
			    role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
			elseif j == 2 then
			    local k = math.random(1, 20)	------------------------------------------待修改
				local localA = map_list[1146339967].map[k].x
			    local localB = map_list[1146339967].map[k].y
			    local localC = map_list[1146339967].map[k].z
			    local localMap = 1146339967
			    role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
			end
		elseif temp==2 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299919].map[k].x
			local localB = map_list[3017299919].map[k].y
			local localC = map_list[3017299919].map[k].z
			local localMap = 3017299919
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		elseif temp==3 then
			local k = math.random(1, 20)	------------------------------------------待修改
			local localA = map_list[3017299407].map[k].x
			local localB = map_list[3017299407].map[k].y
			local localC = map_list[3017299407].map[k].z
			local localMap = 3017299407
			role.SetRoleScriptData(ownerID, 4, RoleDataType["PositionA"], localA, RoleDataType["PositionB"], localB, RoleDataType["PositionC"], localC, RoleDataType["PositionMap"], localMap)
		end
	end
end

--LUA函数：发送消息告诉客户端
function WaBao_SendRandomResult(TalkCommentID, TargetID)
	local MsgID = msg.BeginMsgEvent()
	--msg.AddMsgEvent(MsgID, 13, 0)
	--添加屏显提示，并具备自动寻路功能
	msg.AddMsgEvent(MsgID, 71, TalkCommentID)
	msg.DispatchRoleMsgEvent(TargetID, MsgID)
end

--LUA函数：判断玩家是否进入了指定区域
function WaBao_EnterNearArea(MapID, InstanceID, TargetID)
    local a,b,c,localMap = role.GetRoleScriptData(TargetID, 4, RoleDataType["PositionA"], RoleDataType["PositionB"], RoleDataType["PositionC"], RoleDataType["PositionMap"])
    --首先判断玩家是否处在目标地图中
    if MapID ~= localMap then
        return false
    end

    --得到玩家当前坐标
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)

	--判断玩家是否在指定坐标点附近
	if (x-a <= 15 and x-a >= -15) and (y-b <= 15 and y-b >= -15) and (z-c <= 15 and z-c >= -15) then
	    return true
	else
	    return false
	end
end

--LUA函数：七星挖宝
function QiXing_DigTreasure(MapID, InstanceID, TargetID)
	local randomCount
	randomCount = math.random(1, 100)

	--一无所获 1%
	if randomCount==1  then
		--向客户端发消息
		WaBao_SendRandomResult(40,TargetID)

	--一袋金银 3%
	elseif randomCount>=2 and randomCount<=5 then
		--根据角色等级，奖励一定数量的金钱
		local level = role.GetRoleLevel(MapID, InstanceID, TargetID)
		if level>=40 and level<=49 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 2500, 104)

		elseif level>=50 and level<=59 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 5000, 104)

		elseif level>=60 and level<=69 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 8000, 104)

		elseif level>=70 and level<=79 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 12000, 104)

		elseif level>=80 and level<=89 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 16000, 104)

		elseif level>=90 and level<=99 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 22000, 104)

		elseif level>=100 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 30000, 104)
		end
		--向客户端发消息
		WaBao_SendRandomResult(41,TargetID)

	--遭遇暗算 1%
	elseif randomCount==6 then
		--玩家的体力、真气、活力、持久力均瞬间减至5%(BUFF 2015601)
		unit.AddBuff(MapID, InstanceID, TargetID, 2015601, TargetID)
		--向客户端发消息
		WaBao_SendRandomResult(42,TargetID)

	--破军出世 2%
	elseif randomCount>=7 and randomCount<=9 then
		--获得破军图(1350002)
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350002, 1, -1, 3, 104)
		--向客户端发消息
		WaBao_SendRandomResult(43,TargetID)

	--尘封之箱 15%
	elseif randomCount>=10 and randomCount<=24 then
		--获得尘封之箱
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350031, 1, -1, 3, 104)
		--向客户端发消息
		WaBao_SendRandomResult(44,TargetID)

	--宝藏守卫 35%
	elseif randomCount>=25 and randomCount<=59 then
		--在挖宝指定坐标地点上，根据等级刷出1个普通怪物
		local level = role.GetRoleLevel(MapID, InstanceID, TargetID)

		local localA,localB,localC,localMap = role.GetRoleScriptData(TargetID, 4, RoleDataType["PositionA"], RoleDataType["PositionB"], RoleDataType["PositionC"], RoleDataType["PositionMap"])

        local InOrOut = math.random(1,2)
		if level>=40 and level<=49 then
		    if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510001, localA, localB, localC)
        else
          map.MapCreateCreature(localMap, nil, 1510008, localA, localB, localC)
        end
		elseif level>=50 and level<=59 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510002, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510009, localA, localB, localC)
      end

		elseif level>=60 and level<=69 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510003, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510010, localA, localB, localC)
      end

		elseif level>=70 and level<=79 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510004, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510011, localA, localB, localC)
      end

		elseif level>=80 and level<=89 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510005, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510012, localA, localB, localC)
      end

		elseif level>=90 and level<=99 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510006, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510013, localA, localB, localC)
      end

		elseif level>=100 then
			if InOrOut == 1 then
			    map.MapCreateCreature(localMap, nil, 1510007, localA, localB, localC)
      else
          map.MapCreateCreature(localMap, nil, 1510014, localA, localB, localC)
      end
end

		--向客户端发消息
		WaBao_SendRandomResult(64,TargetID)

	--上古封印 20%
	elseif randomCount>=60 and randomCount<=79 then
		--根据挖宝者所在场景，在规定场景的策划编辑指定地点刷新出1个七星魔王
		local k = math.random(1,20)	---------------------------------------------------待修改
		local localA = map_list[MapID].map[k].x
		local localB = map_list[MapID].map[k].y
		local localC = map_list[MapID].map[k].z
		--玄天龙城
		if MapID == 3017298127 then
			map.MapCreateCreature(3017298127, nil, 4500003, localA, localB, localC)
		--凤翔
		elseif MapID == 3017298383 then
			map.MapCreateCreature(3017298383, nil, 4500003, localA, localB, localC)
		--逐鹿
		elseif MapID == 3017299663 then
			map.MapCreateCreature(3017299663, nil, 4500003, localA, localB, localC)
		--巫水
		elseif MapID == 3017299919 then
			map.MapCreateCreature(3017299919, nil, 4500003, localA, localB, localC)
		--武夷m06
		elseif MapID == 3017299151 then
			map.MapCreateCreature(3017299151, nil, 4500003, localA, localB, localC)
		--武夷m06_1
		elseif MapID == 1146339967 then
			map.MapCreateCreature(1146339967, nil, 4500003, localA, localB, localC)
		--昆仑
		elseif MapID == 3017299407 then
		 	map.MapCreateCreature(3017299407, nil, 4500003, localA, localB, localC)
		end

		--服务器顶端滚动消息全服广播：
		--“###（挖宝者）在寻宝时不慎将七星魔王放出，已出现于###（场景名称）[xxxx,xxxx](坐标）”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 409)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 6, MapID)
		msg.AddMsgEvent(MsgID, 9, localA)
		msg.AddMsgEvent(MsgID, 9, localC)
		msg.DispatchWorldMsgEvent(MsgID)

		--向客户端发消息
		WaBao_SendRandomResult(47,TargetID)

		--财神下凡 1%
	elseif randomCount == 80 then
		if tempCount == 4 then
			--随机场景的随机位置刷新出1个财神
			local randomNum = math.random(1,6)
			local TempMapID
			if randomNum == 1 then
				TempMapID = 3017298127
			elseif randomNum == 2 then
				TempMapID = 3017298383
			elseif randomNum == 3 then
				TempMapID = 3017299663
			elseif randomNum == 4 then
				TempMapID = 3017299919
			elseif randomNum == 5 then
				TempMapID = 3017299151
			elseif randomNum == 6 then
				TempMapID = 1146339967
			elseif randomNum == 7 then
				TempMapID = 3017299407
			end

			local k = math.random(1, 20)	---------------------------------------------------待修改
			local localA = map_list[TempMapID].map[k].x
			local localB = map_list[TempMapID].map[k].y
			local localC = map_list[TempMapID].map[k].z

			map.MapCreateCreature(TempMapID, nil, 1510021, localA, localB, localC)

			--挖宝者获得一个福缘属性提升的BUFF
			unit.AddBuff(MapID, InstanceID, TargetID, 2015701, TargetID)
			--向客户端发消息
			WaBao_SendRandomResult(48, TargetID)
			--服务器顶端滚动消息全服广播：
			--“财神下凡于##（场景名称），寻访有缘分人！”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 50)
			msg.AddMsgEvent(MsgID, 6, TempMapID)
			msg.AddMsgEvent(MsgID, 9, localA)
			msg.AddMsgEvent(MsgID, 9, localC)
			msg.DispatchWorldMsgEvent(MsgID)
		end

	else
		--七星宝箱 20%
		--获得1个七星宝箱或北斗宝箱，任务结束
		local tempCountI
		tempCountI = math.random(1,2)

		if tempCountI == 1 then
			--获得七星宝箱
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350011, 1, -1, 3, 104)
		else
			--获得北斗宝箱
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350012, 1, -1, 3, 104)
		end

		--向客户度发消息
		WaBao_SendRandomResult(46,TargetID)
	end
end

--LUA函数：璇玑挖宝
function XuanJi_DigTreasure(MapID, InstanceID, TargetID)
	local randomCount
	randomCount = math.random(1, 100)

	--一箱金银4%
	if randomCount >=1 and randomCount<=4 then
		--根据角色等级，奖励一定数量的金钱
		local level = role.GetRoleLevel(MapID, InstanceID, TargetID)
		if level>=40 and level<=49 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 60000, 104)

		elseif level>=50 and level<=59 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 160000, 104)

		elseif level>=60 and level<=69 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 260000, 104)

		elseif level>=70 and level<=79 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 460000, 104)

		elseif level>=80 and level<=89 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 660000, 104)

		elseif level>=90 and level<=99 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 880000, 104)

		elseif level>=100 then
			role.AddRoleSilver(MapID, InstanceID, TargetID, 880000, 104)
		end
		--向客户端发消息
		WaBao_SendRandomResult(51,TargetID)

	--镶金之箱[15%]
	elseif randomCount>=5 and randomCount<=19 then
		--获得镶金箱子（1350032）一个，任务结束
		role.AddRoleItem(MapID, InstanceID, TargetID, 1350032, 1, -1, 3, 104)
		--向客户端发消息
		WaBao_SendRandomResult(52,TargetID)

	--真龙四宝[20%]
	elseif randomCount>=20 and randomCount<=39 then
		--获得真龙四宝（逆天龙鳞1350051、真凰龙须1350052、万邪龙眼1350053、玄金龙爪1350054）其中任意一个，任务结束
		local count = math.random(1,4)
		if count == 1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350051, 1, -1, 3, 104)
			--服务器顶端滚动消息全服广播：
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 54)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350051)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif count == 2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350052, 1, -1, 3, 104)
			--服务器顶端滚动消息全服广播：
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 54)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350052)
			msg.DispatchWorldMsgEvent(MsgID)
		elseif count == 3 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350053, 1, -1, 3, 104)
			--服务器顶端滚动消息全服广播：
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 54)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350053)
			msg.DispatchWorldMsgEvent(MsgID)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350054, 1, -1, 3, 104)
			--服务器顶端滚动消息全服广播：
			--“###（挖宝者姓名）获得真龙四宝之####（道具名称）”
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 100, 54)
			msg.AddMsgEvent(MsgID, 2, TargetID)
			msg.AddMsgEvent(MsgID, 4, 1350054)
			msg.DispatchWorldMsgEvent(MsgID)
		end
		--向客户端发消息
		WaBao_SendRandomResult(53,TargetID)
	--七星宝箱[5%]
	elseif randomCount>=40 and randomCount<=44 then
		--获得5个七星宝箱(1350011)或北斗宝箱(1350012)，任务结束
		local countI = math.random(1,2)
		if countI == 1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350011, 5, -1, 3, 104)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350012, 5, -1, 3, 104)
		end
		--向客户端发消息
		WaBao_SendRandomResult(55,TargetID)

	--杀破狼宝箱[40%]
	elseif randomCount>=45 and randomCount<=84 then
		--获得1个七杀宝箱或破军宝箱、或贪狼宝箱，任务结束
		local countII = math.random(1,3)
		if countII == 1 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350013, 1, -1, 3, 104)
		elseif countII == 2 then
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350014, 1, -1, 3, 104)
		else
			role.AddRoleItem(MapID, InstanceID, TargetID, 1350015, 1, -1, 3, 104)
		end
		--向客户端发消息
		WaBao_SendRandomResult(56,TargetID)
	--上古封印 15%
	elseif randomCount>=85 and randomCount<=99 then
		--事件触发后，根据挖宝者所在场景，在规定场景的策划编辑指定地点刷新出刷新NPC“七星魔王”
		local k = math.random(1,20)	---------------------------------------------------待修改
		local localA = map_list[MapID].map[k].x
		local localB = map_list[MapID].map[k].y
		local localC = map_list[MapID].map[k].z
		--玄天龙城
		if MapID == 3017298127 then
			map.MapCreateCreature(3017298127, nil, 4500003, localA, localB, localC)
		--凤翔
		elseif MapID == 3017298383 then
			map.MapCreateCreature(3017298383, nil, 4500003, localA, localB, localC)
		--逐鹿
		elseif MapID == 3017299663 then
			map.MapCreateCreature(3017299663, nil, 4500003, localA, localB, localC)
		--巫水
		elseif MapID == 3017299919 then
			map.MapCreateCreature(3017299919, nil, 4500003, localA, localB, localC)
		--武夷m06
		elseif MapID == 3017299151 then
			map.MapCreateCreature(3017299151, nil, 4500003, localA, localB, localC)
		--武夷m06_1
		elseif MapID == 1146339967 then
			map.MapCreateCreature(1146339967, nil, 4500003, localA, localB, localC)
		--昆仑
		elseif MapID == 3017299407 then
		 	map.MapCreateCreature(3017299407, nil, 4500003, localA, localB, localC)
		end
		--服务器顶端滚动消息全服广播：
		--###（挖宝者）在寻宝时不慎将七星魔王放出，已出现于###（场景名称）[xxxx,xxxx](坐标）
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 409)
		msg.AddMsgEvent(MsgID, 2, TargetID)
		msg.AddMsgEvent(MsgID, 6, MapID)
		msg.AddMsgEvent(MsgID, 9, localA)
		msg.AddMsgEvent(MsgID, 9, localC)
		msg.DispatchWorldMsgEvent(MsgID)
		--向客户端发消息
		WaBao_SendRandomResult(47,TargetID)
	--财神下凡 1%
	else
		--随机场景的随机位置刷新出1个财神
		--local randomNum = math.random(1,7)
		local randomNum = math.random(1,2)    ----------------------------------------------待修改

		local TempMapID = 0
		if randomNum == 1 then
			TempMapID = 3017298127
		elseif randomNum == 2 then
			TempMapID = 3017298383
		elseif randomNum == 3 then
			TempMapID = 3017299663
		elseif randomNum == 4 then
			TempMapID = 3017299919
		elseif randomNum == 5 then
			TempMapID = 3017299151
		elseif randomNum == 6 then
			TempMapID = 1146339967
		elseif randomNum == 7 then
			TempMapID = 3017299407
		end
		local k = math.random(1, 20)	---------------------------------------------------待修改
		local localA = map_list[TempMapID].map[k].x
		local localB = map_list[TempMapID].map[k].y
		local localC = map_list[TempMapID].map[k].z
		map.MapCreateCreature(TempMapID, nil, 1510021, localA, localB, localC)
		--挖宝者获得一个福缘属性提升的BUFF
		unit.AddBuff(MapID, InstanceID, TargetID, 2015701, TargetID)
		--向客户端发消息
		WaBao_SendRandomResult(48, TargetID)
		--服务器顶端滚动消息全服广播：
		--“财神下凡于##（场景名称），寻访有缘分人！”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 50)
		msg.AddMsgEvent(MsgID, 6, TempMapID)
		msg.AddMsgEvent(MsgID, 9, localA)
		msg.AddMsgEvent(MsgID, 9, localC)
		msg.DispatchWorldMsgEvent(MsgID)
	end
end
