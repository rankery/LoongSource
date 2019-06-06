--[[BiWuQieCuo_Available = 0

--如果比武切磋场景被开启，则将BiWuQieCuo_Available设为2.
function BiWuQieCuo_OnInit(MapID,InstanceID,CreatureID,CreatureTypeID)
	BiWuQieCuo_Available = 1 + BiWuQieCuo_Available
end
aux.RegisterMapEvent("a04", 0, "BiWuQieCuo_OnInit")
aux.RegisterMapEvent("a05", 0, "BiWuQieCuo_OnInit")]]

--加载m04时，若全局变量Open_Arena = 1，则刷新NPC竞技场管事
function BiWuQieCuo_OnInit(MapID,InstanceID,CreatureID,CreatureTypeID)
	if Open_Arena == 1 then
		map.MapCreateColCreature(MapID, InstanceID, 4006512, 331, 2150, 307, 1, "")
	end
end
aux.RegisterMapEvent("a04", 0, "BiWuQieCuo_OnInit")
aux.RegisterMapEvent("a05", 0, "BiWuQieCuo_OnInit")

--连杀数计算脚本
function JinRuYanWuChang_KillPeople(MapID, InstanceID, RoleID, KillerID)
    local level1 = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local level2 = role.GetRoleLevel(MapID, InstanceID, KillerID)
	local a = level2 - level1
	if unit.IsPlayer(RoleID) and unit.IsPlayer(KillerID) then	  --判断双方都是人
	    if a <= 20 then
	        local Killer_Num = role.GetRoleScriptData(KillerID, 1, RoleDataType["BiWuQieCuo_Kill"])
		    Killer_Num = Killer_Num + 1
		    role.SetRoleScriptData(KillerID, 1, RoleDataType["BiWuQieCuo_Kill"], Killer_Num)

		    if Killer_Num == 100 then
		        role.SigTitleEvent(KillerID, 186)    --给予嗜血战神称号
		        local MsgID = msg.BeginMsgEvent()
	            msg.AddMsgEvent(MsgID, 13, 1)
			    msg.AddMsgEvent(MsgID, 1, 2010)       --XXX（杀人者姓名）在演武场中取得100连胜，获得嗜血战神称号！
	            msg.AddMsgEvent(MsgID, 2, KillerID)
	            msg.DispatchWorldMsgEvent(MsgID)
			elseif Killer_Num == 50 then
				local MsgID = msg.BeginMsgEvent()
	            msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 2011)       --XXX（杀人者姓名）在演武场中打败了XXX（被杀者姓名），取得50连胜！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchWorldMsgEvent(MsgID)
     		elseif Killer_Num >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 2012)       --XXX（杀人者姓名）在演武场中战胜了XXX（被杀者姓名），取得XX(连胜数)连胜！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, Killer_Num)
				msg.DispatchMapMsgEvent(MsgID, MapID, -1)
			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 2013)       --XXX（杀人者姓名）在演武场中战胜了XXX（被杀者姓名）。
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.DispatchMapMsgEvent(MsgID, MapID, -1)
			end

			local DeadPeople_Num = role.GetRoleScriptData(RoleID, 1, RoleDataType["BiWuQieCuo_Kill"])
			if DeadPeople_Num >= 100 then
				role.SigTitleEvent(KillerID, 187)    --给予战神终结者称号
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 2014)       --XXX（杀人者姓名）终结了嗜血战神——XXX（被杀者姓名）的XX(被杀者连胜数)连胜，获得战神终结者称号！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, DeadPeople_Num)
				msg.DispatchWorldMsgEvent(MsgID)
			elseif DeadPeople_Num >= 50 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 2015)       --XXX（杀人者姓名）技高一筹，终结了XXX（被杀者姓名）的XX(被杀者连胜数)连胜！。
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, DeadPeople_Num)
				msg.DispatchWorldMsgEvent(MsgID)
			elseif DeadPeople_Num >= 10 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 1)
				msg.AddMsgEvent(MsgID, 1, 2016)       --XXX（杀人者姓名）终结了XXX（被杀者姓名）的XX(被杀者连胜数)连胜！
				msg.AddMsgEvent(MsgID, 2, KillerID)
				msg.AddMsgEvent(MsgID, 2, RoleID)
				msg.AddMsgEvent(MsgID, 9, DeadPeople_Num)
				msg.DispatchMapMsgEvent(MsgID, MapID, -1)
			end
		else
		    local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, 2017)       -- "击杀等级与自己相差太多的玩家不会增加连杀数。"
			msg.DispatchRoleMsgEvent(KillerID, MsgID)     --发送消息
		end
	end
    role.SetRoleScriptData(RoleID, 1, RoleDataType["BiWuQieCuo_Kill"], 0)
end


aux.RegisterMapEvent("a04", 5, "JinRuYanWuChang_KillPeople")
aux.RegisterMapEvent("a05", 5, "JinRuYanWuChang_KillPeople")



--进入演武场广播
function JinRuYanWuChang(MapID, InstanceID, RoleID)
	local kill = role.GetRoleScriptData(RoleID, 1, RoleDataType["BiWuQieCuo_Kill"])
	if kill >= 10 then
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 13, 0)
	    msg.AddMsgEvent(MsgID, 1, 2018)       --XXX进入了演武场，他已经在演武场中获得了XX场胜利。
	    msg.AddMsgEvent(MsgID, 2, RoleID)
	    msg.AddMsgEvent(MsgID, 9, kill)
	    msg.DispatchMapMsgEvent(MsgID, MapID, -1)
	else
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 13, 0)
	    msg.AddMsgEvent(MsgID, 1, 2019)       --XXX进入了演武场。
	    msg.AddMsgEvent(MsgID, 2, RoleID)
	    msg.DispatchMapMsgEvent(MsgID, MapID, -1)
	end
end


aux.RegisterMapEvent("a04", 2, "JinRuYanWuChang")
aux.RegisterMapEvent("a05", 2, "JinRuYanWuChang")


--与演武场传送石3021023对话，传送到安全区
function n3021023_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 2020) --    您想回到演武场观战台吗？
		msg.AddMsgEvent(MsgID, 24, TargetID) --npcid
		msg.AddMsgEvent(MsgID, 21, 4) -- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 21) -- "确定"
		msg.AddMsgEvent(MsgID, 21, 6)   --取消按钮
		msg.AddMsgEvent(MsgID, 1, 22)   --“取消”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
    elseif TalkIndex == 4 then
        role.RoleGotoNewMap(MapID, InstanceID, RoleID, MapID, 332, 43, 319)--传送到安全区
    end
end

aux.RegisterCreatureEvent(3021023, 7, "n3021023_OnTalk")


--玩家复活脚本
function BiWuQieCuo_RoloRevive(MapID, InstanceID, RoleID, Type, ReviveHP, ReviveMP, x, y, z, RebornMapID)

    ReviveHP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 8)
	ReviveMP = role.GetRoleAttValue(MapID, InstanceID, RoleID, 10)
	return ReviveHP, ReviveMP, 332, 2155, 319, MapID
end

aux.RegisterMapEvent("a04", 11, "BiWuQieCuo_RoloRevive")
aux.RegisterMapEvent("a05", 11, "BiWuQieCuo_RoloRevive")
