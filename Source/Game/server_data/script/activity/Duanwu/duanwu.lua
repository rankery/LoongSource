
Duanwu_On = 0
Duanwu_Boss_F = 0
Duanwu_Boss_W = 0

function Duanwu_OnStart(actID)
	Duanwu_On = 1
end

function Duanwu_OnEnd(actID)
	Duanwu_On = 0
end


function Duanwu_OnTimerMin(actID)

	local curmin = tonumber(os.date("%M"))
	local curhour = tonumber(os.date("%H"))

	if curhour%2==1 and curmin==34 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3305+Duanwu_Mode)	--活动指引性公告
		msg.DispatchWorldMsgEvent(MsgID)

	end

	if curhour == 20 and curmin == 55 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3301)	--即将开始刷BOSS
		msg.DispatchWorldMsgEvent(MsgID)

	end

	if curhour == 21 and curmin%10 == 5 and curmin~=55 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3301)	--即将开始刷BOSS
		msg.DispatchWorldMsgEvent(MsgID)

	end

	if curhour == 21 and curmin%10 == 0 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3302)	--Boss已刷出
		msg.DispatchWorldMsgEvent(MsgID)

		Duanwu_Boss_F = map.MapCreateCreature(3017298383, -1, 4900485, 938, 10000, 2016) --刷凤翔
		Duanwu_Boss_W = map.MapCreateCreature(3017299919, -1, 4900485, 1166, 20140, 890) --刷巫水

	end

end

--注册该活动的相关事件
aux.RegisterActEvent(146, 4, "Duanwu_OnTimerMin")	--活动ID
aux.RegisterActEvent(146, 2, "Duanwu_OnStart")	--活动ID
aux.RegisterActEvent(146, 3, "Duanwu_OnEnd")		--活动ID

function I3304149_CanUse(MapID, InstanceID, TypeID, TargetID)

	local bRet, bIgnore = 43, false
	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)

	if MapID== 3017298383 then

		if x-938>=-50 and x-938<=50 and z-2016>=-50 and z-2016<=50 and Duanwu_Boss_F~=0 then
			bRet = 0
		end

	elseif MapID== 3017299919 then

		if x-1166>=-50 and x-1166<=50 and z-890>=-50 and z-890<=50 and Duanwu_Boss_W~=0 then
			bRet = 0
		end

	end

	return bRet, bIgnore

end

function I3304149_OnUse(MapID, InstanceID, TypeID, TargetID)

	--给BOSS添加BUFF

	if MapID== 3017298383 then
		unit.AddBuff(MapID, InstanceID, Duanwu_Boss_F, 9431001, Duanwu_Boss_F)
	elseif MapID== 3017299919 then
		unit.AddBuff(MapID, InstanceID, Duanwu_Boss_W, 9431001, Duanwu_Boss_W)
	end

	--获得经验金钱奖励

	local rolelevel = role.GetRoleLevel(MapID, InstanceID, TargetID)
	local LevelUpNeedExp = role.GetRoleLevelUpExp(MapID, InstanceID, TargetID)
	local addexp = LevelUpNeedExp * 30 / (rolelevel+10)^2

	role.AddRoleExp(MapID, InstanceID, TargetID, addexp)

	local addsilver = 50000

	role.AddRoleSilver(MapID, InstanceID, TargetID, addsilver, 102)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 3303)	--Boss掉血
	msg.DispatchRoleMsgEvent(TargetID, MsgID)

end

aux.RegisterItemEvent(3304149, 0, "I3304149_CanUse")
aux.RegisterItemEvent(3304149, 1, "I3304149_OnUse")

--BOSS死亡，刷小BOSS
function c4900485_OnDie(MapID, InstanceID, CreatureID)

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 101, 3304)	--BOSS已死亡
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	if MapID== 3017298383 then

		map.MapCreateCreature(3017298383, -1, 4900488, 938, 10000, 2016)

		Duanwu_Boss_F=0
		map.MapCreateCreature(3017298383, -1, 1534143, 857, 10000, 1930)
		map.MapCreateCreature(3017298383, -1, 1534143, 917, 10000, 1986)
		map.MapCreateCreature(3017298383, -1, 1534143, 960, 10000, 2073)
		map.MapCreateCreature(3017298383, -1, 1534143, 801, 10000, 2077)
		map.MapCreateCreature(3017298383, -1, 1534143, 708, 10000, 2049)
		map.MapCreateCreature(3017298383, -1, 1534143, 762, 10000, 1945)
		map.MapCreateCreature(3017298383, -1, 1534143, 1012, 10000, 1975)
		map.MapCreateCreature(3017298383, -1, 1534143, 1068, 10000, 2158)
		map.MapCreateCreature(3017298383, -1, 1534143, 887, 10000, 2209)
		map.MapCreateCreature(3017298383, -1, 1534143, 728, 10000, 2196)
	elseif MapID== 3017299919 then

		map.MapCreateCreature(3017299919, -1, 4900488, 1166, 20140, 890)

		Duanwu_Boss_W=0
		map.MapCreateCreature(3017299919, -1, 1534143, 1141, 20140, 818)
		map.MapCreateCreature(3017299919, -1, 1534143, 1150, 20140, 947)
		map.MapCreateCreature(3017299919, -1, 1534143, 998, 20140, 970)
		map.MapCreateCreature(3017299919, -1, 1534143, 899, 20140, 1001)
		map.MapCreateCreature(3017299919, -1, 1534143, 821, 20140, 911)
		map.MapCreateCreature(3017299919, -1, 1534143, 820, 20140, 962)
		map.MapCreateCreature(3017299919, -1, 1534143, 820, 20140, 857)
		map.MapCreateCreature(3017299919, -1, 1534143, 1039, 20140, 1005)
		map.MapCreateCreature(3017299919, -1, 1534143, 1455, 21823, 1005)
		map.MapCreateCreature(3017299919, -1, 1534143, 1453, 21823, 825)
	end

end

aux.RegisterCreatureEvent(4900485, 4, "c4900485_OnDie")
aux.RegisterCreatureEvent(4900485, 13, "c4900485_OnDie")
