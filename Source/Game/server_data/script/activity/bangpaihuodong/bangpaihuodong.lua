LingPaiLingQu_Update = {}
--帮派活动
fengxiangjiaofei_man = {}
fengxiangjiaofei_kill = {}
fabu_date = 0
bangpaihuwei_instance = {}
function chengwufaburen(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
    local curhour = tonumber(os.date("%H"))
	if TalkIndex == -1 then
	    local Role_GuildID = guild.GetRoleGuildID(RoleID)
		local Master_GuilID = city.GetCityAppointedAtt(3, 2)
		local Role_ZhiWei = -1
		if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
		    Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, RoleID)
		end
	    if city.GetCityAppointedAtt(3, 2) == 4294967295 or city.GetCityAppointedAtt(3, 2) == nil then
			if Role_ZhiWei ~= 2 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3401)		--“城务发布人：\n    新的城主尚未诞生，此时不会有任何事务发布。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3407)		--“城务发布人：\n    城主已经下达了清剿凤翔城内宵小的命令。作为一帮之主，您每天可以在我这里领取一个清剿匪类令，在凤翔场景的凤翔府中使用该令牌可以为帮派成员开辟一个练功副本。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				if curhour >= 12 then
					msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	3406)			--“领取清剿匪类令”
				end
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif Role_GuildID==4294967295 or Role_ZhiWei ~= 2 then
		    if act.GetActScriptData(99, 1, 16) > 0 and role.GetRoleScriptData(RoleID, 1, RoleDataType["ChengZhuJingYan_Update"]) ~= tonumber(os.date("%j")) and curhour >= 18 and curhour <= 20 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3402)		--“城务发布人：\n    凤翔城主决定施恩于天下，您可以从我这里领取一定量的经验。您只需要向城主谢恩便能获得奖励。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	3403)			--“多谢城主恩典”
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3404)		--“城务发布人：\n    很抱歉，现在没有与您相关的事务发布。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif Role_GuildID ~= -1 and Role_GuildID ~= 4294967295 and Role_GuildID ~= Master_GuilID and Role_ZhiWei == 2 then
		    if act.GetActScriptData(99, 1, 16) > 0 and role.GetRoleScriptData(RoleID, 1, RoleDataType["ChengZhuJingYan_Update"]) ~= tonumber(os.date("%j")) and curhour >= 18 and curhour <= 20 then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3405)		--“城务发布人：\n    凤翔城主决定施恩于天下，您可以从我这里领取一定量的经验。您只需要向城主谢恩便能获得奖励。\n    另外，城主已经下达了清剿凤翔城内宵小的命令。作为一帮之主，您每天可以在我这里领取一个清剿匪类令，在凤翔府中使用该令牌可以为帮派成员开辟一个练功副本。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				--if role.GetRoleScriptData(RoleID, 1, RoleDataType["LingPaiLingQu_Update"]) ~= tonumber(os.date("%j")) then
				if curhour >= 12 then
					msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	3406)			--“领取清剿匪类令”
				end
				--end
				msg.AddMsgEvent(MsgID, 21,	4)			--确定按钮
				msg.AddMsgEvent(MsgID, 1,	3403)			--“多谢城主恩典”
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
                local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3407)		--“城务发布人：\n    城主已经下达了清剿凤翔城内宵小的命令。作为一帮之主，您每天可以在我这里领取一个清剿匪类令，在凤翔场景的凤翔府中使用该令牌可以为帮派成员开辟一个练功副本。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				if curhour >= 12 then
					msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	3406)			--“领取清剿匪类令”
				end
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif Role_GuildID ~= -1 and Role_GuildID ~= 4294967295 and Role_GuildID == Master_GuilID and Role_ZhiWei == 2 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	3408)		--“城务发布人：\n    城主您好，您想发布一些什么事务呢？（清剿匪类令每天只能领取一次，领取之后可以在凤翔场景中的凤翔府范围内使用为帮派成员开辟一个练功副本。）”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			if curhour >= 12 then
				msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	3406)			--“领取清剿匪类令”
			end
			msg.AddMsgEvent(MsgID, 21,	6)			--确定按钮
			msg.AddMsgEvent(MsgID, 1,	3409)			--“恩泽天下”
			msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		end

	elseif TalkIndex == 4 then
        if act.GetActScriptData(99, 1, 16) > 0 then
		    if role.GetRoleScriptData(RoleID, 1, RoleDataType["ChengZhuJingYan_Update"]) ~= tonumber(os.date("%j")) then
			    local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
                role.AddRoleExp(MapID, InstanceID, RoleID, math.floor(activity_newexp[rolelevel]/6))
				role.SetRoleScriptData(RoleID, 1, RoleDataType["ChengZhuJingYan_Update"], tonumber(os.date("%j")))
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3410)		--“城务发布人：\n    您已经领取今日的城主赏赐经验。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 5 then
	    local Role_GuildID = guild.GetRoleGuildID(RoleID)
		local Master_GuilID = city.GetCityAppointedAtt(3, 2)
		local Role_ZhiWei = -1
		if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
		    Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, RoleID)
		end
		if Role_GuildID ~= -1 and Role_GuildID ~= 4294967295 and Role_ZhiWei == 2 then

			if LingPaiLingQu_Update[Role_GuildID]==nil then
				LingPaiLingQu_Update[Role_GuildID]=0
			end

			if LingPaiLingQu_Update[Role_GuildID] ~= tonumber(os.date("%j")) then
			    if role.GetBagFreeSize(RoleID) > 0 and curhour >= 12 then
				    role.AddRoleItem(MapID, InstanceID, RoleID, 3302301, 1, -1, 8, 420)
					LingPaiLingQu_Update[Role_GuildID] = tonumber(os.date("%j"))
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	3411)		--“城务发布人：\n    您可以在凤翔的凤翔府范围内使用该令牌召唤出凤翔府护卫。通过该护卫您的帮派成员可以进入帮派专用的清剿匪徒副本。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				elseif curhour >= 12 then
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	3412)		--“城务发布人：\n    您的背包空间不足，请清理一下再来领取。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3413)		--“城务发布人：\n    您每天只能领取一枚清剿匪类令，请您明天再来领取。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	elseif TalkIndex == 6 then
	    local Role_GuildID = guild.GetRoleGuildID(RoleID)
		local Master_GuilID = city.GetCityAppointedAtt(3, 2)
		local Role_ZhiWei = -1
		if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
		    Role_ZhiWei = guild.GetRoleGuildPos(Role_GuildID, RoleID)
		end
        if Role_GuildID ~= -1 and Role_GuildID ~= 4294967295 and Role_GuildID == Master_GuilID and Role_ZhiWei == 2 then
            if fabu_date ~= tonumber(os.date("%j")) then
				fabu_date = tonumber(os.date("%j"))
				act.SetActScriptData(99, 1, 16, 3)
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3414)		--“城务发布人：\n    您已经成功发布城主奖励。之后的三天里所有玩家都可以在此处领取城主经验奖励。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 102, 100128)    --凤翔城主决定广施恩惠，之后三天晚上18:00-21:00所有玩家可以前往城战场景在城务发布人处领取经验奖励！
				msg.DispatchBroadcast(MsgID,-1,-1,-1)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3415)		--“城务发布人：\n    该事务每天只能发布一次，您今天已经发布过该事务了。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
            end
		end
	end

end

aux.RegisterCreatureEvent(4900706, 7, "chengwufaburen")

function I3302301_GiftBag(MapID, InstanceID, TypeID, TargetID)
	--玩家ID， 生成物品TypeID，数量，品质，生成模式，log用网络消息枚举
    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local ID = map.MapCreateColCreature(3017298383, -1, 4900707, x, y ,z)
	local Role_GuildID = guild.GetRoleGuildID(TargetID)
	cre.SetCreatureScriptData(MapID, InstanceID, ID, 1, 0, Role_GuildID)
	bangpaihuwei_instance[ID] = Role_GuildID
	map.CreateInstance(2092185908, Role_GuildID)
	fengxiangjiaofei_man[Role_GuildID] = 0
    fengxiangjiaofei_kill[Role_GuildID]  = 0
	--map.MapCreateColCreature(2092185908, Role_GuildID, 4900708, 1, 1 ,1)
    local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3416) --帮主在凤翔【xxx，xxx】召唤了凤翔府护卫，请大家前去参与帮派活动领取经验！
	msg.AddMsgEvent(MsgID, 9, x)
	msg.AddMsgEvent(MsgID, 9, z)
	msg.DispatchGuildMsgEvent(MsgID, Role_GuildID)
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 3416)  --帮主在凤翔【xxx，xxx】召唤了凤翔府护卫，请大家前去参与帮派活动领取经验！
	msg.AddMsgEvent(MsgID, 9, x)
	msg.AddMsgEvent(MsgID, 9, z)
	msg.DispatchGuildMsgEvent(MsgID, Role_GuildID)
end

function I3302301_CanUseGiftBag(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	if MapID ~= 3017298383 or x > 1150 or x < 629 or z > 2400 or z < 1855 then
	    bRet = 32
	end
	if guild.GetRoleGuildID(TargetID) == nil or guild.GetRoleGuildID(TargetID) == 4294967295 or guild.GetRoleGuildPos(guild.GetRoleGuildID(TargetID), TargetID) ~= 2 then
	    bRet = 32
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(3302301, 1, "I3302301_GiftBag")
aux.RegisterItemEvent(3302301, 0, "I3302301_CanUseGiftBag")

function a09_PlayerEnter(MapID, InstanceID, RoleID)
	fengxiangjiaofei_man[InstanceID] = fengxiangjiaofei_man[InstanceID] + 1
    --fengxiangjiaofei_kill[Role_GuildID]  = 0
    --map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)+1)
end

--注册进入活动地图事件
aux.RegisterMapEvent("a09", 2, "a09_PlayerEnter")

--玩家离开活动场景
function a09_PlayerLeave(MapID, InstanceID, RoleID)
    if fengxiangjiaofei_man[InstanceID] > 0 then
	   fengxiangjiaofei_man[InstanceID] = fengxiangjiaofei_man[InstanceID] - 1
	   -- map.SetMapScriptData(MapID, InstanceID, 1, 0, map.GetMapScriptData(MapID, InstanceID, 1, 0)-1)
	end
end

--注册离开活动地图事件
aux.RegisterMapEvent("a09", 3, "a09_PlayerLeave")

--找到最合适的副本实例
function a09_GetOnePerfectMap(MapID, InstanceID, RoleID)
	local Role_GuildID = guild.GetRoleGuildID(RoleID)
	if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 then
	    return Role_GuildID
	else
	    return 0
	end
end

aux.RegisterMapEvent("a09",  14, "a09_GetOnePerfectMap")

--怪物死亡
function fengxiangfeilei_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    --map.SetMapScriptData(MapID, InstanceID, 1, 1, map.GetMapScriptData(MapID, InstanceID, 1, 1)+1)
	fengxiangjiaofei_kill[InstanceID]  = fengxiangjiaofei_kill[InstanceID] + 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 3417)  --本帮派已经击杀凤翔匪类 XXX/1500
	msg.AddMsgEvent(MsgID, 9, fengxiangjiaofei_kill[InstanceID])
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end
aux.RegisterCreatureEvent(1534160, 4, "fengxiangfeilei_OnDie")
aux.RegisterCreatureEvent(1534161, 4, "fengxiangfeilei_OnDie")

function N4900707_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)
	local guildID1 = cre.GetCreatureScriptData(MapID, InstanceID, TargetID, 1, 0)
	local Role_GuildID = guild.GetRoleGuildID(RoleID)
	if MapID == 3017298383 then
		if TalkIndex == -1 then
			if guildID1 == Role_GuildID then
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3418)		--“凤翔府护卫：\n    只有xxx帮派的人才能通过本人进入匪类据点。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 19,  guildID1)
				msg.AddMsgEvent(MsgID, 21,	4)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	3419)			--“进入匪徒据点”
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	22)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3420)		--“凤翔府护卫：\n    只有xxx帮派的人才能通过本人进入匪类据点。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 19,  guildID1)
				msg.AddMsgEvent(MsgID, 21,	15)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif TalkIndex == 4 then
			if guildID1 == Role_GuildID and fengxiangjiaofei_man[guildID1] < 50 then
			    role.RoleGotoNewMap(MapID, InstanceID, RoleID, 2092185908, ZhongZhiChengCheng_Point[1].x, ZhongZhiChengCheng_Point[1].y, ZhongZhiChengCheng_Point[1].z)
				--role.RoleGotoArena(MapID, InstanceID, RoleID, 2092185908, Role_GuildID, ZhongZhiChengCheng_Point[1].x, ZhongZhiChengCheng_Point[1].y, ZhongZhiChengCheng_Point[1].z)
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3421)		--“凤翔府护卫：\n    该副本最多可以容纳50名玩家同时进入，请等待其他玩家推出后再尝试进入副本。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“放弃”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		end
	else
	    if TalkIndex == -1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 20,	3422)		--“凤翔府护卫：\n    只要杀掉1500个匪类和宵小，您帮派中的所有人都能在我这里领取大量的经验。\n    已经击杀XXX/1500。”
			msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
			msg.AddMsgEvent(MsgID, 9,   fengxiangjiaofei_kill[InstanceID])
			if  fengxiangjiaofei_kill[InstanceID] >= 1500 then
			    msg.AddMsgEvent(MsgID, 21,	5)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	3423)			--“领取经验和气海丹”
			end
			msg.AddMsgEvent(MsgID, 21,	6)			--取消按钮
			msg.AddMsgEvent(MsgID, 1,	3424)			--“离开匪类据点”
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
		elseif TalkIndex == 5 then
            if Role_GuildID ~= nil and Role_GuildID ~= 4294967295 and math.floor(role.GetRoleScriptData(RoleID, 1, RoleDataType["FengXiangXiaoXiao_Update"])/100) ~= tonumber(os.date("%j")) or role.GetRoleScriptData(RoleID, 1, RoleDataType["FengXiangXiaoXiao_Update"])%100 < tonumber(os.date("%H")) or role.GetRoleScriptData(RoleID, 1, RoleDataType["FengXiangXiaoXiao_Update"])%100 - tonumber(os.date("%H")) >= 2 then
				if role.GetBagFreeSize(RoleID) < 1 then
				    local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	3451)		--“凤翔府护卫：\n    请您确保行囊中有一个以上的空格再来领取。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				else
                    if guild.GetGuildAtt(Role_GuildID) <= 2 then
					    role.AddRoleItem(MapID, InstanceID, RoleID, 1360133, 3, -1, 8, 420)
					else
					    role.AddRoleItem(MapID, InstanceID, RoleID, 1360133, 6, -1, 8, 420)
					end
					local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
					role.AddRoleExp(MapID, InstanceID, RoleID, math.floor(activity_newexp[rolelevel]/4*3))
					role.SetRoleScriptData(RoleID, 1, RoleDataType["FengXiangXiaoXiao_Update"], tonumber(os.date("%j"))*100+tonumber(os.date("%H")))
					local MsgID = msg.BeginMsgEvent()
					msg.AddMsgEvent(MsgID, 20,	3425)		--“凤翔府护卫：\n    只要杀掉1500个匪类和宵小，您帮派中的所有人都能在我这里领取大量的经验。\n    已经击杀XXX/1500。”
					msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
					msg.AddMsgEvent(MsgID, 9,   fengxiangjiaofei_kill[InstanceID])
					msg.AddMsgEvent(MsgID, 21,	6)			--取消按钮
					msg.AddMsgEvent(MsgID, 1,	3426)			--“离开匪类据点”
					msg.DispatchRoleMsgEvent(RoleID, MsgID)
				end
			else
			    local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 20,	3427)		--“凤翔府护卫：\n    您今天已经领取过剿灭宵小的活动奖励了，请您下次再来领取。”
				msg.AddMsgEvent(MsgID, 24,	TargetID)	--npcid
				msg.AddMsgEvent(MsgID, 21,	-1)			--取消按钮
				msg.AddMsgEvent(MsgID, 1,	21)			--“确定”
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
			end
		elseif TalkIndex == 6 then
			role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3017298383, 923, 10000, 2007)
		end
	end
end

aux.RegisterCreatureEvent(4900707, 7, "N4900707_OnTalk")

function c4900707_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID)
   -- if MapID == 3017298383 then
        if bangpaihuwei_instance[TargetID] ~= nil then
		   map.DeleteInstance(2092185908, bangpaihuwei_instance[TargetID])
		   fengxiangjiaofei_kill[InstanceID] = nil
		   fengxiangjiaofei_man[InstanceID] = nil
		   bangpaihuwei_instance[TargetID] = nil
		end
	--end
end

aux.RegisterCreatureEvent(4900707, 13, "c4900707_OnDisappear")

--玩家在活动中掉线后上线
function a09_CanTakeOverWhenOnline(MapID, RoleID)
	--修正玩家的上线地图和坐标todo
	return 3017298383, 923, 10000, 2007
end

aux.RegisterMapEvent("a09",  12, "a09_CanTakeOverWhenOnline")

-------------------------------------------------------------------------------------------------------
--找到地图的出口和坐标
function a09_GetExporMapAndCoord(MapID, InstanceID, RoleID)
	return 1, 3017298383, 923, 10000, 2007
end

aux.RegisterMapEvent("a09",  13, "a09_GetExporMapAndCoord")

--检测玩家是否能够进入活动场景
function a09_CanEnter(MapID, InstanceID, RoleID)
    local Role_GuildID = guild.GetRoleGuildID(RoleID)
	if Role_GuildID ~= 4294967295 then
		return 1
	else
	    return 0
	end
end

aux.RegisterMapEvent("a09",  15, "a09_CanEnter")
