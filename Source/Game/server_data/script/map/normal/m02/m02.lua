


--记录m02地图ScriptData怪物同时存在的数量
--m02地图索引1，10423 半里雄蜂[1003081]记录数量
--m02地图索引2，10437 毒蜂女王[1003165]记录数量
--m02地图索引3，10438 幻水蜂女王[1003166]记录数量
--m02地图索引4，10439 巨峰女王[1003167]记录数量
	--m02地图索引5，13233 个苍莽刀斧手[1006207]记录数量


--怪物消失或者死亡，将ScriptData索引1 数值减1
function m02_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)
	if CreatureTypeID == 1003165 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,2)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
	elseif CreatureTypeID == 1003081 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,1)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
	elseif CreatureTypeID == 1003166 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,3)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
	elseif CreatureTypeID == 1003167 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,4)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,4,a-1)
	elseif CreatureTypeID == 1006207 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,5)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,5,a-1)
	--[[elseif CreatureTypeID == 1006209 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,6)
		if a < 1 or a > 50  then
			a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,6,a-1)]]
	end

end
aux.RegisterMapEvent("m02", 18, "m02_OnDisappear")


Marriage_Available = 0

--加载m02时，若全局变量TW_Endorsor_On=1，则刷新NPC纪妍美眉
function m02_OnInit(MapID,InstanceID,CreatureID,CreatureTypeID)
	if TW_Endorsor_On == 1 then
		map.MapCreateColCreature(MapID, InstanceID, 4900446, 2270, 6648, 2266, 1, "")
	end
	if Open_XJ_XiaoLongNv == 1 then
		map.MapCreateColCreature(MapID, InstanceID, 4900449, 2171, 6646, 2219, 1, "")
		map.MapCreateColCreature(MapID, InstanceID, 4900450, 2166, 6646, 2219, 1, "")
	    XJ_XiaoLongNV_InitQuestionTable()
	end

	--for i = 20,29 do
		--if (act.GetActScriptData(151, 1, i) == 0 and act.GetActScriptData(151, 1, i-20) ~= 0) or act.GetActIsStart(151) or act.GetActScriptData(151, 1, 0) == 0 then
			map.MapCreateCreature(3017298127, -1, 4900490, 2140, 6646, 2258)
			--break
		--end
	--end
	map.MapCreateCreature(3017298127, -1, 4900491, 2133, 6646, 2258)

end
aux.RegisterMapEvent("m02", 0, "m02_OnInit")

