--15408使用道具玉龙幡2615229，可在雪瘟寨的枯树边召唤NPC一眉真人 （3070209）(1222,17387,874)一眉真人刷出上限为1个

--可否使用函数
function i2615229_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    return 43, false
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 1152 or x > 1292 or z < 804 or z > 944 then
	          return 43, false 
            end
	end
	local m = map.GetMapScriptData(MapID, InstanceID, 1, 0)
	if m == 1 then 
	    return 32, false
	end
    return bRet, bIgnore
end

--使用效果
function i2615229_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 3070209, 1222, 17387, 874)
       map.SetMapScriptData(MapID, InstanceID, 1, 0, 1)
end

--注册
aux.RegisterItemEvent(2615229, 1, "i2615229_QuestUsable")
aux.RegisterItemEvent(2615229, 0, "i2615229_CanUse")

--一眉真人消失时将标志位置0
function c3070209_OnDisappear(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
       map.SetMapScriptData(MapID, InstanceID, 1, 0, 0)
end
--注册
aux.RegisterCreatureEvent(3070209, 13, "c3070209_OnDisappear")