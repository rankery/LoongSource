--1005419	魔猿将(冰封魔窟) /示警脚本
--进入战斗后召唤 {1005411 殇妖功曹[左前]} {1005412 殇妖功曹[右前]}
function c1005419_On_Enter_Combat(MapID, InstanceID, CreatureID)
	-- 怪物进入战斗喊话
	cre.MonsterSay(MapID, InstanceID, CreatureID, 20059)

	--local x, y ,z = unit.GetPosition(MapID, InstanceID, CreatureID)
	--map.MapCreateCreature(MapID, InstanceID, 1005411, x-5, y, z)
	--map.MapCreateCreature(MapID, InstanceID, 1005412, x+5, y, z)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end
aux.RegisterCreatureEvent(1005419,2,"c1005419_On_Enter_Combat")

--怪物死亡喊话
function c1005419_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	cre.MonsterSay(MapID, InstanceID, TargetID, 20061)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")


	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 11)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 8)+2
	map.SetMapScriptData(MapID, InstanceID, 1, 8, Boss_num)

	local Boss1 = Boss_num % 2
	local Boss2 = math.floor(Boss_num/2) % 2
	local Boss3 = math.floor(Boss_num/4) % 2
	local Boss4 = math.floor(Boss_num/8) % 2

	local temp = Boss1+Boss2+Boss3+Boss4

	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 1756)	--已击杀副本小怪xx/xx，已击杀副本Bossxx/4。
	msg.AddMsgEvent(MsgID, 9, xiaoguai_num)
	msg.AddMsgEvent(MsgID, 9, temp)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

aux.RegisterCreatureEvent(1005419, 4, "c1005419_OnDie")
