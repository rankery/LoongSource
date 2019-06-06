--[[city.ChangeCityDefence(MapID,InstanceID,ChangeVal)         ChangeVal可正可负

city.GetCityAppointedAttByMapID(MapID,InstanceID,Type)


Type 为下面的值
	ECAT_CityID = 0,			// 城市ID
	ECAT_ClanType,	1		// 氏族类型
	ECAT_Holder,	2		// 统治帮派ID
	ECAT_Defence,	3		// 防御
	ECAT_EudemonTally,	4	// 守护神契合度
	ECAT_TaxRate,	5		// 税率
	ECAT_TaxRateTime,	6	// 税率更新时间
	ECAT_Taxation,			// 税金
	ECAT_Prolificacy,		// 生产力
	ECAT_FundOutput,		// 资金产出
	ECAT_MaterialOutput,	// 资材产出
	ECAT_ItemOutput,		// 物品产出

]]



function q30006_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	--判断玩家是否已加入帮派
	local GuildID = guild.GetRoleGuildID(RoleID)
	if GuildID==4294967295 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 143)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断当前城市的占领帮派ID 与玩家所在帮派ID是否相同
	local curguild = city.GetCityAppointedAttByMapID(MapID,InstanceID,2)
	if curguild ~= GuildID then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1027)--“您的帮派并没有占领这座城市”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断玩家帮派职位
	local GuildPos=guild.GetRoleGuildPos(GuildID, RoleID)
	if (GuildPos~=2) and (GuildPos~=3) and (GuildPos~=12) and (GuildPos~=13) and (GuildPos~=14) then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 145)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断本日任务接取次数是否已满
	--得到当前任务总任务环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["fangwei"])
	local UpdateTime = role.GetRoleScriptData(RoleID, 1, RoleDataType["fangwei_Update"])

	--总环数为0时，设置总环数为第一环
	if 0 == LoopNum then
		LoopNum = 1
		role.SetRoleScriptData(RoleID, 1, RoleDataType["fangwei"], LoopNum)
	end

	--检测任务上次更新日期，若与当前日期不一致，则重置总环数及更新日期
	local CurTime = tonumber(os.date("%j"))
	if CurTime~=UpdateTime then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["fangwei"], 1)
		role.SetRoleScriptData(RoleID, 1, RoleDataType["fangwei_Update"], CurTime)
	end

	--若本日已接取50次，则无法再接取
	if LoopNum > 50 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 147)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

--完成任务
function q30006_OnComplete(MapID, InstanceID, QuestID, RoleID, NPCID)


	--城市防卫值加10
	city.ChangeCityDefence(MapID,InstanceID,10)

	local exp_award = {}
	exp_award[3] = 470
	exp_award[4] = 630
	exp_award[5] = 890
	exp_award[6] = 1170
	exp_award[7] = 1590
	exp_award[8] = 2040
	exp_award[9] = 2580
	exp_award[10] = 2790

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	local k = math.floor(rolelevel/10)
	if k>10 then
		k = 10
	end

	--得到当前任务总环数
	local LoopNum = role.GetRoleScriptData(RoleID, 1, RoleDataType["fangwei"])

	--计算环数
	local LoopNum_Ring = LoopNum % 10
	if LoopNum_Ring == 0 then
		LoopNum_Ring = 10
	end

	--计算轮数
	local LoopNum_Round = (LoopNum - LoopNum_Ring) /10 + 1
	--计算经验奖励
	local Exp = exp_award[k]*(1+(LoopNum_Ring-1)*0.3)*(1 + LoopNum_Round*0.1)
	--奖励玩家经验
	role.AddRoleExp(MapID, InstanceID, RoleID, Exp)

	--整50环奖励贡献300
	if (LoopNum==50) then
		local GuildID = guild.GetRoleGuildID(RoleID)
		guild.ModifyRoleContribute(GuildID, RoleID, 300, 102)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1028)--"本帮成员×××慷慨解囊，今日已X次向禁城统领提供资助提升城市防卫值，获得了X点帮派贡献奖励！"
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 9, LoopNum)
		msg.AddMsgEvent(MsgID, 9, 300)
		msg.DispatchGuildMsgEvent(MsgID,GuildID)

	elseif (LoopNum_Ring==10) then	--整10环奖励贡献100
		local GuildID = guild.GetRoleGuildID(RoleID)
		guild.ModifyRoleContribute(GuildID, RoleID, 100, 102)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 1028)--"本帮成员×××慷慨解囊，今日已X次向禁城统领提供资助提升城市防卫值，获得了X点帮派贡献奖励！"
		msg.AddMsgEvent(MsgID, 2, RoleID)
		msg.AddMsgEvent(MsgID, 9, LoopNum)
		msg.AddMsgEvent(MsgID, 9, 100)
		msg.DispatchGuildMsgEvent(MsgID,GuildID)

	end
	--总任务环数加一
	role.SetRoleScriptData(RoleID, 1, RoleDataType["fangwei"], LoopNum+1)

end


function q30006_OnCheckComplete(MapID, InstanceID, QuestID, RoleID, NPCID)

	--判断玩家是否已加入帮派
	local GuildID = guild.GetRoleGuildID(RoleID)
	if GuildID==4294967295 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 143)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end

	--判断当前城市的占领帮派ID 与玩家所在帮派ID是否相同
	local curguild = city.GetCityAppointedAttByMapID(MapID,InstanceID,2)
	if curguild ~= GuildID then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1027)--“您的帮派并没有占领这座城市”
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(30006, 4, "q30006_OnCheckAccept")
aux.RegisterQuestEvent(30006, 1, "q30006_OnComplete")
aux.RegisterQuestEvent(30006, 5, "q30006_OnCheckComplete")

