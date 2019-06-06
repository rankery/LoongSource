--RWY237：死战
--需求1：接完任务刷新敌人4个枭吴敢死队队员，一个枭吴敢死队队长，玩家需要全部击杀。
--m41副本

function x14237_OnAccept(MapID, InstanceID, QuestID, TargetID, AccepterID)

	--41地图索引10，14237 枭吴敢死队队员 1008006 记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,10)--枭吴敢死队队员 1008006 ,地图索引10
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1008006, 2104, 20897, 404)
		map.SetMapScriptData(MapID,InstanceID,1,10,a+1)
	end

	--41地图索引10，14237 枭吴敢死队队长 1008007 记录数量
	local b = map.GetMapScriptData(MapID,InstanceID,1,11)--枭吴敢死队队长,地图索引11
	if b == nil then
		b = 0
	end
	if b < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1008007, 2101, 20895 , 407)
		map.SetMapScriptData(MapID,InstanceID,1,11,b+1)
	end
end

aux.RegisterQuestEvent(14237, 0, "x14237_OnAccept")

function q14237_OnCheckAccept(MapID, InstanceID, QuestID, RoleID, NPCID)
	if map.GetMapScriptData(MapID,InstanceID,1,10) >= 10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	elseif map.GetMapScriptData(MapID,InstanceID,1,11) >= 10 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 13, 0)
		msg.AddMsgEvent(MsgID, 1, 148)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
		return 0
	end
	return 1
end

aux.RegisterQuestEvent(14237, 4, "q14237_OnCheckAccept")

--怪死亡，将ScriptData索引 数值减1
function c1008006_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, TargetID, 40024)
	end
	local a = map.GetMapScriptData(MapID,InstanceID,1,10) --副本Data索引10
	if a < 1 then
	a = 1
	end
	map.SetMapScriptData(MapID,InstanceID,1,10,a-1)
end
aux.RegisterCreatureEvent(1008006,4,"c1008006_OnDie")

function c1008007_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, TargetID, 40026)
	end
	local a = map.GetMapScriptData(MapID,InstanceID,1,11) --副本Data索引11
	if a < 1 then
	a = 1
	end
	map.SetMapScriptData(MapID,InstanceID,1,11,a-1)
end
aux.RegisterCreatureEvent(1008007,4,"c1008007_OnDie")
