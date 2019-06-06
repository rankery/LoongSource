FX_Normal_Guild_Mem		=	40			-- 城战时普通帮派进入人数上限
FX_Hold_Guild_Mem		=	80			-- 城战时统治帮派进入人数上限
--CityStruggle_Accept_Time = {7200}  	    -- 活动最长持续7200秒

-- 1为防御联盟占领状态，0为进攻联盟占领状态，2为不小心进来的路人
LongZhu_State = {LongZhuID, GuildID, Belong, ChangeMin, RoleID}	-- 占领龙珠的帮派ID， 龙珠所属的阵营， 龙珠阵营转换的时间（单位：分钟）， 正在封印的玩家ID
YinYangZhu_State = {}

Door_State = {}
Door_State[1] = {CreatureID, x, y, z, Opened, Change_Min, Repair_Time, HpNode}		-- 正门
Door_State[2] = {CreatureID, x, y, z, Opened, Change_Min, Repair_Time, HpNode}		-- 西门
Door_State[3] = {CreatureID, x, y, z, Opened, Change_Min, Repair_Time, HpNode}		-- 东门

-- Door_State = {Opened, Change_Min, Repair_Time}	-- 门的打开状态，被击破的时间，重修的次数

Material_State = {}		-- 物资库的毁坏状态，被击毁的时间

-- 阴阳柱TypeID
-- 4900030: // 玄机阴阳柱
-- 4900043: // 太冲阴阳柱
-- 4900044: // 天伏阴阳柱
-- 4900045: // 申火阴阳柱
-- 4900046: // 刑妄阴阳柱
YinYangZhuTypeID = {4900030, 4900043, 4900044, 4900045, 4900046}

-- 招魂幡
Atk_ZhaoHunFan = {CreatureID, use, x, y, z}					-- 招魂幡是否已使用，坐标点
Def_ZhaoHunFan = {CreatureID, use, x, y, z}

-- 进攻防御联盟阵营玩家	1--防御联盟	0--进攻联盟
Role_Alliance = {}

-- 城战开始结束标志	0为未开始（结束），1为开始（结束）
CityStruggle_IsStart = 0
CityStruggle_IsOver = 1

-- 活动全局时间（分钟）
CityStruggle_CurMin = 0

-- 城战活动id
CityStruggle_ActID = 0

-- 阵营复活时间
Atk_Revive_Time = 0
Def_Revive_Time = 0

-- 守护神状态
Eudemon_State = {EudemonTally, NeedPerson, NeedSecond, NowPerson, BeginSummon, Summoned}

-- 记录当前所有召唤人的RoleID
Eudemon_Stone = {}

-- 两方势力的人数
local PlayerNum = {}
PlayerNum[0] = 0
PlayerNum[1] = 0

-- 城战判断敌友
function CityStruggle_FriendEnemy(MapID, InstanceID, SrcID, TargetID)
	-- 城战未正式开始时无法进行任何攻击
	if CityStruggle_IsStart ~= 1 then
	    if true == unit.IsPlayer(SrcID) and true == unit.IsCreature(TargetID) then
		    local TypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
			local Role_GuildID = guild.GetRoleGuildID(SrcID)
			local Master_GuilID = city.GetCityAppointedAtt(3, 2)
			local Role_ZhiWei = -1
			if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
				Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, SrcID)
			end
			if Role_ZhiWei ~= -1 and Role_ZhiWei ==2 and TypeID == 1534162 and Role_GuildID == Master_GuilID then
			    return false, true, false, true
			elseif (Role_ZhiWei ~=2 or Role_GuildID ~= Master_GuilID) and TypeID == 1534162 then
                local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 3447)  --只有城主才可以攻击裂地魔王
				msg.DispatchRoleMsgEvent(SrcID, MsgID)
				return true, false, false, true
			elseif TypeID == 1534163 or TypeID == 1534164 or TypeID == 1534165 then
			    return false, true, false, true
			elseif TypeID == 4900710 then
			    return false, false, true, false
			end
		elseif true == unit.IsCreature(SrcID) then
		    local TypeID = unit.GetUnitTypeID(MapID, InstanceID, SrcID)
		    if TypeID <= 1534165 and TypeID >= 1534162 then
			    if true == unit.IsCreature(TargetID) then
				    return true, false, false, true
				elseif true == unit.IsPlayer(TargetID) then
				    return false, true, false, true
				end
			end
		elseif true == unit.IsPlayer(SrcID) and true == unit.IsPlayer(TargetID) then
		    return true, false, false, true
		end
		return false, false, false, true
	end

    -- 玩家间的敌友判断
    if true == unit.IsPlayer(SrcID) and true == unit.IsPlayer(TargetID) then
        if Role_Alliance[SrcID] ~= Role_Alliance[TargetID] then
            return false, true, false, true
        end
	    return true, false, false, true
    end

    -- 玩家与防御联盟召唤兽，卫兵，物资库，招魂幡的敌友判断
    if true == unit.IsPlayer(SrcID) and true == unit.IsCreature(TargetID) then
		-- 龙珠  阴阳柱为中立
		local UnitTypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)
		if UnitTypeID == 4900029 or UnitTypeID == 4900030 or UnitTypeID == 4900043 or UnitTypeID == 4900044 or UnitTypeID == 4900045 or UnitTypeID == 4900046 then
			return false, true, true, true
		end

        if Role_Alliance[SrcID] == 0	then		-- 进攻联盟  （此处还需判断招魂幡的敌友关系）

			if UnitTypeID == 4900027 then			-- 物资库
					return false, true, false, true
			elseif UnitTypeID == 4900028 then		-- 招魂幡
				if TargetID == Atk_ZhaoHunFan.CreatureID then
					return true, false, false, true
				else
					return false, true, false, true
				end
			else
				return false, true, false, true
			end

		else										-- 防御联盟

			if UnitTypeID == 4900027 then			-- 物资库
				return true, false, false, true
			elseif UnitTypeID == 4900028 then		-- 招魂幡
				if TargetID == Def_ZhaoHunFan.CreatureID then
					return true, false, false, true
				else
					return false, true, false, true
				end
			elseif UnitTypeID == 4900049 or UnitTypeID == 4900050 then		-- 石料
				return false, false, true, false
			else
				return true, false, false, true
			end

        end

    end

    if true == unit.IsCreature(SrcID) and true == unit.IsPlayer(TargetID) then
		local UnitTypeID = unit.GetUnitTypeID(MapID, InstanceID, SrcID)
		if UnitTypeID == 4900028 then										-- 招魂幡的光环buff
			if SrcID == Atk_ZhaoHunFan.CreatureID and Role_Alliance[TargetID] == 0 then
				return true, false, false, true
			elseif SrcID == Def_ZhaoHunFan.CreatureID and Role_Alliance[TargetID] == 1 then
				return true, false, false, true
			else
				return false, false, false, false
			end

        elseif Role_Alliance[TargetID] == 0 then
            return false, true, false, true
        end
	    return true, false, false, true
    end


    return false, false, false, false
end

-- 活动每分钟更新
function CityStruggle_OnTimerMin(actID)
	if CityStruggle_IsOver == 1 then
		return
	end

	CityStruggle_CurMin = CityStruggle_CurMin + 1

	-- 开始后龙珠一小时内没发生所属变化，则防御联盟胜利60
	if LongZhu_State.GuildID == -1 and CityStruggle_CurMin >= 60 then
		CityStruggle_IsOver = 1
		CityStruggle_OnOver()
	end

	-- 封印龙珠后保持20分钟，城战结束
	if LongZhu_State.ChangeMin ~= nil and (CityStruggle_CurMin - LongZhu_State.ChangeMin >= 20) then
		CityStruggle_IsOver = 1
		CityStruggle_OnOver()
	end

	-- 每15分钟，防御联盟士气+5
	if math.fmod(CityStruggle_CurMin, 15) == 0 then
		for k, v in pairs(Role_Alliance) do
			if v == 1 then
				unit.ModAttValue(city_id[actID].MapID, -1, k, 63, 5)
				FX_ResetMorale(city_id[actID].MapID, -1, k)
			end
		end
	end

	-- 物资库被击毁过
	local bWuZiKuNumChanged = 0
	for k, v in pairs(Material_State) do
		if Material_State[k].Destroied == 1 then
			--[[if CityStruggle_CurMin - Material_State[k].Change_Min == 5 then
				-- 恢复物资库被击破对进攻联盟的复活时间惩罚
				Atk_Revive_Time = Atk_Revive_Time - 20
				city.SetReviveTime(city_id[actID].CityID, 0, Atk_Revive_Time)
			end
			]]

			if CityStruggle_CurMin - Material_State[k].Change_Min == 5 then
				-- 重新刷新物资库
				map.MapCreateCreature(city_id[actID].MapID, -1, 4900027, Material_State[k].x, Material_State[k].y, Material_State[k].z)

				-- 恢复物资库被击破对进攻联盟的复活时间惩罚
				Def_Revive_Time = Def_Revive_Time - 5
				city.SetReviveTime(city_id[actID].CityID, 1, Def_Revive_Time)

				Material_State[k].Destroied = 0

				-- 继续广播
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101, 1038)			-- 防御联盟的一座物资库已重新刷新
				msg.DispatchMapMsgEvent(MsgID, city_id[actID].MapID, -1)

				-- 标识物资库数量增加
				bWuZiKuNumChanged = 1
			end
		end
	end

	-- 同步物资库数量
	if bWuZiKuNumChanged == 1 then
		local wuzikuNum = 0
		for k, v in pairs(Material_State) do
			if Material_State[k].Destroied ~= 1 then
				wuzikuNum = wuzikuNum + 1
			end
		end
		SycAllRoleOneInfo(city_id[actID].MapID, -1, 5, 0, wuzikuNum)
	end

	local DoorCreatureID = city_door[city_id[actID].DefenceLevel].CreatureID

	-- 城门被攻破过
	for k, v in pairs(Door_State) do
		if Door_State[k].Opened == 1 then
			if Door_State[k].Repair_Time < 3 then
				if CityStruggle_CurMin - Door_State[k].Change_Min == 20 then
					Door_State[k].CreatureID = map.MapCreateCreature(city_id[actID].MapID, -1, DoorCreatureID, Door_State[k].x, Door_State[k].y, Door_State[k].z)
					Door_State[k].Repair_Time = Door_State[k].Repair_Time + 1
					Door_State[k].HpNode = math.floor(math.pow(0.5, Door_State[k].Repair_Time) * 9)
					local hp = (1 - math.pow(0.5, Door_State[k].Repair_Time)) * unit.GetAttValue(city_id[actID].MapID, -1, Door_State[k].CreatureID, 8)
					unit.ChangeHP(city_id[actID].MapID, -1, Door_State[k].CreatureID, -hp)
					-- 关闭城门
					map.OpenCloseDoorByPos(city_id[actID].MapID, -1, Door_State[k].x, Door_State[k].y, Door_State[k].z, false)

					-- 还要广播
					local id_msg = 1034+k-1
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, id_msg)			-- 摧毁了一座防御联盟的物资库
					msg.DispatchMapMsgEvent(MsgID, city_id[actID].MapID, -1)

					-- 同步城门状态给地图所有玩家
					SycAllRoleOneInfo(city_id[actID].MapID, -1, 7, k, 0)
				end
			end
		end
	end

	-- 活跃时间加1
	for k,v in pairs(Role_Alliance) do
		city.AddRoleData(city_id[actID].MapID, -1, city_id[actID].CityID, k, 0, 1)
	end

	-- 刷新战绩排名
	city.SortByScore(city_id[actID].CityID)

end

-- 初始化
function CityStruggle_OnInit(actID)
	--act.InitEventTime("CityStruggle_Accept_Time", 1, actID)
end

-- 计时器
function CityStruggle_OnTimer(actID, Second)
end

-- 活动开始
function CityStruggle_OnStart(actID)

	CityStruggle_ActID = actID
	CityStruggle_CurMin = 0
	Role_Alliance = {}

	-- 城战开始
	city.SetCityStruggleState(city_id[actID].CityID, 1)
	CityStruggle_IsStart = 1
	CityStruggle_IsOver = 0

	-- 重置龙珠和阴阳柱状态
	LongZhu_State.GuildID = -1		-- 龙珠占领帮派ID初始化为-1
	LongZhu_State.Belong = 1		-- 龙珠初始为防御联盟所有
	LongZhu_State.RoleID = nil		-- 龙珠正在封印的玩家

	-- 初始化阴阳柱
	YinYangZhu_State[4900030] = 1
	YinYangZhu_State[4900043] = 1
	YinYangZhu_State[4900044] = 1
	YinYangZhu_State[4900045] = 0
	YinYangZhu_State[4900046] = 0

	-- 重置物资库的状态
	Material_State[1] = {Destroied, Change_Min, x, y, z}
	Material_State[2] = {Destroied, Change_Min, x, y, z}
	Material_State[3] = {Destroied, Change_Min, x, y, z}
	Material_State[4] = {Destroied, Change_Min, x, y, z}

	Material_State[1].Destroied = 0
	Material_State[1].Change_Min = 0
	Material_State[2].Destroied = 0
	Material_State[2].Change_Min = 0
	Material_State[3].Destroied = 0
	Material_State[3].Change_Min = 0
	Material_State[4].Destroied = 0
	Material_State[4].Change_Min = 0

	-- 重置招魂幡使用状态
	Atk_ZhaoHunFan.use = 0
	Atk_ZhaoHunFan.x = 0
	Atk_ZhaoHunFan.y = 0
	Atk_ZhaoHunFan.z = 0
	Def_ZhaoHunFan.use = 0
	Def_ZhaoHunFan.x = 0
	Def_ZhaoHunFan.y = 0
	Def_ZhaoHunFan.z = 0

	-- 重置守护石状态
	Eudemon_Stone = {}

	-- 刷出卫兵
	for k, v in pairs(city_guard[city_id[actID].GuardIndex]) do
		map.MapDeleteCreature(city_id[actID].MapID, -1, v.CreatureID)	-- 删除塑像
		map.MapCreateCreature(city_id[actID].MapID, -1, 1300948, v.x, v.y, v.z)	-- 刷出卫兵
	end

	-- 重置阵营复活时间
	local AtkYinYangNum = 0
	local DefYinYangNum = 0
	for k, v in pairs(YinYangZhu_State) do
		if v == 0 then
			AtkYinYangNum = AtkYinYangNum + 1
		else
			DefYinYangNum = DefYinYangNum + 1
		end
	end

	Atk_Revive_Time = 25 - AtkYinYangNum * 5
	Def_Revive_Time = 25 - DefYinYangNum * 5
	city.SetReviveTime(city_id[actID].CityID, 0, Atk_Revive_Time)
	city.SetReviveTime(city_id[actID].CityID, 1, Def_Revive_Time)

	-- 获得守护神相关信息
	Eudemon_State.EudemonTally = city.GetCityAppointedAtt(city_id[actID].CityID, 4)
	Eudemon_State.Summoned = 0
	Eudemon_State.NeedPerson = math.floor( 10 + (1000 - Eudemon_State.EudemonTally) / 50 )
	Eudemon_State.NeedSecond = math.floor( 20 + (1000 - Eudemon_State.EudemonTally) / 10 )
	Eudemon_State.NowPerson = 0
	Eudemon_State.BeginSummon = 0

	-- 城市防卫值
	local CityDefence = city.GetCityAppointedAtt(city_id[actID].CityID, 3)
	if CityDefence < 1000 then
		city_id[actID].DefenceLevel = 1
	elseif CityDefence < 3000 then
		city_id[actID].DefenceLevel = 2
	elseif CityDefence < 4000 then
		city_id[actID].DefenceLevel = 3
	elseif CityDefence < 6000 then
		city_id[actID].DefenceLevel = 4
	elseif CityDefence < 7000 then
		city_id[actID].DefenceLevel = 5
	elseif CityDefence < 8000 then
		city_id[actID].DefenceLevel = 6
	elseif CityDefence < 9000 then
		city_id[actID].DefenceLevel = 7
	else
		city_id[actID].DefenceLevel = 8
	end

	-- 根据城市防卫值确定城门的模型
	local DoorModelID = city_door[city_id[actID].DefenceLevel].ModelID
	city.ChangeCityDoor(city_id[actID].MapID, DoorModelID)

	-- 刷出城门怪物
	local DoorCreatureID = city_door[city_id[actID].DefenceLevel].CreatureID
	local Door_Pos = {}
	Door_Pos[1] = {}
	Door_Pos[2] = {}
	Door_Pos[3] = {}

	Door_Pos[1], Door_Pos[2], Door_Pos[3] = map.GetDoorPos(city_id[actID].MapID)

	-- 初始化门属性
	local index = 0
	for k, v in pairs(Door_Pos) do
		Door_State[k].CreatureID = map.MapCreateCreature(city_id[actID].MapID, -1, DoorCreatureID, Door_Pos[k][index+1], Door_Pos[k][index+2], Door_Pos[k][index+3])
		Door_State[k].x = Door_Pos[k][index+1]
		Door_State[k].y = Door_Pos[k][index+2]
		Door_State[k].z = Door_Pos[k][index+3]
		Door_State[k].Opened = 0
		Door_State[k].Change_Min = 0
		Door_State[k].Repair_Time = 0
		Door_State[k].HpNode = 9
		index = index + 3
	end

	-- 关闭所有城门
	map.OpenCloseAllDoors(city_id[actID].MapID, -1, false)

	-- 刷出龙珠
	-- 先删除掉非城战期间的龙珠
	map.MapDeleteCreature(city_id[actID].MapID, -1, LongZhu_State.LongZhuID)
	LongZhu_State.LongZhuID = map.MapCreateCreature(city_id[actID].MapID, -1, 4900029, 731, 11180, 754)
	unit.AddBuff(city_id[actID].MapID, -1, LongZhu_State.LongZhuID, 9999201, LongZhu_State.LongZhuID)		-- 添加守方特效

	-- 传出所有玩家
	local RoleTbl = map.GetMapAllRoleID(city_id[actID].MapID, -1)
	for k, v in pairs(RoleTbl) do
		-- 随机传送地点
		local num = table.getn(FX_Out_Pos)
		local index = math.random(1, num)
		role.RoleGotoNewMap(city_id[actID].MapID, -1, v, 3017298383, FX_Out_Pos[index].x, FX_Out_Pos[index].y, FX_Out_Pos[index].z)
	end

	-- 设置所有参战帮派为征战状态
	local AtkGuildTbl = city.GetConfirmGuild(city_id[actID].CityID, 0)
	local DefGuildTbl = city.GetConfirmGuild(city_id[actID].CityID, 1)

	if AtkGuildTbl ~= nil then
		for k, v in pairs(AtkGuildTbl) do
			guild.SetGuildStatus(v, 8)
		end
	end

	if DefGuildTbl ~= nil then
		for k, v in pairs(DefGuildTbl) do
			guild.SetGuildStatus(v, 8)
		end
	end

	PlayerNum[0] = 0
	PlayerNum[1] = 0

	-- 初始化城战玩家战绩
	city.InitCSRoleData(city_id[actID].CityID)

end

-- 活动结束
function CityStruggle_OnEnd(actID)
	city.SetCityStruggleState(0)

	CityStruggle_IsStart = 0

	if CityStruggle_IsOver == 0 then
		CityStruggle_IsOver = 1
		CityStruggle_OnOver()
	end

	-- 清空表
	Role_Alliance = {}
	YinYangZhu_State = {}
	LongZhu_State = {}

	CityStruggle_ActID = nil
	city_guard[city_id[actID].GuardIndex] = {}
	city_id[actID].GuardNum = 0

	-- 重置守护石状态
	Eudemon_Stone = {}
end

-- 活动广播
function CityStruggle_OnBroadCast(state)
	if state == 0 then					-- 城战即将开始
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1019)
		msg.DispatchWorldMsgEvent(MsgID)

	elseif state == 1 then				-- 城战开始
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1020)
		msg.DispatchWorldMsgEvent(MsgID)

	elseif state == 2 then				-- 城战即将结束


	elseif state == 3 then				-- 城战结束

	else

	end

end

-- 玩家复活
function CityStruggle_RoleRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)

    ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
	ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)

	if Type == 2 then			-- 接受复活
		local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
		return ReviveHP, ReviveMP, x, y, z, MapID
	elseif Type == 3 then		-- 牢狱复活
		return ReviveHP, ReviveMp, x, y, z, RebornMapID
	end

	if CityStruggle_IsStart ~= 1 then
	    return ReviveHP, ReviveMP, 808, 10350, 575, MapID
	end

	local x, y, z
	local force = city.GetRoleForce(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, RoleID)
	local GuildID = guild.GetRoleGuildID(RoleID)
	local GuildTbl = city.GetConfirmGuild(city_id[CityStruggle_ActID].CityID, force)

	-- 路人的话复活到出口点
	if GuildTbl == nil then
		return ReviveHP, ReviveMP, 890, 11259, 2307, 3017298383
	end

	for k, v in pairs(GuildTbl) do
		if GuildID == v then
			if force == 0 then
				-- 传入该帮派的集结点
				x = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].x
				y = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].y
				z = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].z
			elseif force == 1 then
				x = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].x
				y = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].y
				z = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].z
			else

			end
			break
		end
	end

	-- 获取玩家阵营
	if Role_Alliance[RoleID] == 0 then
		if Atk_ZhaoHunFan.use == 1 then
			x = Atk_ZhaoHunFan.x
			y = Atk_ZhaoHunFan.y
			z = Atk_ZhaoHunFan.z
		else
		end
	elseif Role_Alliance[RoleID] == 1 then
		if Def_ZhaoHunFan.use == 1 then
			x = Def_ZhaoHunFan.x
			y = Def_ZhaoHunFan.y
			z = Def_ZhaoHunFan.z
		else
		end
	else
	end


	-- 添加一个15秒无敌的buff
	unit.AddBuff(MapID, InstanceID, RoleID, 9999001, RoleID)    --加一个15秒钟无敌buff

	RebornMapID = city_id[CityStruggle_ActID].MapID

	return ReviveHP, ReviveMP, x, y, z, RebornMapID
end

-- 玩家上线后不能进入此地图
function CityStruggle_CanEnterWhenOnline(MapID, RoleID)
	return 3017298383, 890, 11259, 2307
end

-- 玩家进入地图
function CityStruggle_OnPlayerEnter(MapID, InstanceID, RoleID)
	-- 非城战时间不进行下面的判断
	if CityStruggle_IsStart ~= 1 then
		return
	end

    -- 玩家一进入地图便判断其联盟阵营
    local force = city.GetRoleForce(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, RoleID)

    -- 进攻联盟
    if force == 0 then
        if Role_Alliance[RoleID] == nil then
            Role_Alliance[RoleID] = 0
			PlayerNum[force] = PlayerNum[force] + 1
        end

    -- 防御联盟
    elseif force == 1 then
        if Role_Alliance[RoleID] == nil then
            Role_Alliance[RoleID] = 1
			PlayerNum[force] = PlayerNum[force] + 1
        end
    else

    end

	-- 增加一条玩家战绩记录
	city.AddRoleDataInfo(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, RoleID)

	-- 给客户端发送消息
	city.EnterLeaveCityStruggle(city_id[CityStruggle_ActID].CityID, RoleID, 1, Role_Alliance[RoleID])

	-- 给客户端同步城战状态
	SendCityStruggleInfo(MapID, InstanceID, RoleID)
	SycAllRoleOneInfo(MapID, InstanceID, 0, force, PlayerNum[force])

	-- 给玩家添加封印技能
	local LearnedSkill1 = role.IsLearnedSkill(RoleID, 90015)
	local LearnedSkill2 = role.IsLearnedSkill(RoleID, 90016)
	local LearnedSkill3 = role.IsLearnedSkill(RoleID, 90018)
	if LearnedSkill1 == nil then
		role.AddSkill(RoleID, 9001501)
	end
	if LearnedSkill2 == nil then
		role.AddSkill(RoleID, 9001601)
	end

	--if force == 1 and LearnedSkill3 == nil then
	--	role.AddSkill(RoleID, 9001801)
	--end

	-- 重置玩家士气为100
	unit.SetAttValue(MapID, InstanceID, RoleID, 63, 100)
end

-- 玩家离开地图
function CityStruggle_OnPlayerLeave(MapID, InstanceID, RoleID)
	-- 重置玩家士气为100
	unit.SetAttValue(MapID, InstanceID, RoleID, 63, 100)

	-- 如果是正在封印龙珠的玩家，停止封印，改变龙珠状态
	if LongZhu_State.RoleID == RoleID then
		unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999101)		-- 去除封印时特效
		unit.CancelBuff(MapID, InstanceID, RoleID, 9999401)						-- 去除封印者无敌buff

		-- 同步龙珠状态给地图所有玩家
		SycAllRoleOneInfo(MapID, InstanceID, 2, 0, Role_Alliance[RoleID])

		LongZhu_State.RoleID = nil				-- 去除当前封印玩家记录
	end

	-- 非城战时间进行下面的判断
	if CityStruggle_IsOver == 1 then
		if unit.IsInState(MapID, InstanceID, RoleID, 8) then	-- 城战结束后传出取消城战状态
			city.EnterLeaveCityStruggle(3, RoleID, 0, 0)

		elseif unit.IsInState(MapID, InstanceID, RoleID, 9) then
			city.EnterLeaveCityStruggle(3, RoleID, 0, 1)

		else

		end

		return
	end

	-- 给客户端发送消息
	city.EnterLeaveCityStruggle(city_id[CityStruggle_ActID].CityID, RoleID, 0, Role_Alliance[RoleID])

	if Role_Alliance[RoleID] ~= nil then
		if Role_Alliance[RoleID] == 0 then
			PlayerNum[0] = PlayerNum[0] - 1
		else
			PlayerNum[1] = PlayerNum[1] - 1
		end

		SycAllRoleOneInfo(MapID, InstanceID, 0, Role_Alliance[RoleID], PlayerNum[Role_Alliance[RoleID]])

		Role_Alliance[RoleID] = nil
	end

end

-- 发送当前城战信息给客户端
function SendCityStruggleInfo(MapID, InstanceID, RoleID)
	-- 龙珠封印状态
	local bSealing
	if LongZhu_State.RoleID == nil then
		bSealing = false
	else
		bSealing = true
	end

	-- 城战结束倒计时
	local CountDown
	if LongZhu_State.ChangeMin == nil then
		CountDown = (60 - CityStruggle_CurMin)*60
	else
		CountDown = (20 - (CityStruggle_CurMin - LongZhu_State.ChangeMin))*60
	end

	-- 阴阳柱占领状态
	local index = 1
	local YinYangZhu = {}
	for k,v in pairs(YinYangZhu_State) do
		local YinYangZhuIndex = CityStruggle_GetYinYangZhuIndex(k)
		if YinYangZhuIndex ~= nil then
			YinYangZhu[YinYangZhuIndex+1] = v
		end
	end

	-- 物资库数量
	local nWuZiKuNum = 0
	for k, v in pairs(Material_State) do
		if Material_State[k].Destroied == true then

		else
			nWuZiKuNum = nWuZiKuNum + 1
		end
	end

	-- 城门状态
	index = 1
	local Door = {}
	Door[1] = {Pos, bDestroied, nCountDown}
	Door[2] = {Pos, bDestroied, nCountDown}
	Door[3] = {Pos, bDestroied, nCountDown}

	for k, v in pairs(Door_State) do
		Door[k].Pos = k
		Door[k].bDestroied = Door_State[k].Opened
		if Door_State[k].Repair_Time >= 3 then
			Door[k].nCountDown = -1
		else
			Door[k].nCountDown = 20 - (CityStruggle_CurMin - Door_State[k].Change_Min)
		end
	end

	-- 统计双方人数
--[[	local AtkNum = 0
	local DefNum = 0
	for k, v in pairs(Role_Alliance) do
		if v == 0 then
			AtkNum = AtkNum + 1
		else
			DefNum = DefNum + 1
		end
	end
]]
	city.SendCityStruggleInfo(
								MapID,				InstanceID,			RoleID,				LongZhu_State.Belong,	bSealing,
								CountDown,			YinYangZhu[1],		YinYangZhu[2],		YinYangZhu[3],			YinYangZhu[4],
								YinYangZhu[5],		nWuZiKuNum,			Door[1].Pos,		Door[1].bDestroied,		Door[1].nCountDown,
								Door[2].Pos,		Door[2].bDestroied, Door[2].nCountDown, Door[3].Pos,			Door[3].bDestroied,
								Door[3].nCountDown, PlayerNum[0],		PlayerNum[1]
								)
end

-- 发送一条城战消息
function SendOneCityStruggleInfo(MapID, InstanceID, RoleID, Type1, Type2, Data)
--[[
	Type1 == 0			-- 玩家数量发生改变

	Type1 == 1			-- 龙珠正在被封印

	Type1 == 2			-- 龙珠被占领

	Type1 == 3			-- 阴阳柱被占领

	Type1 == 4			-- 物资库数量改变

	Type1 == 5			-- 城门攻陷

]]

	city.SendOneCityStruggleInfo(MapID, InstanceID, RoleID, Type1, Type2, Data)
end

-- 向地图中所有玩家同步城战状态
function SycAllRoleOneInfo(MapID, InstanceID, Type1, Type2, Data)
	for k, v in pairs(Role_Alliance) do
		city.SendOneCityStruggleInfo(MapID, InstanceID, k, Type1, Type2, Data)
	end
end

-- 生物死亡
function CityStruggle_OnCreatureDie(MapID, InstanceID, CreID, CreTypeID, KillerID)
	-- 击杀守护神，进攻联盟士气增加
end

-- 玩家死亡
function CityStruggle_OnRoleDie(MapID, InstanceID, RoleID, KillerID)
	if unit.IsPlayer(KillerID) == true then
		-- 判断玩家是否是帮主
		local guildID = guild.GetRoleGuildID(RoleID)
		if guildID ~= 4294967295 then
			if guild.GetRoleGuildPos(guildID, RoleID) == 2 then
				-- 击杀帮主，击杀者帮派功勋+10
				local killGuildID = guild.GetRoleGuildID(KillerID)
				if guildID ~= 4294967295 then
					guild.ModifyMemberExploit(guildID, KillerID, 10)
				end

				-- 击杀者所属帮派成员士气+5 被击杀帮派全体成员士气-5
				for k, v in pairs(Role_Alliance) do
					local GID = guild.GetRoleGuildID(k)
					if killGuildID == GID then
						unit.ModAttValue(MapID, InstanceID, k, 63, 5)
					elseif guildID == GID then
						unit.ModAttValue(MapID, InstanceID, k, 63, -5)
					else

					end
					FX_ResetMorale(MapID, InstanceID, k)
				end

				-- 广播广播
				local MsgID = msg.BeginMsgEvent()
				local id_msg = 0
				if Role_Alliance[RoleID] == 0 then -- 防御联盟击杀进攻联盟帮主
					id_msg = 1042
				else
					id_msg = 1041
				end
				msg.AddMsgEvent(MsgID, 101, id_msg)
				msg.AddMsgEvent(MsgID, 19, guildID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

			else
				-- 击杀普通帮众，击杀者帮派功勋+1
				local killGuildID = guild.GetRoleGuildID(KillerID)
				if guildID ~= 4294967295 then
					guild.ModifyMemberExploit(guildID, KillerID, 1)
				end

				-- 击杀者士气+1，被杀者士气-1
				unit.ModAttValue(MapID, InstanceID, KillerID, 63, 1)
				FX_ResetMorale(MapID, InstanceID, KillerID)
				unit.ModAttValue(MapID, InstanceID, RoleID, 63, -1)
				FX_ResetMorale(MapID, InstanceID, RoleID)
			end

			-- 增加经验
			local Level = role.GetRoleLevel(MapID, InstanceID, RoleID)
			local AddExp = role.GetKillObjExp(Level)
			if AddExp ~= nil then
				role.AddRoleExp(MapID, InstanceID, KillerID, AddExp)
			end

			-- 小队增加击杀玩家积分（除击杀者）
			local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, KillerID)
			if TeamID~= 4294967295 then
				local TeamMem = {}
				TeamMem[1], TeamMem[2], TeamMem[3], TeamMem[4], TeamMem[5], TeamMem[6] = role.GetRoleTeamMemID(TeamID)
				for k,v in pairs(TeamMem) do
					if v ~= nil and v ~= KillerID then
						city.AddKillRoleNum(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, v, RoleID)
					end
				end
			end

			-- 击杀者增加击杀积分
			city.AddKillRoleNum(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, KillerID, RoleID)

			-- 得到击杀人数，如果很多就要广播！！
			local KillNum = city.GetRoleKillNum(MapID, InstanceID, KillerID, city_id[CityStruggle_ActID].CityID)
			if KillNum == 20 or KillNum == 50 or KillNum == 100 then
				local MsgID = msg.BeginMsgEvent()
				local id_msg = 0
				if Role_Alliance[KillerID] == 0 then -- 进攻联盟成员杀成神了！
					id_msg = 1040
				else
					id_msg = 1039
				end
				msg.AddMsgEvent(MsgID, 101, id_msg)
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 9, KillNum)
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			end
		end
	end
end

-- 城战结束
function CityStruggle_OnOver()
	local CityID = city_id[CityStruggle_ActID].CityID

	-- 禁城统治权改变
	if LongZhu_State.Belong == 0 then
		-- 失去统治权帮派额外减少200点威望
		guild.ModifyGuildReputation(city.GetCityAppointedAtt(cityID, 2), -1, -200, 607)
		city.ChangeCityHolder(CityID, LongZhu_State.GuildID)
	end

	-- 删除城门
	map.MapDeleteCreature(city_id[CityStruggle_ActID].MapID, -1, Door_State[1].CreatureID)
	map.MapDeleteCreature(city_id[CityStruggle_ActID].MapID, -1, Door_State[2].CreatureID)
	map.MapDeleteCreature(city_id[CityStruggle_ActID].MapID, -1, Door_State[3].CreatureID)

	-- 打开所有城门
	map.OpenCloseAllDoors(city_id[CityStruggle_ActID].MapID, -1, true)


	-- 广播结束城战
	if LongZhu_State.GuildID == -1 then
		LongZhu_State.GuildID = city.GetCityAppointedAtt(CityID, 2)
	end

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1021)
	msg.AddMsgEvent(MsgID, 6, city_id[CityStruggle_ActID].MapID)
	msg.AddMsgEvent(MsgID, 19, LongZhu_State.GuildID)
	msg.AddMsgEvent(MsgID, 6, city_id[CityStruggle_ActID].MapID)
	msg.DispatchWorldMsgEvent(MsgID)

    -- 帮派威望的改变
	local Reputation = 0

	-- 计算防御联盟威望改变值 （50 + 对立联盟帮派的帮派威望平均值 * 0.1）
	local AtkAddRep = 0
	local DefAddRep = 0
	local AtkGuildTbl = city.GetConfirmGuild(CityID, 0)
	if AtkGuildTbl ~= nil then
		for k, v in pairs(AtkGuildTbl) do
			Reputation = Reputation + guild.GetGuildReputation(AtkGuildTbl[k])
		end
		DefAddRep = 50 + Reputation / table.getn(AtkGuildTbl) * 0.1
	end

	-- 计算防御联盟威望改变值
	Reputation = 0
	local DefGuildTbl = city.GetConfirmGuild(CityID, 1)
	if DefGuildTbl ~= nil then
		for k, v in pairs(DefGuildTbl) do
			Reputation = Reputation + guild.GetGuildReputation(DefGuildTbl[k])
		end
		AtkAddRep = 50 + Reputation / table.getn(DefGuildTbl) * 0.1
	end

	-- 取消参战帮派的征战状态
	if AtkGuildTbl ~= nil then
		for k, v in pairs(AtkGuildTbl) do
			guild.UnsetGuildStatus(v, 8)
		end
	end

	if DefGuildTbl ~= nil then
		for k, v in pairs(DefGuildTbl) do
			guild.UnsetGuildStatus(v, 8)
		end
	end

	if LongZhu_State.Belong == 0 then
		DefAddRep = 0 - DefAddRep
	else
		AtkAddRep = 0 - AtkAddRep
	end

	-- 更改威望
	if AtkGuildTbl ~= nil then
		for k, v in pairs(AtkGuildTbl) do
			guild.ModifyGuildReputation(AtkGuildTbl[k], -1, AtkAddRep, 607)
		end
	end

	if DefGuildTbl ~= nil then
		for k, v in pairs(DefGuildTbl) do
			guild.ModifyGuildReputation(DefGuildTbl[k], -1, DefAddRep, 607)
		end
	end

	-- 得到统治权的帮派额外增加200点威望
	guild.ModifyGuildReputation(city.GetCityAppointedAtt(cityID, 2), -1, 200, 607)

	-- 个人功勋奖励
	for k, v in pairs(Role_Alliance) do
		-- 获得玩家积分
		local nScore = city.GetRoleCSScore(city_id[CityStruggle_ActID].MapID, -1, city_id[CityStruggle_ActID].CityID, k)
		-- 获得帮派ID
		local guildID = guild.GetRoleGuildID(k)

		local nExploitGain = 0
		local nContributeGain = 0

		if Role_Alliance[k] == LongZhu_State.Belong then

			--[[
			if guildID == LongZhu_State.GuildID then
				-- 帮派获得城战胜利且获得城市统治权，帮派功勋+200
				guild.ModifyMemberExploit(guildID, k, 200)
			else
				-- 获得城战胜利但未获得城市统治权，帮派功勋+100
				guild.ModifyMemberExploit(guildID, k, 100)
			end
			]]
			-- 胜利帮派奖励
			nExploitGain = nScore*3
			nContributeGain = nScore/2

		else
			-- 未获得城战胜利帮派奖励
			nExploitGain = nScore
			nContributeGain = nScore/3
		end

		guild.ModifyMemberExploit(guildID, k, nExploitGain)			-- 功勋奖励
		guild.ModifyRoleContribute(guildID, k, nContributeGain, 0)	-- 贡献奖励

		city.AddRoleData(city_id[CityStruggle_ActID].MapID, -1,city_id[CityStruggle_ActID].CityID, k, 7, nExploitGain)		-- 功勋奖励
		city.AddRoleData(city_id[CityStruggle_ActID].MapID, -1,city_id[CityStruggle_ActID].CityID, k, 8, nContributeGain)	-- 贡献奖励
	end

	-- 积分排名奖励
	city.SortByScore(CityID)
	-- 获取积分前20名玩家
	local RankRoleID = city.GetCSRankedRoleID(CityID, 20)

	-- 给排名前20的玩家发称号奖励
	if RankRoleID ~= nil then
		for k, v in pairs(RankRoleID) do

			-- 积分排名第1的玩家获得称号"十步杀一人"
			if k == 1 then
				role.SigTitleEvent(v, 258)
			end

			-- 积分排名第2，3的玩家获得称号"千里不留行"
			if k == 2 or k == 3 then
				role.SigTitleEvent(v, 259)
			end

			-- 积分排名第4～10的玩家获得称号"事了拂衣去"
			if k >= 4 and k <= 10 then
				role.SigTitleEvent(v, 260)
			end

			-- 积分排名第11～20的玩家获得称号"深藏身与名"
			if k >= 11 and k <= 20 then
				role.SigTitleEvent(v, 261)
			end
		end
	end

	-- 奖励待定
	--[[ role.AddRoleItem(RankRoleID[1])
	......
	]]

    -- 将城战失败联盟传送出场景
	local FailForce
	if LongZhu_State.Belong == 0 then
		FailForce = 1
	else
		FailForce = 0
	end

	for k, v in pairs(Role_Alliance) do
		if v == FailForce then
			-- 随机传送地点
			local num = table.getn(FX_Out_Pos)
			local index = math.random(1, num)
			role.RoleGotoNewMap(city_id[CityStruggle_ActID].MapID, -1, k, 3017298383, FX_Out_Pos[index].x, FX_Out_Pos[index].y, FX_Out_Pos[index].z)
		end

		-- 给所有玩家发送个人战绩信息
		role.SendCityStruggleRecord2Role(k);
	end

	-- 给龙珠挂上非城战的特效
	unit.AddBuff(city_id[CityStruggle_ActID].MapID, -1, LongZhu_State.LongZhuID, 9999201, LongZhu_State.LongZhuID)		-- 添加非城战特效

	-- 删除招魂幡
	if Atk_ZhaoHunFan.use == 1 then
		map.MapDeleteCreature(city_id[CityStruggle_ActID].MapID, -1, Atk_ZhaoHunFan.CreatureID)
	end

	if Def_ZhaoHunFan.use == 1 then
		map.MapDeleteCreature(city_id[CityStruggle_ActID].MapID, -1, Def_ZhaoHunFan.CreatureID)
	end

	CityStruggle_IsStart = 0

	-- 给全服所有玩家添加普天同庆buff
	act.AddAllRoleBuff(2027901);
end

-- 凤翔城战传送卫城npc对话
function C4900005_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 1026)		--“您是否要前往凤翔卫城？”
		msg.AddMsgEvent(MsgID, 21, 4)
		msg.AddMsgEvent(MsgID, 1, 21)
		msg.AddMsgEvent(MsgID, 21, 5)
		msg.AddMsgEvent(MsgID, 1, 22)
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	elseif TalkIndex == 4 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298383, 890, 225, 2307)
	end
end

-- 凤翔城战传送npc对话
function C4900004_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	--如果是第一次对话
	if TalkIndex == -1 then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["CS_FX_TalkIndex"], 1)
	end

	local stepindex = role.GetRoleScriptData(RoleID, 1, RoleDataType["CS_FX_TalkIndex"])

	if stepindex == 1 then
		if CityStruggle_IsStart == 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1001)		--“城战已经开始，是否要参加”
			msg.AddMsgEvent(MsgID, 21, 4)
			msg.AddMsgEvent(MsgID, 1, 1002)
			msg.AddMsgEvent(MsgID, 21, 5)
			msg.AddMsgEvent(MsgID, 1, 1003)
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

			stepindex = 2
		else
			-- 非城战对话
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1001)		-- 非占领帮派进入需要路费
			msg.AddMsgEvent(MsgID, 21, 4)
			msg.AddMsgEvent(MsgID, 1, 1002)		-- 我要进入
			msg.AddMsgEvent(MsgID, 21, 5)
			msg.AddMsgEvent(MsgID, 1, 1003)		-- 还是算了
			msg.AddMsgEvent(MsgID, 24, TargetID)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)

			stepindex = 3
		end

	elseif stepindex == 2 then
		if TalkIndex ~= 4 then
			return
		end

		local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
		local CityID = city_id[CityStruggle_ActID].CityID
		if level < 60 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20, 1006)	-- 等级必须大于60级
			msg.AddMsgEvent(MsgID, 21, 4)
			msg.AddMsgEvent(MsgID, 1, 21)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		else
			local force = city.GetRoleForce(MapID, InstanceID, CityID, RoleID)
			if force == 2 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20, 1005)	-- 您无权参与城战
				msg.AddMsgEvent(MsgID, 21, 4)
				msg.AddMsgEvent(MsgID, 1, 21)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			else
				local GuildID = guild.GetRoleGuildID(RoleID)
				local Num = CityStruggle_GetGuildMemNum(GuildID)
				local HoldGuildID = city.GetCityAppointedAtt(CityID, 2)

				if Num >= FX_Normal_Guild_Mem then
					if GuildID ~= HoldGuildID then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1007)	-- 帮派参与人数已达上限
						msg.AddMsgEvent(MsgID, 21, 4)
						msg.AddMsgEvent(MsgID, 1, 21)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)

					elseif GuildID == HoldGuildID and Num >= FX_Hold_Guild_Mem then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 20, 1007)	-- 帮派参与人数已达上限
						msg.AddMsgEvent(MsgID, 21, 4)
						msg.AddMsgEvent(MsgID, 1, 21)
						msg.DispatchRoleMsgEvent(RoleID, MsgID)

					else
						-- 禁城拥有帮派，传入指定地点
						local x, y, z
						local GuildTbl = city.GetConfirmGuild(CityID, 1)
						local GuildID = guild.GetRoleGuildID(RoleID)
						for k, v in pairs(GuildTbl) do
							if GuildID == v then
								-- 传入该帮派的集结点
								x = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].x
								y = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].y
								z = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].z
								break
							end
						end

						role.RoleGotoNewMap(MapID, InstanceID, RoleID, city_id[CityStruggle_ActID].MapID, x, y, z)
					end
				else
					-- 其他帮派
					--local force = city.GetRoleForce(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, RoleID)
					local x, y, z
					local GuildID = guild.GetRoleGuildID(RoleID)
					local GuildTbl = city.GetConfirmGuild(CityID, force)
					for k, v in pairs(GuildTbl) do
						if GuildID == v then
							if force == 0 then
								-- 传入该帮派的集结点
								x = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].x
								y = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].y
								z = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k+5].z
							elseif force == 1 then
								x = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].x
								y = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].y
								z = Enter_Pos[city_id[CityStruggle_ActID].EnterIndex][k].z
							else

							end
							break
						end
					end

					role.RoleGotoNewMap(MapID, InstanceID, RoleID, city_id[CityStruggle_ActID].MapID, x, y, z)
				end
			end
		end

		stepindex = 1

	elseif stepindex == 3 then
		if TalkIndex ~= 4 then
			return
		end

		local GuildID = guild.GetRoleGuildID(RoleID)
		if GuildID ~= 4294967295 and GuildID == city.GetCityAppointedAtt(3, 2) then		-- 城市统治帮派成员传送免费
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3695619387, 812, 208, 503)

		else
			local Money = role.GetRoleSilver(MapID, InstanceID, RoleID)
			if Money < 10000 then
				--提示玩家金钱不足
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	1004)
				msg.AddMsgEvent(MsgID, 21,	4)			    --确定
				msg.AddMsgEvent(MsgID, 1,	21)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)

			else
				-- 扣除金钱并传送
				role.DecRoleSilver(MapID, InstanceID, RoleID, 10000, 200)
				role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3695619387, 812, 208, 503)
			end

			stepindex = 1
		end
	end

	role.SetRoleScriptData(RoleID, 1, RoleDataType["CS_FX_TalkIndex"], stepindex)
end

-- 获取帮派参加城战人数
function CityStruggle_GetGuildMemNum(GuildID)
	local num = 0
	for k, v in pairs(Role_Alliance) do
		if GuildID == guild.GetRoleGuildID(k) then
			num = num + 1
		end
	end

	return num
end

-- 获取阴阳柱索引
function CityStruggle_GetYinYangZhuIndex(TargetTypeID)
	for i = 1, 5 do
		if YinYangZhuTypeID[i] == TargetTypeID then
			return i-1;
		end
	end

	return nil
end

-- 封印
function OnSealSkill(MapID, InstanceID, SkillID, OwnerID, TargetID, TargetTypeID)
	local GuildID = guild.GetRoleGuildID(OwnerID)

	if TargetTypeID == 4900029 then			-- 龙珠
		unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999101)		-- 去除封印时特效
		unit.CancelBuff(MapID, InstanceID, OwnerID, 9999401)						-- 去除封印者无敌buff

		-- 去除龙珠当前特效，添加新特效
		if LongZhu_State.Belong == 1 then
			unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999201)
			unit.AddBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999301, OwnerID)
		else
			unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999301)
			unit.AddBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999201, OwnerID)
		end

		LongZhu_State.ChangeMin = CityStruggle_CurMin
		LongZhu_State.GuildID = GuildID
		LongZhu_State.RoleID = nil			-- 封印完成去除封印者记录
		LongZhu_State.Belong = Role_Alliance[OwnerID]


		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101,	1018)				-- 成功封印龙珠
		msg.AddMsgEvent(MsgID, 19, GuildID)
		msg.AddMsgEvent(MsgID, 2, OwnerID)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		-- 所在联盟士气+5
		for k, v in pairs(Role_Alliance) do
			if v == Role_Alliance[OwnerID] then
				unit.ModAttValue(MapID, InstanceID, k, 63, 5)
				FX_ResetMorale(MapID, InstanceID, k)
			end
		end

		-- 同步龙珠状态给地图所有玩家
		SycAllRoleOneInfo(MapID, InstanceID, 3, 20, Role_Alliance[OwnerID])

		-- 增加开启龙珠积分
		city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, OwnerID, 6, 1)

	elseif TargetTypeID == 4900030 or TargetTypeID == 4900043 or TargetTypeID == 4900044 or TargetTypeID == 4900045 or TargetTypeID == 4900046 then		-- 阴阳柱

		-- 已有己方帮派成员封印过了
		if YinYangZhu_State[TargetTypeID] == Role_Alliance[OwnerID] then
			return 0
		end

		-- 去除阴阳柱当前特效，添加新特效
		unit.CancelBuff(MapID, InstanceID, TargetID, 9999102)					-- 去除封印时的特效

		if YinYangZhu_State[TargetTypeID] == 1 then
			unit.CancelBuff(MapID, InstanceID, TargetID, 9999202)
			unit.AddBuff(MapID, InstanceID, TargetID, 9999302, OwnerID)
		else
			unit.CancelBuff(MapID, InstanceID, TargetID, 9999302)
			unit.AddBuff(MapID, InstanceID, TargetID, 9999202, OwnerID)
		end

		YinYangZhu_State[TargetTypeID] = Role_Alliance[OwnerID]

		-- 同步阴阳柱状态给地图所有玩家
		local YinYangZhuIndex = CityStruggle_GetYinYangZhuIndex(TargetTypeID)
		if YinYangZhuIndex ~= nil then
			SycAllRoleOneInfo(MapID, InstanceID, 4, YinYangZhuIndex, Role_Alliance[OwnerID])
		end

		-- 获取双方占领阴阳柱数量
		local AtkSeize = 0
		local DefSeize = 0
		for k, v in pairs(YinYangZhu_State) do
			if v == 0 then
				AtkSeize = AtkSeize + 1
			else
				DefSeize = DefSeize + 1
			end
		end

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101,	1017)				-- 成功封印阴阳柱
		msg.AddMsgEvent(MsgID, 19, GuildID)
		msg.AddMsgEvent(MsgID, 2, OwnerID)
		msg.AddMsgEvent(MsgID, 5, TargetTypeID)
		msg.AddMsgEvent(MsgID, 9, AtkSeize)
		msg.AddMsgEvent(MsgID, 9, DefSeize)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		-- 所在联盟士气+3
		for k, v in pairs(Role_Alliance) do
			if v == Role_Alliance[OwnerID] then
				unit.ModAttValue(MapID, InstanceID, k, 63, 3)
				FX_ResetMorale(MapID, InstanceID, k)
			end
		end

		-- 设置双方的复活时间
		if YinYangZhu_State[TargetTypeID] == 0 then
			Atk_Revive_Time = Atk_Revive_Time - 5
			Def_Revive_Time = Def_Revive_Time + 5
		else
			Atk_Revive_Time = Atk_Revive_Time + 5
			Def_Revive_Time = Def_Revive_Time - 5
		end

		city.SetReviveTime(city_id[CityStruggle_ActID].CityID, 0, Atk_Revive_Time)
		city.SetReviveTime(city_id[CityStruggle_ActID].CityID, 1, Def_Revive_Time)

		-- 如果己方阴阳柱数量少于3个，则强制结束己方封印龙珠的操作
		if LongZhu_State.RoleID ~= nil then
			local SeizeNum = 0
			for k, v in pairs(YinYangZhu_State) do
				if v ~= nil and  v == Role_Alliance[LongZhu_State.RoleID] then
					SeizeNum = SeizeNum + 1
				end
			end

			if SeizeNum < 3 and LongZhu_State.RoleID ~= nil then
				role.RoleCancelSkill(MapID, InstanceID, LongZhu_State.RoleID, 9001501)
				local GuildID = guild.GetRoleGuildID(LongZhu_State.RoleID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 101,	1016)				-- 强制终止封印龙珠
				msg.AddMsgEvent(MsgID, 19, GuildID)
				msg.AddMsgEvent(MsgID, 2, LongZhu_State.RoleID)
				msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

				-- 同步龙珠状态给地图所有玩家
				--SycAllRoleOneInfo(MapID, InstanceID, 2, 0, Role_Alliance[OwnerID])

				--unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999101)		-- 去除封印时特效
				--unit.CancelBuff(MapID, InstanceID, LongZhu_State.RoleID, 9999401)						-- 去除封印者无敌buff
				--LongZhu_State.RoleID = nil						-- 龙珠封印者记录置空
			end
		end

		-- 增加开启阴阳柱积分
		city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, OwnerID, 5, 1)

	else

	end

	return 0
end

-- 是否能使用封印技能  （返回999代表无法使用技能且无错误屏显）
function OnSealCanCast(MapID, InstanceID, SkillID, OwnerID, TargetID)
	local GuildID = guild.GetRoleGuildID(OwnerID)
	local Pos = guild.GetRoleGuildPos(GuildID, OwnerID)
	local UnitTypeID
	UnitTypeID = unit.GetUnitTypeID(MapID, InstanceID, TargetID)

	-- 封印龙珠
	if UnitTypeID == 4900029 then
		if CityStruggle_IsStart ~= 1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1010)			-- 非城战期间无法执行封印龙珠操作
			msg.DispatchRoleMsgEvent(OwnerID, MsgID)
			return 999
		end

		if Pos ~= 2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1008)			-- 只有各帮派帮主可对龙珠进行封印！
			msg.DispatchRoleMsgEvent(OwnerID, MsgID)
			return 999
		else
			if Role_Alliance[OwnerID] == LongZhu_State.Belong then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1009)		-- 此龙珠目前已被您所属的联盟封印！
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)
				return 999

			else
				local SeizeNum = 0
				for k, v in pairs(YinYangZhu_State) do
					if v ~= nil and  v == Role_Alliance[OwnerID] then
						SeizeNum = SeizeNum + 1
					end
				end

				if SeizeNum < 3 then
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 26, 1011)		-- 只有当己方联盟占领阴阳柱数量大于三个才可进行龙珠封印！
					msg.DispatchRoleMsgEvent(OwnerID, MsgID)
					return 999
				else
					if LongZhu_State.RoleID ~= nil then
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 1030)	-- 龙珠同时只可有一人进行封印操作！
						msg.DispatchRoleMsgEvent(OwnerID, MsgID)

						return 999
					end

					-- 跑马灯公告
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 101, 1015)		-- 已开始封印龙珠，当其所属联盟占领阴阳柱少于3个时，封印操作将被终止！
					msg.AddMsgEvent(MsgID, 19, GuildID)
					msg.AddMsgEvent(MsgID, 2, OwnerID)
					msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

					LongZhu_State.RoleID = OwnerID		-- 记录封印玩家的ID

					unit.AddBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999101, OwnerID)		-- 添加封印特效
					unit.AddBuff(MapID, InstanceID, OwnerID, 9999401, OwnerID)						-- 给封印者添加无敌buff

					-- 同步龙珠状态给地图所有玩家
					SycAllRoleOneInfo(MapID, InstanceID, 1, 0, Role_Alliance[OwnerID])

					return 0							-- 成功封印
				end
			end
		end

	-- 封印阴阳柱
	elseif UnitTypeID == 4900030 or UnitTypeID == 4900043 or UnitTypeID == 4900044 or UnitTypeID == 4900045 or UnitTypeID == 4900046 then

		if Pos ~= 2 and Pos ~= 3 and Pos ~= 6 and Pos ~= 9 and Pos~= 12 and Pos ~= 15 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1012)			-- 只有各帮派堂主以上职务者可对阴阳柱进行封印！
			msg.DispatchRoleMsgEvent(OwnerID, MsgID)
			return 999
		else
			if YinYangZhu_State[UnitTypeID] ~= nil and Role_Alliance[OwnerID] == YinYangZhu_State[UnitTypeID] then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1013)		-- 此阴阳柱目前已被您所属的联盟封印！
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)
				return 999
			else
				unit.AddBuff(MapID, InstanceID, TargetID, 9999102, OwnerID)		-- 添加封印特效
				return 0								-- 成功封印
			end
		end

	-- 守护石
	elseif UnitTypeID == 4900048 then
--[[ 关闭守护神召唤功能
		if Role_Alliance[OwnerID] == 1 then
			if Eudemon_State.Summoned ~= 1 then

				Eudemon_Stone[OwnerID] = 1
				-- 计算当前召唤人数
				local nPerson = 0
				for k,v in pairs(Eudemon_Stone) do
					if v == 1 then
						nPerson = nPerson + 1
					end
				end
				Eudemon_State.NowPerson = nPerson


				if Eudemon_State.NowPerson < Eudemon_State.NeedPerson and Eudemon_State.BeginSummon ~= 1 then	-- 还没开始召唤

					for k,v in pairs(Eudemon_Stone) do
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 1043)		-- 当前召唤人数：X/Y，请勿进行移动操作，当人数足够后将开始召唤。
						msg.AddMsgEvent(MsgID, 9, Eudemon_State.NowPerson)
						msg.AddMsgEvent(MsgID, 9, Eudemon_State.NeedPerson)
						msg.DispatchRoleMsgEvent(k, MsgID)
					end

--				elseif Eudemon_State.NowPerson < Eudemon_State.NeedPerson and Eudemon_State.BeginSummon == 1 then	-- 已经开始召唤
					-- 所有召唤的人停止召唤技能
--					for k,v in pairs(Eudemon_Stone) do
--						role.RoleCancelSkill(MapID, InstanceID, k, SkillID)
--					end
--
					-- 清空召唤表
--					Eudemon_Stone = {}

				else	-- 还没召唤，已达到召唤条件

					-- 开始召唤兽倒计时
					city.SetSummonTime(city_id[CityStruggle_ActID].CityID, Eudemon_State.NeedSecond)

					-- 广播
					for k,v in pairs(Eudemon_Stone) do
						local MsgID = msg.BeginMsgEvent()
						msg.AddMsgEvent(MsgID, 26, 1044)		-- 守护神召唤已开始，请勿进行移动操作。
						msg.DispatchRoleMsgEvent(k, MsgID)

						--role.EudemonSummonCountDown(MapID, IstanceID, k, Eudemon_State.NeedSecond)
					end

					Eudemon_State.BeginSummon = 1
				end
				return 0

			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1045)			-- 本次城战中已经进行过守护神召唤
				msg.DispatchRoleMsgEvent(OwnerID, MsgID)
				return 999
			end

		else
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1046)				-- 只有防御联盟的帮派成员可进行守护神召唤
			msg.DispatchRoleMsgEvent(OwnerID, MsgID)
			return 999
		end
]] -- 关闭守护神召唤功能
		-- 关闭守护神召唤
		return 999
	else

	end
end

-- 取消封印
function OnSealCancelSkill(MapID, InstanceID, SkillID, OwnerID, TargetID, TargetTypeID)
	if TargetTypeID == 4900048 then				-- 复活石
		Eudemon_Stone[OwnerID] = nil

		-- 计算当前召唤人数
		Eudemon_State.NowPerson = Eudemon_State.NowPerson - 1
		if Eudemon_State.BeginSummon == 1 and Eudemon_State.NowPerson < Eudemon_State.NeedPerson then	-- 如果已经开始召唤且人数不足
			-- 所有召唤的人停止召唤技能
			for k,v in pairs(Eudemon_Stone) do
				role.RoleCancelSkill(MapID, InstanceID, k, SkillID)

				-- 广播
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 1048)			-- 召唤人数不足，召唤守护神被强制终止
				msg.DispatchRoleMsgEvent(k, MsgID)
			end

			-- 清空召唤表
			Eudemon_Stone = {}
			-- 停止召唤
			Eudemon_State.BeginSummon = 0
			city.StopSummon(city_id[CityStruggle_ActID].CityID)
		end

	elseif TargetTypeID == 4900029 then			-- 龙珠
		unit.CancelBuff(MapID, InstanceID, LongZhu_State.LongZhuID, 9999101)		-- 去除封印时特效
		unit.CancelBuff(MapID, InstanceID, OwnerID, 9999401)						-- 去除封印者无敌buff
		if LongZhu_State.RoleID == OwnerID then
			LongZhu_State.RoleID = nil				-- 去除当前封印玩家记录
		end

		-- 同步龙珠状态给地图所有玩家
		SycAllRoleOneInfo(MapID, InstanceID, 2, 0, Role_Alliance[OwnerID])

	elseif TargetTypeID == 4900030 or TargetTypeID == 4900043 or TargetTypeID == 4900044 or TargetTypeID == 4900045 or TargetTypeID == 4900046 then
		unit.CancelBuff(MapID, InstanceID, TargetID, 9999102)		-- 去除封印时特效
	end
end

-- 击破城门
function CDoor_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	-- 找到对应城门 打开
	for k, v in pairs(Door_State) do
		if Door_State[k].CreatureID == CreatureID then
			Door_State[k].Opened = 1
			Door_State[k].Change_Min = CityStruggle_CurMin
			map.OpenCloseDoorByPos(MapID, InstanceID, Door_State[k].x, Door_State[k].y, Door_State[k].z, true)

			-- 同步城门状态
			local CountDown = 20
			if Door_State[k].Repair_Time >= 3 then
				CountDown = -1
			end
			SycAllRoleOneInfo(MapID, InstanceID, 6, k, CountDown)

			-- 广播
			local GuildID = guild.GetRoleGuildID(KillerID)
			local id_msg = 1031 + k - 1
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, id_msg)			-- 正门/东门/西门已被×××帮派的×××攻破！
			msg.AddMsgEvent(MsgID, 19, GuildID)
			msg.AddMsgEvent(MsgID, 2, KillerID)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

			break
		end
	end

	-- 增加攻陷城门积分
	city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, KillerID, 2, 1)

	return true
end

-- 击毁防御联盟物资库
function C4900027_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	Def_Revive_Time = Def_Revive_Time + 5
	city.SetReviveTime(city_id[CityStruggle_ActID].CityID, 1, Def_Revive_Time)

	for k, v in pairs(Material_State) do
		if Material_State[k].Destroied ~= 1 then
			local x, y, z = unit.GetPosition(MapID, InstanceID, CreatureID)
			Material_State[k].Destroied = 1
			Material_State[k].Change_Min = CityStruggle_CurMin
			Material_State[k].x = x
			Material_State[k].y = y
			Material_State[k].z = z
			break
		end
	end

	-- 获取当前未被击毁的物资库数量
	local num = 0
	for k, v in pairs(Material_State) do
		if Material_State[k].Destroied ~= 1 then
			num = num + 1
		end
	end

	-- 同步
	SycAllRoleOneInfo(MapID, InstanceID, 5, 0, num)

	-- 增加攻陷物资库积分
	city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, KillerID, 3, 1)

	-- 又是广播
	local GuildID = guild.GetRoleGuildID(KillerID)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1037)			-- 摧毁了一座防御联盟的物资库
	msg.AddMsgEvent(MsgID, 19, GuildID)
	msg.AddMsgEvent(MsgID, 2, KillerID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end

-- 击毁招魂幡
function C4900028_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, KillerID)
	if CreatureID == Atk_ZhaoHunFan.CreatureID then
		Atk_ZhaoHunFan.use = 0
	elseif CreatureID == Def_ZhaoHunFan.CreatureID then
		Def_ZhaoHunFan.ues = 0
	else

	end
end

--[[

-- 使用卫兵塑像
function I1350011_Use(MapID, InstanceID, TypeID, TargetID)
	-- 在原地刷出一个卫兵塑像
	local tbl = {CreatureID, x, y, z}
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	tbl.x = x
	tbl.y = y
	tbl.z = z
	tbl.CreatureID = map.MapCreateCreature(MapID, InstanceID, 1000893, x, y, z)

	for k, v in pairs(city_id) do
		if MapID == v.MapID then
			city_id[k].GuardNum = city_id[k].GuardNum + 1
			table.insert(city_guard[city_id[k].GuardIndex], tbl)
		end
	end
end

-- 能否使用卫兵塑像
function I1350011_CanUse(MapID, InstanceID, TypeID, TargetID)
	for k, v in pairs(city_id) do
		if MapID == v.MapID then
			if act.GetActIsStart(k) == true then	-- 城战开始后无法使用
				return 32, true
			elseif city_id[k].GuardNum >= 20 then	-- 最多摆放20个卫兵塑像
				return 32, true
			end
		end
	end

	return 0, false
end

]]

-- 能否使用招魂幡
function I3300006_CanUse(MapID, InstanceID, TypeID, TargetID)
	if MapID ~= 3695619387 then						-- 非城战场景不能使用
		return 47, 0
	end

	local guildID = guild.GetRoleGuildID(TargetID)
	local bCanNotUse = map.IsInArea(MapID, InstanceID, TargetID, 1442)
	if bCanNotUse == true then
		return 43, 0
	elseif guildID ~= 4294967295 then
		if guild.GetRoleGuildPos(guildID, TargetID) == 2 then		-- 帮主才可以使用
			if Role_Alliance[TargetID] == 0 then
				if Atk_ZhaoHunFan.use == 1 then
					return 39, 0
				else
					return 0, 0
				end
			else
				if Def_ZhaoHunFan.use == 1 then
					return 39, 0
				else
					return 0, 0
				end
			end
		else
			return 32, 0
		end

	else
		return 32, 0
	end

end

-- 使用招魂幡
function I3300006_Use(MapID, InstanceID, TypeID, TargetID)
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	if Role_Alliance[TargetID] == 0 then	-- 进攻方
		Atk_ZhaoHunFan.CreatureID = map.MapCreateCreature(MapID, InstanceID, 4900028, x, y, z)
		Atk_ZhaoHunFan.use = 1
		Atk_ZhaoHunFan.x = x
		Atk_ZhaoHunFan.y = y
		Atk_ZhaoHunFan.z = z
		unit.SetState(MapID, InstanceID, Atk_ZhaoHunFan.CreatureID, 8)		-- 设置进攻方
		--unit.AddBuff(MapID, InstanceID, Atk_ZhaoHunFan.CreatureID, 2028001, Atk_ZhaoHunFan.CreatureID)
	else									-- 防守方
		Def_ZhaoHunFan.CreatureID = map.MapCreateCreature(MapID, InstanceID, 4900028, x, y, z)
		Def_ZhaoHunFan.use = 1
		Def_ZhaoHunFan.x = x
		Def_ZhaoHunFan.y = y
		Def_ZhaoHunFan.z = z
		unit.SetState(MapID, InstanceID, Def_ZhaoHunFan.CreatureID, 9)		-- 设置防御方
		--unit.AddBuff(MapID, InstanceID, Def_ZhaoHunFan.CreatureID, 2028001, Def_ZhaoHunFan.CreatureID)
	end
end

-- 出城暗道西
function C4900051_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if CityStruggle_IsStart == 1 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, MapID, 213, 232, 321)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1022)			-- 目前非城战时间，偷袭地道已关闭！
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end

-- 出城暗道东
function C4900052_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	if CityStruggle_IsStart == 1 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, MapID, 1287, 209, 173)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1022)			-- 目前非城战时间，偷袭地道已关闭！
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	end
end

-- 青龙石料
function I3300004_CanUse(MapID, InstanceID, TypeID, TargetID)
	if CityStruggle_IsStart == 1 and MapID == city_id[CityStruggle_ActID].MapID then
		local bCanUse = map.IsInArea(MapID, InstanceID, TargetID, 1424)
		if bCanUse == true then
			return 0, false
		else
			return 43, false	-- 不可在该位置使用物品
		end
	end
	return 43, false
end

function I3300004_Use(MapID, InstanceID, TypeID, TargetID)
	unit.ChangeHP(MapID, InstanceID, Door_State[1].CreatureID, 10000)
	unit.ChangeHP(MapID, InstanceID, Door_State[3].CreatureID, 10000)

	-- 增加使用石料积分
	city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, TargetID, 4, 1)
end

-- 白虎石料
function I3300005_CanUse(MapID, InstanceID, TypeID, TargetID)
	if CityStruggle_IsStart == 1 and MapID == city_id[CityStruggle_ActID].MapID then
		local bCanUse = map.IsInArea(MapID, InstanceID, TargetID, 1425)
		if bCanUse == true then
			return 0, false
		else
			return 43, false	-- 不可在该位置使用物品
		end
	end
	return 43, false
end

function I3300005_Use(MapID, InstanceID, TypeID, TargetID)
	unit.ChangeHP(MapID, InstanceID, Door_State[1].CreatureID, 10000)
	unit.ChangeHP(MapID, InstanceID, Door_State[2].CreatureID, 10000)

	-- 增加使用石料积分
	city.AddRoleData(MapID, InstanceID, city_id[CityStruggle_ActID].CityID, TargetID, 4, 1)
end

-- 地图初始化时
function CityStruggle_OnMapInit(MapID, InstanceID)
	-- 打开地图所有的门
	map.OpenCloseAllDoors(MapID, InstanceID, true)

	-- 刷出非城战时龙珠
	LongZhu_State.LongZhuID = map.MapCreateCreature(MapID, -1, 4900029, 731, 11180, 754)
	-- 挂上特效

end

-- 城门被攻击
function CDoor_OnBeAttack(MapID, InstanceID, CreatureID, SrcID)

	-- 广播城门生命值
	local MainDoorHpPercent = 0
	local WestDoorHpPercent = 0
	local EastDoorHpPercent = 0
	if Door_State[1].Opened ~= 1 then
		MainDoorHpPercent = unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[1].CreatureID, 9) * 100 / unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[1].CreatureID, 8)
		MainDoorHpPercent = math.floor(100 - MainDoorHpPercent)
	end

	if Door_State[2].Opened ~= 1 then
		WestDoorHpPercent = unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[2].CreatureID, 9) * 100 / unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[2].CreatureID, 8)
		WestDoorHpPercent = math.floor(100 - WestDoorHpPercent)
	end

	if Door_State[3].Opened ~= 1 then
		EastDoorHpPercent = unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[3].CreatureID, 9) * 100 / unit.GetAttValue(city_id[CityStruggle_ActID].MapID, -1, Door_State[3].CreatureID, 8)
		EastDoorHpPercent = math.floor(100 - EastDoorHpPercent)
	end

	if MainDoorHpPercent ~= 0 and MainDoorHpPercent ~= 100 and (100 - MainDoorHpPercent)/10 <= Door_State[1].HpNode then
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for k, v in pairs(RoleTbl) do
			local MsgID = msg.BeginMsgEvent()
			--msg.AddMsgEvent(MsgID, 13,	1)
			msg.AddMsgEvent(MsgID, 101, 1023)
			msg.AddMsgEvent(MsgID, 9, MainDoorHpPercent)
			--msg.DispatchMapMsgEvent(MsgID, city_id[CityStruggle_ActID].MapID, -1)
			msg.DispatchRoleMsgEvent(v, MsgID)
		end
		Door_State[1].HpNode = Door_State[1].HpNode - 1
	end
	if WestDoorHpPercent ~= 0 and WestDoorHpPercent ~= 100 and (100 - WestDoorHpPercent)/10 <= Door_State[2].HpNode then
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for k, v in pairs(RoleTbl) do
			local MsgID = msg.BeginMsgEvent()
			--msg.AddMsgEvent(MsgID, 13,	1)
			msg.AddMsgEvent(MsgID, 101, 1024)
			msg.AddMsgEvent(MsgID, 9, WestDoorHpPercent)
			--msg.DispatchMapMsgEvent(MsgID, city_id[CityStruggle_ActID].MapID, -1)
			msg.DispatchRoleMsgEvent(v, MsgID)
		end
		Door_State[2].HpNode = Door_State[2].HpNode - 1
	end
	if EastDoorHpPercent ~= 0 and EastDoorHpPercent ~= 100 and (100 - EastDoorHpPercent)/10 <= Door_State[3].HpNode then
		local RoleTbl = map.GetMapAllRoleID(MapID, InstanceID)
		for k, v in pairs(RoleTbl) do
			local MsgID = msg.BeginMsgEvent()
			--msg.AddMsgEvent(MsgID, 13,	1)
			msg.AddMsgEvent(MsgID, 101,	1025)
			msg.AddMsgEvent(MsgID, 9, EastDoorHpPercent)
			--msg.DispatchMapMsgEvent(MsgID, city_id[CityStruggle_ActID].MapID, -1)
			msg.DispatchRoleMsgEvent(v, MsgID)
		end
		Door_State[3].HpNode = Door_State[3].HpNode - 1
	end

end

-- 判断玩家士气是否为0-200
function FX_ResetMorale(MapID, InstanceID, RoleID)
	local Morale = unit.GetAttValue(MapID, InstanceID, RoleID, 63)
	if Morale ~= nil then
		if Morale < 0 then
			unit.SetAttValue(MapID, InstanceID, RoleID, 63, 0)
		elseif Morale > 200 then
			unit.SetAttValue(MapID, InstanceID, RoleID, 63, 200)
		else
		end
	end
end

-- 守护神召唤完成
function CityStruggle_OnFinishEudemonSummon()
	-- 停止所有防守玩家的召唤技能
	for k,v in pairs(Eudemon_Stone) do
		role.RoleCancelSkill(city_id[CityStruggle_ActID].MapID, -1, k, 9001801)

	end

	Eudemon_State.Summoned = 1
	Eudemon_State.BeginSummon = 0
	Eudemon_Stone = {}

	-- 刷出守护神

	-- 广播
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 1047)
	msg.DispatchMapMsgEvent(MsgID, city_id[CityStruggle_ActID].MapID, -1)

	-- 增加士气
	for k, v in pairs(Role_Alliance) do
		if v == 1 then
			unit.ModAttValue(city_id[CityStruggle_ActID].MapID, -1, k, 63, 10)
			FX_ResetMorale(city_id[CityStruggle_ActID].MapID, -1, k)
		end
	end
end

--城市广播
function City_OnBroadCast(BroadType, Para1, Para2, Para3, Para4, Para5)

	--0//全服消息广播：“###（帮派名称）获得###（禁城名称）的统治权！”
	--1//设置成功后，系统全世界广播：“###（城市名称）的税率变更为###。”
	--2//系统全服消息广播：“本次城战的各参与方已确定！请各位可在城战信息界面中查询！”

	--3//被确认拥有城战攻击权的各个帮派，全帮派成员提示：“帮派获得了对###（禁城名称）的攻击权！请各位帮众积极备战！”
	--4//被确认的帮派全帮派成员提示：“本帮已被同意参与###（禁城名称）的城战，将作为防御方协助防守，请各位成员积极备战”
	--5//全体帮派提示：“帮派已对###（禁城名称）进行攻击报名！”
	--6//全体帮派提示：“帮派已对###（禁城名称）进行协助防御报名！”
	--7//同时全帮派成员消息：“###（角色名称）主动交纳####（道具名称），###（城市名称）生产力提升###点。”
	--8//###（帮主名称）帮主决定申请管理###（城市名称），统治权由全体帮派成员对应该城市氏族类型的氏族声望之和决定，结果将于周六0点进行计算！。
	if BroadType == 0 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1100)
		msg.AddMsgEvent(MsgID, 19, Para1)
		msg.AddMsgEvent(MsgID, 6, Para2)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif BroadType == 1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1101)
		msg.AddMsgEvent(MsgID, 6, Para1)
		msg.AddMsgEvent(MsgID, 9, Para2)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif BroadType == 2 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1102)
		msg.DispatchWorldMsgEvent(MsgID)
	elseif BroadType == 3 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1103)
		msg.AddMsgEvent(MsgID, 6, Para1)
		msg.DispatchGuildMsgEvent(MsgID,Para2)
	elseif BroadType == 4 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1104)
		msg.AddMsgEvent(MsgID, 6, Para1)
		msg.DispatchGuildMsgEvent(MsgID,Para2)
	elseif BroadType == 5 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1105)
		msg.AddMsgEvent(MsgID, 6, Para1)
		msg.DispatchGuildMsgEvent(MsgID,Para2)
	elseif BroadType == 6 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1106)
		msg.AddMsgEvent(MsgID, 6, Para1)
		msg.DispatchGuildMsgEvent(MsgID,Para2)
	elseif BroadType == 7 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1107)
		msg.AddMsgEvent(MsgID, 2, Para1)
		msg.AddMsgEvent(MsgID, 4, Para2)
		msg.AddMsgEvent(MsgID, 6, Para3)
		msg.AddMsgEvent(MsgID, 9, Para4)
		msg.DispatchGuildMsgEvent(MsgID,Para5)
	elseif BroadType == 8 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1108)
		msg.AddMsgEvent(MsgID, 2, Para1)
		msg.AddMsgEvent(MsgID, 6, Para2)
		msg.DispatchGuildMsgEvent(MsgID,Para3)
	end

end



aux.RegisterActEvent(70, 0, "CityStruggle_OnInit")
aux.RegisterActEvent(70, 1, "CityStruggle_OnTimer")
aux.RegisterActEvent(70, 2, "CityStruggle_OnStart")
aux.RegisterActEvent(70, 3, "CityStruggle_OnEnd")
aux.RegisterActEvent(70, 4, "CityStruggle_OnTimerMin")
aux.RegisterActEvent(70, 6, "CityStruggle_OnBroadCast")
aux.RegisterActEvent(70, 7, "CityStruggle_OnFinishEudemonSummon")

aux.RegisterMapEvent("c03", 0, "CityStruggle_OnMapInit")
aux.RegisterMapEvent("c03", 2, "CityStruggle_OnPlayerEnter")
aux.RegisterMapEvent("c03", 3, "CityStruggle_OnPlayerLeave")
aux.RegisterMapEvent("c03", 4, "CityStruggle_OnCreatureDie")
aux.RegisterMapEvent("c03", 5, "CityStruggle_OnRoleDie")
aux.RegisterMapEvent("c03", 11, "CityStruggle_RoleRevive")
aux.RegisterMapEvent("c03", 12, "CityStruggle_CanEnterWhenOnline")
aux.RegisterMapEvent("c03", 16, "CityStruggle_FriendEnemy")


aux.RegisterCreatureEvent(4900004, 7, "C4900004_OnTalk")		-- 凤翔禁城传送人
aux.RegisterCreatureEvent(4900005, 7, "C4900005_OnTalk")		-- 凤翔卫城传送人
aux.RegisterCreatureEvent(4900051, 7, "C4900051_OnTalk")		-- 出城暗道西
aux.RegisterCreatureEvent(4900052, 7, "C4900052_OnTalk")		-- 出城暗道东
aux.RegisterCreatureEvent(4900027, 4, "C4900027_OnDie")			-- 物资库
aux.RegisterCreatureEvent(4900028, 4, "C4900028_OnDie")			-- 招魂幡
aux.RegisterCreatureEvent(4900019, 4, "CDoor_OnDie")			-- 各种城门
aux.RegisterCreatureEvent(4900020, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900021, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900022, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900023, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900024, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900025, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900026, 4, "CDoor_OnDie")
aux.RegisterCreatureEvent(4900019, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900020, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900021, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900022, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900023, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900024, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900025, 14, "CDoor_OnBeAttack")
aux.RegisterCreatureEvent(4900026, 14, "CDoor_OnBeAttack")


aux.RegisterSkillEvent(9001501, 0, "OnSealCanCast")
aux.RegisterSkillEvent(9001501, 2, "OnSealSkill")
aux.RegisterSkillEvent(9001501, 3, "OnSealCancelSkill")
aux.RegisterSkillEvent(9001601, 0, "OnSealCanCast")
aux.RegisterSkillEvent(9001601, 2, "OnSealSkill")
aux.RegisterSkillEvent(9001601, 3, "OnSealCancelSkill")

aux.RegisterItemEvent(3300006, 0, "I3300006_CanUse")			-- 招魂幡
--aux.RegisterItemEvent(1350012, 0, "I1350012_CanUse")
aux.RegisterItemEvent(3300004, 0, "I3300004_CanUse")
aux.RegisterItemEvent(3300005, 0, "I3300005_CanUse")

aux.RegisterItemEvent(3300006, 1, "I3300006_Use")				-- 招魂幡
--aux.RegisterItemEvent(1350012, 1, "I1350012_Use")
aux.RegisterItemEvent(3300004, 1, "I3300004_Use")
aux.RegisterItemEvent(3300005, 1, "I3300005_Use")

aux.RegisterMapEvent("c03", 22, "City_OnBroadCast") --城市广播
