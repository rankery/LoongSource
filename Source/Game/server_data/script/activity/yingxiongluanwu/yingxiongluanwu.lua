--[[固定活动英雄乱武

--活动中玩家数据(主键RoleID, RebornTime, Score，IsDeadLeave, IsInMap, JoinTime)
Act14_RoleData = {} --侠客组
Act15_RoleData = {} --豪杰组
Act16_RoleData = {} --英雄组
Act17_RoleData = {} --天下组

--固定活动已开始的时间（单位是分钟）
act14_LastTime = 0



--NPC传送进入场景todo
function C4500101_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	--判断活动是否已经开始
	local bStart = act.GetActIsStart(14)

	if -1 == TalkIndex then
		--活动已经开始
		if bStart then
			--提示玩家是否进入活动场景

			--如果玩家是死亡后离开，提示玩家再次进入需要花费金钱

		else --活动还没有开始
			--提示玩家活动没有开始todo
		end
	end

	--如果玩家选择确定
	if TalkIndex == 4 then
		local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
		if RoleLevel < 40 then
			--提示玩家等级不够，无法进入活动
		else if RoleLevel < 60 then
			--传送玩家到侠客组

			--如果玩家是死亡后离开，扣除玩家金钱


		else if RoleLevel < 80 then
			--传送玩家到豪杰组

			--如果玩家是死亡后离开，扣除玩家金钱

		else if RoleLevel < 100 then
			--传送玩家到英雄组

			--如果玩家是死亡后离开，扣除玩家金钱

		else
			--传送玩家到天下组

			--如果玩家是死亡后离开，扣除玩家金钱

		end


	end
end

function

--注册玩家进入活动事件
aux.RegisterCreatureEvent(4500101, 7, "C4500101_OnTalk")

--固定活动开始
function act14_OnStart(ActID)
	--初始化活动已进行时间
	act14_LastTime = 0

	--初始化活动中玩家数据
	Act14_RoleData = nil
	Act15_RoleData = nil
	Act16_RoleData = nil
	Act17_RoleData = nil
end

function act14_OnTimerMin(ActID)
	--固定活动已开始+1
	act14_LastTime = act14_LastTime + 1

	--活动开始后的第5整数倍时间（第5、10……50、55分钟）, 存活的玩家积分+5
	if act14_LastTime % 5 == 0 then
		--侠客组
		for k in pairs(Act14_RoleData) do
			if nil == Act14_RoleData[k] then
				break
			end

			--玩家在活动地图中
			if Act14_RoleData[k].IsInMap then
				--玩家没有死亡
				if unit.IsDead(XXXXX, -1, k) then
					Act14_RoleData[k].Score = Act14_RoleData[k].Score + 5
				end
			end
		end

		--豪杰组
		for n in pairs(Act15_RoleData) do
			if nil == Act15_RoleData[n] then
				break
			end

			--玩家在活动地图中
			if Act15RoleData[n].IsInMap then
				--玩家没有死亡
				if unit.IsDead(XXXXX, -1, n) then
					Act15_RoleData[n].Score = Act15_RoleData[n].Score + 5
				end
			end
		end

		--英雄组
		for m in pairs(Act16_RoleData) do
			if nil == Act16_RoleData[m] then
				break
			end

			--玩家在活动地图中
			if Act16_RoleData[m].IsInMap then
				--玩家没有死亡
				if unit.IsDead(XXXXX, -1, m) then
					Act16_RoleData[m].Score = Act16_RoleData[m].Score + 5
				end
			end
		end

		----天下组
		for i in pairs(Act17_RoleData) do
			if nil == Act17_RoleData[i] then
				break
			end

			--玩家在活动地图中
			if Act17_RoleData[i].IsInMap then
				--玩家没有死亡
				if unit.IsDead(XXXXX, -1, i) then
					Act17_RoleData[i].Score = Act17_RoleData[i].Score + 5
				end
			end
		end

		--随机刷新5种不同类型的晶球
		for j=1, 5 do
			local CrystalBallID = math.random(1520001, 1520019)
			--随机一个坐标todo

			--刷出水晶球

		end
	end

	--在活动开始后每隔15分钟，会在活动场景内刷新出1个凶星怪物
	if act14_LastTime % 15 == 0 then
		--豪杰组
		if ActID = 15 then
			map.MapCreateColCreature(MapID, InstanceID, 1520020, XXXX, XXXX, XXXX, 0, "")
		end

		--英雄组
		if ActID = 16 then
			map.MapCreateColCreature(MapID, InstanceID, 1520021, XXXX, XXXX, XXXX, 0, "")
		end

		--天下组
		if ActID = 17 then
			map.MapCreateColCreature(MapID, InstanceID, 1520022, XXXX, XXXX, XXXX, 0, "")
		end
	end

	--每隔10分钟，自动挑选活动场景内的3名玩家，附加上“天命”状态
	if act14_LastTime % 10 == 0 then
		if ActID = 16 then
			local SelectedNum	= 0   		--已经选中的玩家数量（最多三个）
			local CircleNum		= 20		--最大检测次数
			local RoleNum		= table.getn(Act16_RoleData)
			while CircleNum == 0 do
				local Index = math.random(1, RoleNum)
				for k in pairs(Act16_RoleData) do
					Index = Index - 1
					if Index == 0 then
						if Act16_RoleData[k].IsInMap == true and false == unit.IsDead(k) then
							--给玩家添加“天命”状态 todo
							SelectedNum = SelectedNum + 1
							break
						end
					end
				end

				CircleNum = CircleNum - 1
				if SelectedNum == 3 then
					break;
				end
			end
		end

		if ActID = 17 then
			local SelectedNum	= 0   		--已经选中的玩家数量（最多三个）
			local CircleNum		= 20		--最大检测次数
			local RoleNum		= table.getn(Act17_RoleData)
			while CircleNum == 0 do
				local Index = math.random(1, RoleNum)
				for k in pairs(Act17_RoleData) do
					Index = Index - 1
					if Index == 0 then
						if Act17_RoleData[k].IsInMap == true and false == unit.IsDead(k) then
							--给玩家添加“天命”状态 todo
							SelectedNum = SelectedNum + 1
							break
						end
					end
				end

				CircleNum = CircleNum - 1
				if SelectedNum == 3 then
					break;
				end
			end
		end
	end

	--更新玩家积分排序
end

--活动结束
function act14_OnEnd(actID)

end

--注册该活动的相关事件
aux.RegisterActEvent(14, 2, "act14_OnStart") 	--侠客组
aux.RegisterActEvent(15, 2, "act14_OnStart") 	--豪杰组
aux.RegisterActEvent(16, 2, "act14_OnStart") 	--英雄组
aux.RegisterActEvent(17, 2, "act14_Onstart") 	--天下组
aux.RegisterActEvent(14, 4, "act14_OnTimerMin") --侠客组
aux.RegisterActEvent(15, 4, "act14_OnTimerMin") --豪杰组
aux.RegisterActEvent(16, 4, "act14_OnTimerMin") --英雄组
aux.RegisterActEvent(17, 4, "act14_OnTimerMin") --天下组
aux.RegisterActEvent(14, 3, "act14_OnEnd") 		--侠客组
aux.RegisterActEvent(14, 3, "act14_OnEnd")		--豪杰组
aux.RegisterActEvent(14, 3, "act14_OnEnd")		--英雄组
aux.RegisterActEvent(14, 3, "act14_OnEnd")		--天下组

--------------------------------------------------------------------------------------------------------
--todo玩家进入活动场景
function m21_PlayerEnter(MapID, InstanceID, RoleID)
	--给玩家添加一个15秒的无敌buff
	unit.AddBuff(MapID, InstanceID, RoleID,

	--玩家是否是第一次进入活动
	if Act14_RoleData[RoleID] then
		--第一次进入，加入该活动的RoleData表
		local RoleDataTemp = {RebornTime=0, Score=0, IsDeadLeave=false, IsInMap=true, JoinTime=0}
		table.insert(Act14_RoleData, RoleID, RoleDataTemp)
	else
		--复活次数加1
		Act14_RoleData[RoleID].RebornTime = Act14_RoleData[RoleID].RebornTime + 1
		Act14_RoleData[RoleID].IsInMap = true
	end
end

function m22_PlayerEnter(MapID, InstanceID, RoleID)
	--给玩家添加一个15秒的无敌buff, todo
	unit.AddBuff(MapID, InstanceID, RoleID,

	--玩家是否是第一次进入活动
	if Act15_RoleData[RoleID] then
		--第一次进入，加入该活动的RoleData表
		local RoleDataTemp = {RebornTime=0, Score=0, IsDeadLeave=false, IsInMap=true, JoinTime=0}
		table.insert(Act15_RoleData, RoleID, RoleDataTemp)
	else
		--复活次数加1
		Act15_RoleData[RoleID].RebornTime = Act15_RoleData[RoleID].RebornTime + 1
		Act15_RoleData[RoleID].IsInMap = true
	end
end

function m23_PlayerEnter(MapID, InstanceID, RoleID)
	--给玩家添加一个15秒的无敌buff, todo
	unit.AddBuff(MapID, InstanceID, RoleID,

	--玩家是否是第一次进入活动
	if Act16_RoleData[RoleID] then
		--第一次进入，加入该活动的RoleData表
		local RoleDataTemp = {RebornTime=0, Score=0, IsDeadLeave=false, IsInMap=true, JoinTime=0}
		table.insert(Act16_RoleData, RoleID, RoleDataTemp)
	else
		--复活次数加1
		Act16_RoleData[RoleID].RebornTime = Act16_RoleData[RoleID].RebornTime + 1
		Act16_RoleData[RoleID].IsInMap = true
	end
end

function m24_PlayerEnter(MapID, InstanceID, RoleID)
	--给玩家添加一个15秒的无敌buff, todo
	unit.AddBuff(MapID, InstanceID, RoleID,

	--玩家是否是第一次进入活动
	if Act17_RoleData[RoleID] then
		--第一次进入，加入该活动的RoleData表
		local RoleDataTemp = {RebornTime=0, Score=0, IsDeadLeave=false, IsInMap=true, JoinTime=0}
		table.insert(Act17_RoleData, RoleID, RoleDataTemp)
	else
		--复活次数加1
		Act17_RoleData[RoleID].RebornTime = Act17_RoleData[RoleID].RebornTime + 1
		Act17_RoleData[RoleID].IsInMap = true
	end
end

--注册进入活动地图事件
aux.RegisterMapEvent(2983744463, 2, "m21_PlayerEnter")
aux.RegisterMapEvent(2983743695, 2, "m22_PlayerEnter")
aux.RegisterMapEvent(2983743951, 2, "m23_PlayerEnter")
aux.RegisterMapEvent(2983745231, 2, "m24_PlayerEnter")


-------------------------------------------------------------------------------------------------------
--玩家离开活动场景
function m21_PlayerEnter(MapID, InstanceID, RoleID)
	Act14_RoleData[RoleID].IsInMap = false
end

function m22_PlayerEnter(MapID, InstanceID, RoleID)
	Act15_RoleData[RoleID].IsInMap = false
end

function m23_PlayerEnter(MapID, InstanceID, RoleID)
	Act16_RoleData[RoleID].IsInMap = false
end

function m24_PlayerEnter(MapID, InstanceID, RoleID)
	Act17_RoleData[RoleID].IsInMap = false
end

--注册离开活动地图事件
aux.RegisterMapEvent(2983744463, 3, "m21_PlayerEnter")
aux.RegisterMapEvent(2983743695, 3, "m22_PlayerEnter")
aux.RegisterMapEvent(2983743951, 3, "m23_PlayerEnter")
aux.RegisterMapEvent(2983745231, 3, "m24_PlayerEnter")

-------------------------------------------------------------------------------------------------------
--玩家在活动中掉线后上线
function m21_CanTakeOverWhenOnline(MapID, RoleID)
	--修正玩家的上线地图和坐标todo

	return 1, dwOutMapID, x, y, z
end

aux.RegisterMapEvent(2983744463,  12, "m21_CanTakeOverWhenOnline")
aux.RegisterMapEvent(2983743695,  12, "m21_CanTakeOverWhenOnline")
aux.RegisterMapEvent(2983743951,  12, "m21_CanTakeOverWhenOnline")
aux.RegisterMapEvent(2983745231,  12, "m21_CanTakeOverWhenOnline")


-------------------------------------------------------------------------------------------------------
--活动中玩家死亡todo
function m21_RoleDie(MapID, InstanceID, RoleID, KillerID)

end

function m22_RoleDie(MapID, InstanceID, RoleID, KillerID)

end

function m23_RoleDie(MapID, InstanceID, RoleID, KillerID)

end

function m24_RoleDie(MapID, InstanceID, RoleID, KillerID)

end

aux.RegisterMapEvent(2983744463, 5, "m21_RoleDie")
aux.RegisterMapEvent(2983743695, 5, "m22_RoleDie")
aux.RegisterMapEvent(2983743951, 5, "m23_RoleDie")
aux.RegisterMapEvent(2983745231, 5, "m24_RoleDie")]]





















