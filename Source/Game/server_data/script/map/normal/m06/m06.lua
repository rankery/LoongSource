


--记录m06地图ScriptData怪物同时存在的数量
	--m06地图索引1，14217 5个密幽伏兵 1007107 记录数量
	--m06地图索引2，14217 一个密幽伏兵将领1007108 记录数量
	--m06地图索引3，14237 枭吴敢死队队员 1008006 记录数量
	--m06地图索引4，14237 枭吴敢死队队长 1008007 记录数量
	--m06地图索引5，14254 BOSS深潭猛兽 1007113 记录数量
	--m06地图索引6，14026 诘珂的同党 1007026 记录数量
	--m06地图索引7，记录战神化身活动中的化身刷新数量



--怪物消失或者死亡，将ScriptData 数值减1
function m06_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)
	if CreatureTypeID == 1007107 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,1)
		map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
	elseif CreatureTypeID == 1007108 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
	elseif CreatureTypeID == 1008006 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,3)
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	elseif CreatureTypeID == 1008007 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		map.SetMapScriptData(MapID,InstanceID,1,4,a-1)
	elseif CreatureTypeID == 1007113 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,5)
		map.SetMapScriptData(MapID,InstanceID,1,5,a-1)
	elseif CreatureTypeID == 1007026 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,6)
		map.SetMapScriptData(MapID,InstanceID,1,6,a-1)
	end

end
aux.RegisterMapEvent("m06", 18, "m06_OnDisappear")
