--RFX908刷出玄灵精魄，RZL910刷出怪物
--ScriptData 0为记录是否已经刷12910任务怪；1，2为记录11908的宝石调查次数与是否刷怪；

function q02_OnPlayerEnter(MapID, InstanceID, RoleID)
       local k = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 12910)
	   local i = map.GetMapScriptData(MapID, InstanceID, 1, 0)
       if k == true and i ~= 1 then
            unit.AddBuff(MapID, InstanceID, RoleID, 2019501, RoleID)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 164, 120, 88)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 154, 140, 113)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 170, 119, 152)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 189, 140, 155)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 214, 108, 145)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 203, 140, 244)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 199, 140, 232)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 231, 140, 232)
            map.MapCreateCreature(MapID, InstanceID, 1531007, 226, 140, 244)
            map.MapCreateCreature(MapID, InstanceID, 1531008, 216, 160, 233)
	    map.SetMapScriptData(MapID, InstanceID, 1, 0, 1)
       end
       local a = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 11908)
       if a == true then
	    map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)
		map.SetMapScriptData(MapID, InstanceID, 1, 2, 0)
       end
       local b = role.IsRoleHaveQuest(MapID, InstanceID, RoleID, 11907)
       if b == true then
            map.MapCreateCreature(MapID, InstanceID, 1531005, 216, 160, 233)
       end
end

--注册
aux.RegisterMapEvent("q02", 2, "q02_OnPlayerEnter")


