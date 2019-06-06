--RFX908
--在试炼幻境（1826990375）调查玄灵精魄（3011008）刷出烈风战魂（1531006）
--map.SetMapScriptData(MapID, InstanceID, 1, 2, 0)第几次调查玄灵精魄，每次%4余数为3的时候刷出烈风战魂
--map.SetMapScriptData(MapID, InstanceID, 1, 1, 0)第几次触碰玄灵精魄，单数次触碰获得血晶石，双数次触碰获得太阳石和风吟石的效果
--调查玄灵精魄
function c3011008_OnCheck(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       local m = map.GetMapScriptData(MapID, InstanceID, 1, 1)
       local n = map.GetMapScriptData(MapID, InstanceID, 1, 2)
       local i = m%2
       if i==0 then
           unit.AddBuff(MapID, InstanceID, RoleID, 2016901, RoleID)
       elseif i==1 then
           unit.AddBuff(MapID, InstanceID, RoleID, 2017001, RoleID)
           unit.AddBuff(MapID, InstanceID, RoleID, 2017101, RoleID)
       end
       local k = n%3
       if k == 2 or n == 0 then 
           map.MapCreateCreature(MapID, InstanceID, 1531006, 216, 160, 233)
       end
       map.SetMapScriptData(MapID, InstanceID, 1, 1, m+1)
       map.SetMapScriptData(MapID, InstanceID, 1, 2, n+1)
end

aux.RegisterCreatureEvent(3011008, 6, "c3011008_OnCheck")


--任务接取时函数
--function x11908_OnAccept(MapID, InstanceID, questID, ownerID, accepterID)
--       role.RoleGotoNewMap(MapID, InstanceID, ownerID, 1826990375, 171, 5, 32)
--end

--注册
--aux.RegisterQuestEvent(11908, 0, "x11908_OnAccept")


