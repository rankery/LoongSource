--怪物喊话脚本

-- 怪物进入战斗喊话
function c1010526_OnEnterCombat(MapID, InstanceID, CreatureID)

       cre.MonsterSay(MapID, InstanceID, CreatureID, 50069)

end

aux.RegisterCreatureEvent(1010526, 2, "c1010526_OnEnterCombat")

--怪物死亡喊话
function c1010526_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 50070)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,4)+4
	map.SetMapScriptData(MapID,InstanceID,1,4,Boss_num)

	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 14)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1777)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

end
aux.RegisterCreatureEvent(1010526, 4, "c1010526_OnDie")


