


--记录m04地图ScriptData怪物同时存在的数量
--m04索引1 ，12274，1005140，洪庭护卫 2个
--m04索引2 ，12274，1005139，洪庭亲信 1个
--m04索引3 ，轩辕神龙 召唤绿晶球 消失数量
--m04索引4 ，轩辕神龙 召唤紫晶球 消失数量


--怪物消失或者，将ScriptData 数值减1
function m04_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)
	if CreatureTypeID == 1005140 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,1)
		map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
	elseif CreatureTypeID == 1005139 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
	end
end
aux.RegisterMapEvent("m04", 18, "m04_OnDisappear")


--加载m04时，若全局变量Salary_NPC_On=1，则刷新NPC《龙》薪酬使者
function m04_OnInit(MapID,InstanceID,CreatureID,CreatureTypeID)
	if Salary_NPC_On == 1 then
		map.MapCreateColCreature(MapID, InstanceID, 3610202, 2158, 18338, 2396, 1, "")
	end
	if Open_Arena == 1 then
		map.MapCreateColCreature(MapID, InstanceID, 4006512, 2071, 18345, 2666, 1, "")
	end

	--for i = 20,29 do
		--if (act.GetActScriptData(151, 1, i) == 0 and act.GetActScriptData(151, 1, i-20) ~= 0) or act.GetActIsStart(151)  or act.GetActScriptData(151, 1, 0) == 0 then
			map.MapCreateCreature(3017299663, -1, 4900490, 2093, 18345, 2724)
			--break
		--end
	--end
	map.MapCreateCreature(3017299663, -1, 4900491, 2093, 18345, 2718)

end
aux.RegisterMapEvent("m04", 0, "m04_OnInit")

