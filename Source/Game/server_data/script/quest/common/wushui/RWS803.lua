--盘阳妖众：13803
--需求1：在盘阳山顶[m05: 1991, 1396]附近使用物品盘阳妖令[2613606]后刷新出怪物盘阳巡山小妖[1006607]
--需求2：盘阳悬山小妖[1006607]死亡后刷新出盘阳妖使[1006608]
--需求3：盘阳妖使[1006608]死亡后刷新出盘阳左法王[1006609]和盘阳右法王[1006610]
--需求4：盘阳左法王[1006609]死亡后刷新出盘阳妖祖[1006611]

--物品盘阳妖令使用判断
function i2613606_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	--不在巫水不能使用
	if MapID ~= 3017299919 then
	    bRet = 43
	else
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=1975 or x>=2007 or z<=1380 or z>=1412 then    --不在该位置不能使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--物品盘阳妖令使用效果函数
function i2613606_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.MapCreateCreature(MapID, InstanceID, 1006607, 1991, 24790, 1396)
end

--注册
aux.RegisterItemEvent(2613606, 1, "i2613606_QuestUsable")
aux.RegisterItemEvent(2613606, 0, "i2613606_CanUse")

--怪物盘阳巡山小妖死亡函数
function c1006607_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006608, 1991, 24790, 1396)
end

--怪物盘阳妖使死亡函数
function c1006608_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1006609, 1988, 24790, 1396)
	map.MapCreateCreature(MapID, InstanceID, 1006610, 1994, 24790, 1396)
end

--怪物盘阳左法王死亡函数
function c1006609_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    cre.MonsterSay(MapID, InstanceID, TargetID, 30122)
	map.MapCreateCreature(MapID, InstanceID, 1006611, 1991, 24790, 1396)
end

--注册
aux.RegisterCreatureEvent(1006607, 4, "c1006607_OnDie")
aux.RegisterCreatureEvent(1006608, 4, "c1006608_OnDie")
aux.RegisterCreatureEvent(1006609, 4, "c1006609_OnDie")
