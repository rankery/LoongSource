--1005461 精明妖（冰封魔窟）体力低于50%后召唤
--{1005486 召唤法\示警脚本
---------------------------------------------------------暂时以进入战斗代替体力50%，后期修改
function c1005461_On_Enter_Combat(MapID, InstanceID, CreatureID)
	-- 怪物进入战斗喊话
	cre.MonsterSay(MapID, InstanceID, CreatureID, 20062)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, CreatureID)
	map.MapCreateCreature(MapID, InstanceID, 1005472, x, y, z+10)
	--得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为8个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)

end
aux.RegisterCreatureEvent(1005461,2,"c1005461_On_Enter_Combat")

--怪物死亡喊话
function c1005461_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 2, "jinjieshili")


	local xiaoguai_num = map.GetMapScriptData(MapID, InstanceID, 1, 11)
	local Boss_num = map.GetMapScriptData(MapID, InstanceID, 1, 8)+8
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

aux.RegisterCreatureEvent(1005461, 4, "c1005461_OnDie")


