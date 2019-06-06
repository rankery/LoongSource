
--BOSS合元天魔第一形态（1010410）血量40%时消失且同时刷出
--第二形态（1010411）X=500 Y=5664 Z=1778
--冷酷守卫（1010417）
--X=495 Y=5664 Z=1795
--X=508 Y=5664 Z=1796
--邪念道人（1010414）
--X=486 Y=5664 Z=1781
--X=515 Y=5664 Z=1780
--X=497 Y=5664 Z=1765
--X=506 Y=5664 Z=1765
--1524和1525大门初始为关闭状态，第一形态（1010410）消失时，大门开启。第二形态（1010411）进入战斗后，大门关闭。击杀第二形态大门开启。
--1526 大门初始为关闭状态，使用开门地物，大门打开。

--[[function s2423701_Cast(MapID, InstanceID, SkillID, OwnerID)
	map.SetMapScriptData(MapID, InstanceID, 1, 5, 1)
	local TypeID = cre.GetCreatureTypeID(MapID, InstanceID, OwnerID)
	if TypeID == 1010410 then
		cre.MonsterSay(MapID, InstanceID, OwnerID, 50064)
	end
	return 0
end

--注册

aux.RegisterSkillEvent(2423701, 1, "s2423701_Cast")]]

-- 怪物进入战斗喊话
function c1010410_OnEnterCombat(MapID, InstanceID, CreatureID)

	cre.MonsterSay(MapID, InstanceID, CreatureID, 50063)

end

aux.RegisterCreatureEvent(1010410, 2, "c1010410_OnEnterCombat")

function c1010410_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
--    local i = map.GetMapScriptData(MapID, InstanceID, 1, 5)
--	if i == 1 then
--	    map.MapDeleteCreature(MapID, InstanceID, TargetID)
        cre.MonsterSay(MapID, InstanceID, TargetID, 50064)
	    local BossID = map.MapCreateColCreature(MapID, InstanceID, 1010411, 500, 5664, 1778, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010417, 495, 5664, 1796, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010417, 508, 5664, 1796, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010414, 486, 5664, 1781, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010414, 515, 5664, 1780, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010414, 497, 5664, 1765, 1, "")
	    map.MapCreateColCreature(MapID, InstanceID, 1010414, 506, 5664, 1765, 1, "")
	   -- map.OpenCloseDoor(MapID, InstanceID, 1524, 1)
	   -- map.OpenCloseDoor(MapID, InstanceID, 1525, 1)
	    map.SetMapScriptData(MapID, InstanceID, 1, 2, BossID)

	local Num = map.GetMapScriptData(MapID, InstanceID, 1, 13) + 1000
	map.SetMapScriptData(MapID, InstanceID, 1, 13, Num)
	if Num >= 1100 then
	    local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 3351)	--通往合元天魔第二形态房间的大门已经打开！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	    map.OpenCloseDoor(MapID, InstanceID, 1524, 1)
	    map.OpenCloseDoor(MapID, InstanceID, 1525, 1)
	elseif Num < 1100 then
	    local a = math.floor(Num/1000)
		local b = Num % 1000
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3352)	--击杀喽啰XX/100,击杀合元天魔第一形态X/1
		msg.AddMsgEvent(MsgID, 9, b)
		msg.AddMsgEvent(MsgID, 9, a)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 3353)	--条件完成后将会开启通往最终首领房间的大门！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
--	end
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010410, 4, "c1010410_OnDie")


function c1010411_OnEnterCombat(MapID, InstanceID, CreatureID)
    cre.MonsterSay(MapID, InstanceID, CreatureID, 50065)
	map.OpenCloseDoor(MapID, InstanceID, 1524, nil)
	map.OpenCloseDoor(MapID, InstanceID, 1525, nil)

end

aux.RegisterCreatureEvent(1010411, 2, "c1010411_OnEnterCombat")

function c1010411_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

    local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
	local Role = {}
	Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
	for i=1, 6 do
	    if Role[i] and Role[i]~= 4294967295 then
		    local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
			if Target_MapID == MapID then
			    role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 3305003, 5, -1, 8, 420)
			    if act.GetActIsStart(151) then
				    HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
					HuoDeYinYuanJie(Role[i])
				end
			end
	    end
	end

    cre.MonsterSay(MapID, InstanceID, TargetID, 50066)
	map.OpenCloseDoor(MapID, InstanceID, 1524, 1)
	map.OpenCloseDoor(MapID, InstanceID, 1525, 1)
	map.MapCreateCreature(MapID, InstanceID, 3075203, 500, 5664, 1778)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

end
aux.RegisterCreatureEvent(1010411, 4, "c1010411_OnDie")

function c1010411_LeaveCombat(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.OpenCloseDoor(MapID, InstanceID, 1524, 1)
	map.OpenCloseDoor(MapID, InstanceID, 1525, 1)

end
aux.RegisterCreatureEvent(1010411, 3, "c1010411_LeaveCombat")



function c3075211_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local BossID = map.GetMapScriptData(MapID, InstanceID, 1, 2)
	unit.CancelBuff(MapID, InstanceID, BossID, 9422602)
end
aux.RegisterCreatureEvent(3075211, 4, "c3075211_OnDie")
