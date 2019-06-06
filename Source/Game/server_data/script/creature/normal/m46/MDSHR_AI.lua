function c3075218_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local temp = map.GetMapScriptData(MapID, InstanceID, 1, 8)+1000
	map.SetMapScriptData(MapID, InstanceID, 1, 8, temp)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1010425, GetMiddle(x, 705, 793), y, GetMiddle(z, 974, 1145))
	map.MapCreateCreature(MapID, InstanceID, 1010425, GetMiddle(x+8, 705, 793), y, GetMiddle(z+5, 974, 1145))
	map.MapCreateCreature(MapID, InstanceID, 1010425, GetMiddle(x+5, 705, 793), y, GetMiddle(z-6, 974, 1145))
	map.MapCreateCreature(MapID, InstanceID, 1010426, GetMiddle(x-7, 705, 793), y, GetMiddle(z+8, 974, 1145))

end
aux.RegisterCreatureEvent(3075218, 6, "c3075218_OnInvest")


function GetMiddle(value, min_v, max_v)
	if value>= max_v then
		return max_v
	end

	if value<= min_v then
		return min_v
	end

	return value
end

function m46_qiandi_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local temp = map.GetMapScriptData(MapID, InstanceID, 1, 8)+1
	map.SetMapScriptData(MapID, InstanceID, 1, 8, temp)

	if temp == 7039 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1755)	--秘道守护人已经出现于坐标[751, 1088]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		map.MapCreateCreature(MapID, InstanceID, 1010428, 751, 3840, 1088)
	else
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1754)	--已击杀潜地妖魔xx/39，已调查秘道墓穴x/7
		msg.AddMsgEvent(MsgID, 9, temp%1000)
		msg.AddMsgEvent(MsgID, 9, math.floor(temp/1000))
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end
end

aux.RegisterCreatureEvent(1010425, 4, "m46_qiandi_OnDie")
aux.RegisterCreatureEvent(1010426, 4, "m46_qiandi_OnDie")
