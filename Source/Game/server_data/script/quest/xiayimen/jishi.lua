Quest_JiShi = {}
Quest_JiShi[1]={TargetType=QuestTargetType["NPCTalk"], TargetID1=4500311, TargetID2=4500312, TargetID3=4500313, TargetID4=0, Num1=0, Num2=0, Num3=0, Num4=0, MsgID=16005}


--任务接取函数

function q20080_OnQuestInit(MapID, InstanceID, QuestID, RoleID, NPCID)

	local Money_Need = {}
	Money_Need[25]=1000
	Money_Need[30]=2000
	Money_Need[35]=3300
	Money_Need[40]=4900
	Money_Need[45]=6800
	Money_Need[50]=9000
	Money_Need[55]=11500
	Money_Need[60]=14300
	Money_Need[65]=17400
	Money_Need[70]=20800
	Money_Need[75]=24500
	Money_Need[80]=28500
	Money_Need[85]=32800
	Money_Need[90]=37400
	Money_Need[95]=42300
	Money_Need[100]=47500

	local rolelevel=role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5)*5

	if k > 100 then
	   k = 100
	end

	role.DecRoleSilver(MapID, InstanceID, RoleID, Money_Need[k]*3, 102)-- 系统扣除对应金钱

	JiShi_GetPoint(MapID, InstanceID, RoleID)--获取随机坐标点

	--初始化任务的动态数据
	role.QuestInit(RoleID, QuestID, GetRandQuestTable(Quest_JiShi ,1))



	--同步客户端总任务环数和任务说明
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"])

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 11, QuestID)
	msg.AddMsgEvent(MsgID, 10, LoopNum)
	msg.AddMsgEvent(MsgID, 1, Quest_JiShi[1].MsgID)
	msg.DispatchRoleMsgEvent(RoleID, MsgID)

end

--完成任务
function q20080_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	local exp_award = {}
	exp_award[25]=4050
	exp_award[30]=5450
	exp_award[35]=6700
	exp_award[40]=9200
	exp_award[45]=11300
	exp_award[50]=14752
	exp_award[55]=18432
	exp_award[60]=22312
	exp_award[65]=26392
	exp_award[70]=30672
	exp_award[75]=35152
	exp_award[80]=39832
	exp_award[85]=44712
	exp_award[90]=49792
	exp_award[95]=55072
	exp_award[100]=60552
	local exp_extra=0

	local rolelevel=role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5)*5
	if k>100 then
		k = 100
	end

	--得到当前任务总环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"])
	--计算环数

	local Exp=0
	local LoopNum_Ring = LoopNum % 5
	if LoopNum_Ring==0 then
		LoopNum_Ring=5
	end

	if LoopNum_Ring==5 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3200005, 1, -1, 8, 102)
		Exp = exp_award[k]*(1+(LoopNum-1)*0.5)+ exp_extra
	else
		Exp = exp_award[k]*(1+(LoopNum-1)*0.5)
	end

	--奖励玩家经验
	role.AddRoleExp(MapID, InstanceID, RoleID, Exp)

	--金钱奖励系数
	for i=1,30 do
		if i*5>rolelevel then
			silver=activity_money[i*5]*activity_money_rate["JiShi"]
			break
		end
	end
	silver = silver*(1+(LoopNum_Ring-1)*0.3)/2.35
	role.AddRoleSilver(MapID, InstanceID, RoleID, silver, 102)

	--当前环数加一,总任务环数加一
	role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"], LoopNum+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"], TotalNum+1)

	--增加进阶实力值
				Increase_shili(MapID, InstanceID, RoleID, 0, 2, "jinjieshili_Q")
    --[[开学活动
    local curhour = os.date("%H")
	local act_start = act.GetActIsStart(77)
	if act_start == true and (curhour=="19" or curhour=="20" or curhour=="21" or curhour=="22" or curhour=="23") then
	    role.AddRoleItem(MapID, InstanceID, RoleID, 3303206, 1, -1, 8, 102)
		HuoDeLeiFengMao(RoleID)
		local i = math.random(3)
		if i == 1 then
			role.AddRoleItem(MapID, InstanceID, RoleID, 3303208, 1, -1, 8, 102)
		end
	end]]

end

function q20080_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)

	local FreeSize = role.GetBagFreeSize(RoleID)
	if(FreeSize < 1) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 141)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断本日任务接取次数是否已满
	--得到当前任务总任务环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"])
	local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Update"])

	--总环数为0时，设置总环数为第一环
	if 0 == LoopNum then
		LoopNum = 1
		role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"], LoopNum)
	end
	if 0 == TotalNum then
		TotalNum = 1
		role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"], TotalNum)
	end

	--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
	local CurTime = tonumber(os.date("%j"))
	if CurTime~=UpdateTime then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"], 1)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"], 1)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Update"], CurTime)
	end
	--若本日已完成10次，则无法再接取
	if TotalNum > 10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 147)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	local Money_Need = {}
	Money_Need[25]=1000
	Money_Need[30]=2000
	Money_Need[35]=3300
	Money_Need[40]=4900
	Money_Need[45]=6800
	Money_Need[50]=9000
	Money_Need[55]=11500
	Money_Need[60]=14300
	Money_Need[65]=17400
	Money_Need[70]=20800
	Money_Need[75]=24500
	Money_Need[80]=28500
	Money_Need[85]=32800
	Money_Need[90]=37400
	Money_Need[95]=42300
	Money_Need[100]=47500
	local rolelevel=role.GetRoleLevel(MapID, InstanceID, RoleID)
	local money = role.GetRoleSilver(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/5)*5

	if k > 100 then
	   k = 100
	end

	if money < Money_Need[k]*3 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 284)
		msg.AddMsgEvent(MsgID, 9, Money_Need[k]*3)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 149)
		msg.AddMsgEvent(MsgID, 9, Money_Need[k]*3)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

		return 1
	end
end



--放弃任务
function q20080_OnCancel(MapID, InstanceID, QuestID, RoleID)

	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"])
	local TotalNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"])
	local k = LoopNum % 10
	if k == 0 then
		k=10
	end
	--总任务环数加一，当前环数置为1
	role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Loop"], LoopNum-k+1)
	role.SetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Total"], TotalNum+1)
end



--是否可以使用诛恶密信
function I3200004_CanUseItem(MapID, InstanceID, TypeID, RoleID)
	--检测玩家已到达了指定坐标点
	local CanUse = JiShi_EnterNearArea(MapID, InstanceID, RoleID)
	--如果玩家不在指定坐标附近，将指定坐标发给客户端
	if CanUse == false then
		local Index = role.GetRoleScriptData(RoleID, 1, RoleDataType["JiShi_Pos"])
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 13, 0)
	    msg.AddMsgEvent(MsgID, 1, 255)
	    msg.AddMsgEvent(MsgID, 6, Quest_ZhuE_Pos[Index].MapCrc)
	    msg.AddMsgEvent(MsgID, 9, Quest_ZhuE_Pos[Index].x)
	    msg.AddMsgEvent(MsgID, 9, Quest_ZhuE_Pos[Index].z)
	    msg.DispatchRoleMsgEvent(RoleID, MsgID)

	    local MsgID1 = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID1, 26, 255)
	    msg.AddMsgEvent(MsgID1, 6, Quest_ZhuE_Pos[Index].MapCrc)
	    msg.AddMsgEvent(MsgID1, 9, Quest_ZhuE_Pos[Index].x)
	    msg.AddMsgEvent(MsgID1, 9, Quest_ZhuE_Pos[Index].z)
	    msg.DispatchRoleMsgEvent(RoleID, MsgID1)
	    return 36, false
    else
        return 0, false
	end
end


--使用诛恶密信
function I3200004_UseItem(MapID, InstanceID, TypeID, RoleID)
	--到达指定坐标点,使用物品
	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 4500311, x+3, y, z+3)
	map.MapCreateCreature(MapID, InstanceID, 4500312, x+3, y, z-3)
	map.MapCreateCreature(MapID, InstanceID, 4500313, x+3, y, z)
end



aux.RegisterQuestEvent(20080, 1, "q20080_OnComplete")
aux.RegisterQuestEvent(20080, 7, "q20080_OnQuestInit")
aux.RegisterQuestEvent(20080, 2, "q20080_OnCancel")
aux.RegisterQuestEvent(20080, 4, "q20080_OnCheckAccept")

aux.RegisterItemEvent(3200004, 0, "I3200004_CanUseItem")
aux.RegisterItemEvent(3200004, 1, "I3200004_UseItem")


