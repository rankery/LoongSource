Quest_ChuMo = {}
Quest_ChuMo[1]={TargetType=QuestTargetType["Collect"], TargetID1=3200151, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[2]={TargetType=QuestTargetType["Collect"], TargetID1=3200152, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[3]={TargetType=QuestTargetType["Collect"], TargetID1=3200153, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[4]={TargetType=QuestTargetType["Collect"], TargetID1=3200154, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[5]={TargetType=QuestTargetType["Collect"], TargetID1=3200155, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[6]={TargetType=QuestTargetType["Collect"], TargetID1=3200156, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[7]={TargetType=QuestTargetType["Collect"], TargetID1=3200157, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[8]={TargetType=QuestTargetType["Collect"], TargetID1=3200158, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[9]={TargetType=QuestTargetType["Collect"], TargetID1=3200159, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[10]={TargetType=QuestTargetType["Collect"], TargetID1=3200160, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[11]={TargetType=QuestTargetType["Collect"], TargetID1=3200161, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[12]={TargetType=QuestTargetType["Collect"], TargetID1=3200162, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}
Quest_ChuMo[13]={TargetType=QuestTargetType["Collect"], TargetID1=3200163, TargetID2=0, TargetID3=0, TargetID4=0, Num1=1, Num2=0, Num3=0, Num4=0, MsgID=16004}

--任务接取函数

function q20079_OnQuestInit(MapID, InstanceID, QuestID, RoleID, NPCID)

	ChuMo_GetPoint(MapID, InstanceID, RoleID)

	local RoleLevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local Index=math.floor(RoleLevel/5)-7

	if Index>13 then
		Index=13
	end

	--初始化任务的动态数据
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Index"], Index)
	role.QuestInit(RoleID, 20079, GetRandQuestTable(Quest_ChuMo ,Index))

	--同步客户端总任务环数和任务说明
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 11, QuestID)
	msg.AddMsgEvent(MsgID, 10, LoopNum)
	msg.AddMsgEvent(MsgID, 1, Quest_ChuMo[Index].MsgID)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

end



--完成任务
function q20079_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)
	--击杀怪物种类，用于计算经验倍率
	local MonRate = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Mon"])

	local exp_award = {}
	exp_award[0] = 950
	exp_award[4] = 1510
	exp_award[5] = 2420
	exp_award[6] = 3660
	exp_award[7] = 5040
	exp_award[8] = 6540
	exp_award[9] = 8180
	exp_award[10]= 9940

	local exp_extra=0

	local rolelevel1=role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Index"])
	rolelevel1=(rolelevel1+7)*5

	local rolelevel2=role.GetRoleLevel(MapID, InstanceID, RoleID)

	local klevel=0

	local rolelevel=0

	if rolelevel1>rolelevel2 then
		rolelevel = rolelevel2
		klevel = math.floor(rolelevel2/10)
	else
		rolelevel = rolelevel1
		klevel = math.floor(rolelevel1/10)
	end

	if klevel>10 then
		klevel = 10
	end

	--得到当前任务总环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"])
	--计算环数
	local LoopNum_Ring = LoopNum % 10
	if LoopNum_Ring == 0 then
		LoopNum_Ring = 10
	end

	local Exp=0
	--增加每天前10次接取任务经验值加成系数，1.5倍
	local IsDouble = 0
	if TotalNum <= 10 then
		IsDouble = 1.5
	else
		IsDouble = 1
	end
	--计算经验奖励
	if LoopNum_Ring == 10 then
		Exp = exp_award[klevel]*(1+(LoopNum_Ring-1)*0.15)*MonRate*IsDouble+ exp_extra
		local randomitem=math.random(2)
		if randomitem==1 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2000022, 2, -1, 8, 102)
		else
			role.AddRoleItem(MapID, InstanceID, RoleID, 2000222, 2, -1, 8, 102)
		end
		--[[if LoopNum == 10 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 2005201, 1, -1, 8, 102)
		elseif LoopNum > 10 then
			local R = math.random(1,100)
			if R < 40 then
				role.AddRoleItem(MapID, InstanceID, RoleID, 2005201, 1, -1, 8, 102)
			end
		end]]


		local y = math.random(100)
		local x = math.random(100)
		if y <= 70 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303006, 1, -1, 8, 102) --百味灵种
		elseif y <= 90 and y >= 71 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303008, 1, -1, 8, 102) --玫瑰灵种
		elseif y >= 91 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303007, 1, -1, 8, 102) --珍肴灵种
			local MsgID = msg.BeginMsgEvent()
	        msg.AddMsgEvent(MsgID, 13, 1)
	        msg.AddMsgEvent(MsgID, 1, 2127) --&lt;p1&gt;辛苦完成一轮侠义门-除魔任务，意外获得“&lt;p2&gt”一个
	        msg.AddMsgEvent(MsgID, 2, RoleID)
	        msg.AddMsgEvent(MsgID, 4, 3303007)
			msg.DispatchWorldMsgEvent(MsgID)
		end

		if x <= 40 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303009, 1, -1, 8, 102) --生命之水
		elseif x > 40 and x < 81 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303011, 1, -1, 8, 102) --培土农肥
        elseif x >= 81 and x < 91 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303010, 1, -1, 8, 102) --生命灵水
			local MsgID = msg.BeginMsgEvent()
	        msg.AddMsgEvent(MsgID, 13, 1)
	        msg.AddMsgEvent(MsgID, 1, 2127) --&lt;p1&gt;辛苦完成一轮侠义门-除魔任务，意外获得“&lt;p2&gt”一个
	        msg.AddMsgEvent(MsgID, 2, RoleID)
	        msg.AddMsgEvent(MsgID, 4, 3303010)
			msg.DispatchWorldMsgEvent(MsgID)
        elseif x > 90 then
		    role.AddRoleItem(MapID, InstanceID, RoleID, 3303012, 1, -1, 8, 102) --培土灵肥
            local MsgID = msg.BeginMsgEvent()
	        msg.AddMsgEvent(MsgID, 13, 1)
	        msg.AddMsgEvent(MsgID, 1, 2127) --&lt;p1&gt;辛苦完成一轮侠义门-除魔任务，意外获得“&lt;p2&gt”一个
	        msg.AddMsgEvent(MsgID, 2, RoleID)
	        msg.AddMsgEvent(MsgID, 4, 3303012)
			msg.DispatchWorldMsgEvent(MsgID)
		end
	else
		Exp = exp_award[klevel]*(1+(LoopNum_Ring-1)*0.15)*MonRate*IsDouble
	end

	--奖励玩家经验
	role.AddRoleExp(MapID, InstanceID, RoleID, Exp)

	--当前环数加一,总任务环数加一
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"], LoopNum+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"], TotalNum+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Pos"], 0)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Mon"], 0)

		--增加进阶实力值
					Increase_shili(MapID, InstanceID, RoleID, 0, 2, "jinjieshili_Q")
    --开学活动
    local curhour = os.date("%H")
	local act_start = act.GetActIsStart(77)
	if act_start == true and (curhour=="19" or curhour=="20" or curhour=="21" or curhour=="22" or curhour=="23") then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303206, 1, -1, 8, 102)
		HuoDeLeiFengMao(RoleID)
		local i = math.random(3)
		if i == 1 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 3303208, 1, -1, 8, 102)
		end
	end
end

--放弃任务
function q20079_OnCancel(MapID, InstanceID, QuestID, RoleID)

	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"])
	local k = LoopNum % 10
	if k == 0 then
		k=10
	end
	--总任务环数加一，当前环数置为1
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"], LoopNum-k+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"], TotalNum+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Pos"], 0)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Mon"], 0)
end



--接受任务前各种判断
function q20079_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断本日任务接取次数是否已满
	--得到当前任务总任务环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"])
	local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Update"])

	--总环数为0时，设置总环数为第一环
	if 0 == LoopNum then
		LoopNum = 1
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"], LoopNum)
	end
	if 0 == TotalNum then
		TotalNum = 1
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"], TotalNum)
	end

	--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
	local CurTime = tonumber(os.date("%j"))
	if CurTime~=UpdateTime then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"], 1)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Total"], 1)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Update"], CurTime)
	end
	--若本日已接取50次，则无法再接取
	if TotalNum > 50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 147)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end



	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	--得到队伍成员ID

	if TeamID==4294967295 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 66)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

	local TeamMaxNum=0
--	while Role[TeamMaxNum]~=4294967295 do
--		TeamMaxNum = TeamMaxNum+1
--	end

	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
	        TeamMaxNum = TeamMaxNum+1
	    end
	end

--	TeamMaxNum=TeamMaxNum-1

	if TeamMaxNum<3 then
		--提示玩家需要组队
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 66)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	return 1

end

function x20079_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Loop"])
	local LoopNum_Ring = LoopNum % 10
	if LoopNum_Ring == 0 then
		--判断背包空闲空间是否足够
		local FreeSize = role.GetBagFreeSize(RoleID)
		if(FreeSize < 1) then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 142)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			return 0
		end
	end

	return 1
end

--是否可以使用照妖镜

function IZhaoYaoJing_CanUseItem(MapID, InstanceID, TypeID, RoleID)

	--检测玩家已到达了指定坐标点
	if role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 20079)==false then
		return 32, false
	end

	--检测玩家已到达了指定坐标点
	local CanUse = ChuMo_EnterNearArea(MapID, InstanceID, RoleID)
	--如果玩家不在指定坐标附近，将指定坐标发给客户端
	if CanUse ~= true then
		local Index = role.GetRoleScriptData(RoleID, 1, RoleDataType["ChuMo_Pos"])
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 13, 0)
	    msg.AddMsgEvent(MsgID, 1, 254)
	    msg.AddMsgEvent(MsgID, 6, Quest_ZhuE_Pos[Index].MapCrc)
	    msg.AddMsgEvent(MsgID, 9, Quest_ZhuE_Pos[Index].x)
	    msg.AddMsgEvent(MsgID, 9, Quest_ZhuE_Pos[Index].z)
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)

	    local MsgID1 = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID1, 26, 254)
	    msg.AddMsgEvent(MsgID1, 6, Quest_ZhuE_Pos[Index].MapCrc)
	    msg.AddMsgEvent(MsgID1, 9, Quest_ZhuE_Pos[Index].x)
	    msg.AddMsgEvent(MsgID1, 9, Quest_ZhuE_Pos[Index].z)
	    msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	    return 36, false
    else
        return 0, false
	end
end


--使用铸铜照妖镜
function IZhaoYaoJing_UseItem(MapID, InstanceID, TypeID, RoleID)

	local a = math.floor(role.GetRoleLevel(MapID, InstanceID, RoleID)/5)-8
	if a>12 then
		a=12
	end
	local b = math.random(5)
	local c = TypeID-3200202

	local MonsterID = 1541000+a*15+c*5+b

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)

	map.MapCreateCreature(MapID, InstanceID, MonsterID, x+2, y, z+2)

end

function ChuMo_OnDie(MapID, InstanceID, CreatureID, TypeID, RoleID)

	local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	--得到队伍成员ID

	if TeamID==4294967295 then
		--提示玩家需要组队
		return 0
	end

	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)

	Index=math.floor(((TypeID-1541000)%15)/5+1)

	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
	       role.SetRoleScriptData(Role[i], 1, RoleDataType["ChuMo_Mon"], Index)
	    end
	end
end



aux.RegisterQuestEvent(20079, 1, "q20079_OnComplete")
aux.RegisterQuestEvent(20079, 7, "q20079_OnQuestInit")
aux.RegisterQuestEvent(20079, 2, "q20079_OnCancel")
aux.RegisterQuestEvent(20079, 4, "q20079_OnCheckAccept")
aux.RegisterQuestEvent(20079, 5, "x20079_OnCheckComplete")

aux.RegisterItemEvent(3200202, 0, "IZhaoYaoJing_CanUseItem")
aux.RegisterItemEvent(3200202, 1, "IZhaoYaoJing_UseItem")
aux.RegisterItemEvent(3200203, 0, "IZhaoYaoJing_CanUseItem")
aux.RegisterItemEvent(3200203, 1, "IZhaoYaoJing_UseItem")
aux.RegisterItemEvent(3200204, 0, "IZhaoYaoJing_CanUseItem")
aux.RegisterItemEvent(3200204, 1, "IZhaoYaoJing_UseItem")

aux.RegisterCreatureEvent(1541001, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541002, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541003, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541004, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541005, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541006, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541007, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541008, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541009, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541010, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541011, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541012, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541013, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541014, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541015, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541016, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541017, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541018, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541019, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541020, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541021, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541022, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541023, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541024, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541025, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541026, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541027, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541028, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541029, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541030, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541031, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541032, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541033, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541034, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541035, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541036, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541037, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541038, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541039, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541040, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541041, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541042, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541043, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541044, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541045, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541046, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541047, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541048, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541049, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541050, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541051, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541052, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541053, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541054, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541055, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541056, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541057, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541058, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541059, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541060, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541061, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541062, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541063, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541064, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541065, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541066, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541067, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541068, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541069, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541070, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541071, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541072, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541073, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541074, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541075, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541076, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541077, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541078, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541079, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541080, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541081, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541082, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541083, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541084, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541085, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541086, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541087, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541088, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541089, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541090, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541091, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541092, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541093, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541094, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541095, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541096, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541097, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541098, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541099, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541100, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541101, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541102, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541103, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541104, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541105, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541106, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541107, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541108, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541109, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541110, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541111, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541112, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541113, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541114, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541115, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541116, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541117, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541118, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541119, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541120, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541121, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541122, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541123, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541124, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541125, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541126, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541127, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541128, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541129, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541130, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541131, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541132, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541133, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541134, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541135, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541136, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541137, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541138, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541139, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541140, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541141, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541142, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541143, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541144, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541145, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541146, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541147, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541148, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541149, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541150, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541151, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541152, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541153, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541154, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541155, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541156, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541157, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541158, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541159, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541160, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541161, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541162, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541163, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541164, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541165, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541166, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541167, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541168, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541169, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541170, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541171, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541172, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541173, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541174, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541175, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541176, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541177, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541178, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541179, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541180, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541181, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541182, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541183, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541184, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541185, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541186, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541187, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541188, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541189, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541190, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541191, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541192, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541193, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541194, 4, "ChuMo_OnDie")
aux.RegisterCreatureEvent(1541195, 4, "ChuMo_OnDie")
