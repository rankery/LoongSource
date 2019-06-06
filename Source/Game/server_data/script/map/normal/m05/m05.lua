


--记录m05地图ScriptData怪物同时存在的数量
	--m05地图索引1，13449 噬牛鬼[1006321]记录数量
	--m05地图索引2，13469 心欲魔[1006151]记录数量
	--m05地图索引3，13602 四个官兵[1006404]记录数量



--怪物消失或者死亡，将ScriptData 数值减1
function m05_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)
	if CreatureTypeID == 1006321 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,1)
		map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
	elseif CreatureTypeID == 1006151 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
	elseif CreatureTypeID == 1006404 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,3)
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	end

end
aux.RegisterMapEvent("m05", 18, "m05_OnDisappear")
