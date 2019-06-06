
function Arena_Shop_CanBuy(MapID, InstanceID, ShopID, RoleID, ItemID)
    local Goods_Arena = {}
	Goods_Arena[3300102]={Rank=4,Winning=30}
	Goods_Arena[8220113]={Rank=4,Winning=50}
	Goods_Arena[8220114]={Rank=4,Winning=50}
	Goods_Arena[8220115]={Rank=4,Winning=50}
	Goods_Arena[8220116]={Rank=4,Winning=50}
	Goods_Arena[8220121]={Rank=4,Winning=50}
	Goods_Arena[8220122]={Rank=4,Winning=50}
	Goods_Arena[8220123]={Rank=4,Winning=50}
	Goods_Arena[8220124]={Rank=4,Winning=50}
	Goods_Arena[8220105]={Rank=5,Winning=50}
	Goods_Arena[8220106]={Rank=5,Winning=50}
	Goods_Arena[8220107]={Rank=5,Winning=50}
	Goods_Arena[8220108]={Rank=5,Winning=50}
	Goods_Arena[8220097]={Rank=5,Winning=50}
	Goods_Arena[8220098]={Rank=5,Winning=50}
	Goods_Arena[8220099]={Rank=5,Winning=50}
	Goods_Arena[8220100]={Rank=5,Winning=50}
	Goods_Arena[8220089]={Rank=6,Winning=50}
	Goods_Arena[8220090]={Rank=6,Winning=50}
	Goods_Arena[8220091]={Rank=6,Winning=50}
	Goods_Arena[8220092]={Rank=6,Winning=50}
	Goods_Arena[8220081]={Rank=6,Winning=50}
	Goods_Arena[8220082]={Rank=6,Winning=50}
	Goods_Arena[8220083]={Rank=6,Winning=50}
	Goods_Arena[8220084]={Rank=6,Winning=50}
	Goods_Arena[8220073]={Rank=7,Winning=50}
	Goods_Arena[8220074]={Rank=7,Winning=50}
	Goods_Arena[8220075]={Rank=7,Winning=50}
	Goods_Arena[8220076]={Rank=7,Winning=50}
	Goods_Arena[8220065]={Rank=7,Winning=50}
	Goods_Arena[8220066]={Rank=7,Winning=50}
	Goods_Arena[8220067]={Rank=7,Winning=50}
	Goods_Arena[8220068]={Rank=7,Winning=50}
	Goods_Arena[8220057]={Rank=7,Winning=50}
	Goods_Arena[8220058]={Rank=7,Winning=50}
	Goods_Arena[8220059]={Rank=7,Winning=50}
	Goods_Arena[8220060]={Rank=7,Winning=50}
	Goods_Arena[8220049]={Rank=7,Winning=50}
	Goods_Arena[8220050]={Rank=7,Winning=50}
	Goods_Arena[8220051]={Rank=7,Winning=50}
	Goods_Arena[8220052]={Rank=7,Winning=50}
	Goods_Arena[8220041]={Rank=7,Winning=55}
	Goods_Arena[8220042]={Rank=7,Winning=55}
	Goods_Arena[8220043]={Rank=7,Winning=55}
	Goods_Arena[8220044]={Rank=7,Winning=55}
	Goods_Arena[8220033]={Rank=7,Winning=55}
	Goods_Arena[8220034]={Rank=7,Winning=55}
	Goods_Arena[8220035]={Rank=7,Winning=55}
	Goods_Arena[8220036]={Rank=7,Winning=55}
	Goods_Arena[8220025]={Rank=8,Winning=60}
	Goods_Arena[8220026]={Rank=8,Winning=60}
	Goods_Arena[8220027]={Rank=8,Winning=60}
	Goods_Arena[8220028]={Rank=8,Winning=60}
	Goods_Arena[8220017]={Rank=8,Winning=60}
	Goods_Arena[8220018]={Rank=8,Winning=60}
	Goods_Arena[8220019]={Rank=8,Winning=60}
	Goods_Arena[8220020]={Rank=8,Winning=60}
	Goods_Arena[8220009]={Rank=8,Winning=60}
	Goods_Arena[8220010]={Rank=8,Winning=60}
	Goods_Arena[8220011]={Rank=8,Winning=60}
	Goods_Arena[8220012]={Rank=8,Winning=60}
	Goods_Arena[8220001]={Rank=8,Winning=60}
	Goods_Arena[8220002]={Rank=8,Winning=60}
	Goods_Arena[8220003]={Rank=8,Winning=60}
	Goods_Arena[8220004]={Rank=8,Winning=60}
	Goods_Arena[1423001]={Rank=1,Winning=30}
    Goods_Arena[1423002]={Rank=4,Winning=50}
    Goods_Arena[1423003]={Rank=7,Winning=80}
    Goods_Arena[1423004]={Rank=1,Winning=30}
    Goods_Arena[1423005]={Rank=4,Winning=50}
    Goods_Arena[1423006]={Rank=7,Winning=80}
	Goods_Arena[1423007]={Rank=1,Winning=30}
	Goods_Arena[1423008]={Rank=4,Winning=50}
    Goods_Arena[1423009]={Rank=7,Winning=80}
    Goods_Arena[1423010]={Rank=1,Winning=30}
    Goods_Arena[1423011]={Rank=4,Winning=50}
    Goods_Arena[1423012]={Rank=7,Winning=80}
    Goods_Arena[1423013]={Rank=1,Winning=30}
    Goods_Arena[1423014]={Rank=4,Winning=50}
    Goods_Arena[1423015]={Rank=7,Winning=80}
    Goods_Arena[1423016]={Rank=1,Winning=30}
    Goods_Arena[1423017]={Rank=4,Winning=50}
    Goods_Arena[1423018]={Rank=7,Winning=80}
	Goods_Arena[1410006]={Rank=3,Winning=30}
	Goods_Arena[1410007]={Rank=4,Winning=50}
	Goods_Arena[1410008]={Rank=5,Winning=80}
	Goods_Arena[1410018]={Rank=3,Winning=30}
	Goods_Arena[1410019]={Rank=4,Winning=50}
	Goods_Arena[1410020]={Rank=5,Winning=80}
	Goods_Arena[1410030]={Rank=3,Winning=30}
	Goods_Arena[1410031]={Rank=4,Winning=50}
	Goods_Arena[1410032]={Rank=5,Winning=80}
	Goods_Arena[1410042]={Rank=3,Winning=30}
	Goods_Arena[1410043]={Rank=4,Winning=50}
	Goods_Arena[1410044]={Rank=5,Winning=80}
	Goods_Arena[1410054]={Rank=3,Winning=30}
	Goods_Arena[1410055]={Rank=4,Winning=50}
	Goods_Arena[1410056]={Rank=5,Winning=80}
	Goods_Arena[1410066]={Rank=3,Winning=30}
	Goods_Arena[1410067]={Rank=4,Winning=50}
	Goods_Arena[1410068]={Rank=5,Winning=80}
	Goods_Arena[1410074]={Rank=3,Winning=30}
	Goods_Arena[1410075]={Rank=4,Winning=50}
	Goods_Arena[1410076]={Rank=5,Winning=80}
	Goods_Arena[1410078]={Rank=3,Winning=30}
	Goods_Arena[1410079]={Rank=4,Winning=50}
	Goods_Arena[1410080]={Rank=5,Winning=80}
    Goods_Arena[1432003]={Rank=6,Winning=30}
	Goods_Arena[1432004]={Rank=7,Winning=50}
	Goods_Arena[1432005]={Rank=8,Winning=80}
	Goods_Arena[1432008]={Rank=6,Winning=30}
	Goods_Arena[1432009]={Rank=7,Winning=50}
	Goods_Arena[1432010]={Rank=8,Winning=80}
	Goods_Arena[1432013]={Rank=6,Winning=30}
	Goods_Arena[1432014]={Rank=7,Winning=50}
	Goods_Arena[1432015]={Rank=8,Winning=80}
	Goods_Arena[1432018]={Rank=6,Winning=30}
	Goods_Arena[1432019]={Rank=7,Winning=50}
	Goods_Arena[1432020]={Rank=8,Winning=80}
	Goods_Arena[1432023]={Rank=6,Winning=30}
	Goods_Arena[1432024]={Rank=7,Winning=50}
	Goods_Arena[1432025]={Rank=8,Winning=80}
	Goods_Arena[8907001]={Rank=10,Winning=80}
	Goods_Arena[8907002]={Rank=10,Winning=80}
	Goods_Arena[8907003]={Rank=10,Winning=80}
	Goods_Arena[8907004]={Rank=10,Winning=80}
	Goods_Arena[8907005]={Rank=10,Winning=80}
	Goods_Arena[8907006]={Rank=10,Winning=80}
	Goods_Arena[8907007]={Rank=10,Winning=80}
	Goods_Arena[8907008]={Rank=10,Winning=80}

	if ItemID <= 8972020 and ItemID >= 8972001 then
		local CurTime = tonumber(os.date("%j"))
		local OnlyOnce = role.GetRoleScriptData(RoleID, 1, RoleDataType["ShopDate_Arena"])
		if CurTime ~= OnlyOnce then
			role.SetRoleScriptData(RoleID, 1, RoleDataType["ShopDate_Arena"], CurTime)
			role.SetRoleScriptData(RoleID, 1, RoleDataType["BuyEquip_Arena"], 0)
		end
		if role.GetRoleScriptData(RoleID, 1, RoleDataType["BuyEquip_Arena"]) == 1 then
			--提示玩家
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 2534)   --您每天只能购买一件职业装备
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			return 0
		end
	end

	if Goods_Arena[ItemID] ~= nil then

		local Rank = RankPoint_Self(role.GetRoleAttValue(MapID, InstanceID, RoleID, 79))
		local TotleTimes = role.GetRoleScriptData(RoleID, 1, RoleDataType["TotleTimes_Arena"])
		local Winning1
		if TotleTimes >0 then
			Winning1 = role.GetRoleScriptData(RoleID, 1, RoleDataType["WinTimes_Arena"]) / TotleTimes * 100
		else
			Winning1 = 0
		end

		if Goods_Arena[ItemID].Winning > Winning1 then
		    --提示玩家
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 2532)   --您需要达到XX%的胜率才能购买此物品
			msg.AddMsgEvent(MsgID, 9, Goods_Arena[ItemID].Winning)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			return 0
		end

		if TotleTimes < Goods_Arena[ItemID].Rank * 10 then
		    --提示玩家
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 2533)   --您需要完成XX场比赛才能购买此物品
			msg.AddMsgEvent(MsgID, 9, Goods_Arena[ItemID].Rank * 10)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
			return 0
		end

		if ItemID <= 8220124 and ItemID >= 8220001 then
			local CurTime = tonumber(os.date("%j"))
			local OnlyOnce = role.GetRoleScriptData(RoleID, 1, RoleDataType["ShopDate_Arena"])
			if CurTime ~= OnlyOnce then
				role.SetRoleScriptData(RoleID, 1, RoleDataType["ShopDate_Arena"], CurTime)
				role.SetRoleScriptData(RoleID, 1, RoleDataType["BuyEquip_Arena"], 0)
			end
			if role.GetRoleScriptData(RoleID, 1, RoleDataType["BuyEquip_Arena"]) == 1 then
				--提示玩家
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 2534)   --您每天只能购买一件职业装备
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				return 0
			end
		end

		if ItemID <= 8907008 and ItemID >= 8907001 then
		    local curmon = tonumber(os.date("%m"))
			if curmon ~= act.GetActScriptData(99, 1, 40) then
			    act.SetActScriptData(99, 1, 40, curmon)
			    act.SetActScriptData(99, 1, 41, 0)
				act.SaveActScriptData(99)
			end
		    if act.GetActScriptData(99, 1, 41) == 1 then
			    --提示玩家
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 2535)   --每个月只能有一个玩家购买神器
				msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
				return 0
			end
		end
    end
	return 1
end

function Arena_Shop_OnBuy(MapID, InstanceID, ShopID, RoleID, ItemID)
    if (ItemID <= 8220124 and ItemID >= 8220001) or (ItemID <= 8972020 and ItemID >= 8972001) then
		role.SetRoleScriptData(RoleID, 1, RoleDataType["BuyEquip_Arena"], 1)
	end

	if ItemID <= 8907008 and ItemID >= 8907001 then
		act.SetActScriptData(99, 1, 41, 1)
		act.SaveActScriptData(99)
		--提示玩家
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 2536)   --XXX从竞技场神器商处购买了绝世神器XXX!
		msg.AddMsgEvent(MsgID, 2, RoleID)
	    msg.AddMsgEvent(MsgID, 4, ItemID)
		msg.DispatchWorldMsgEvent(MsgID)     --发送消息
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 100, 2536)   --XXX从竞技场神器商处购买了绝世神器XXX!
		msg.AddMsgEvent(MsgID, 2, RoleID)
	    msg.AddMsgEvent(MsgID, 4, ItemID)
		msg.DispatchWorldMsgEvent(MsgID)     --发送消息
	end
end

aux.RegisterCreatureEvent(4006514, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006515, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006516, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006517, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006518, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006519, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006520, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006521, 16, "Arena_Shop_CanBuy")
aux.RegisterCreatureEvent(4006514, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006515, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006516, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006517, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006518, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006519, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006520, 17, "Arena_Shop_OnBuy")
aux.RegisterCreatureEvent(4006521, 17, "Arena_Shop_OnBuy")


--[[function Arena_Shop_IsShopOpen(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	return 0
end
aux.RegisterCreatureEvent(4006514, 15, "Arena_Shop_IsShopOpen")
aux.RegisterCreatureEvent(4006515, 15, "Arena_Shop_IsShopOpen")]]
--aux.RegisterCreatureEvent(4006516, 15, "Arena_Shop_IsShopOpen")
--aux.RegisterCreatureEvent(4006517, 15, "Arena_Shop_IsShopOpen")
--aux.RegisterCreatureEvent(4006518, 15, "Arena_Shop_IsShopOpen")


