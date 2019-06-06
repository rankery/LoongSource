--噬牛之鬼：13449
--需求1：在牛骨石祭坛[m05: 1821, 2555]附近使用物品牛骨石祭品[2613226]后刷新怪物噬牛鬼[1006321]

--物品牛骨石使用判断脚本
function i2613226_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false

	--不在巫水不可使用
	if MapID ~= 3017299919 then
	    bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	    if x<=1806 or x>=1838 or z<=2539 or z>=2571 then
		    bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,1) >= 10 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 13, 0)
			msg.AddMsgEvent(MsgID, 1, 148)
			msg.DispatchRoleMsgEvent(RoleID, MsgID)
			bRet = 32
			end
	    end
	end

	--返回
	return bRet, bIgnore
end

--物品牛骨石使用效果脚本
function i2613226_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--m05地图索引1，13449 噬牛鬼[1006321]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,1)
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1006321, 1822, 24427, 2555)
		map.SetMapScriptData(MapID,InstanceID,1,1,a+1)
	end
end

--注册
aux.RegisterItemEvent(2613226, 1, "i2613226_QuestUsable")
aux.RegisterItemEvent(2613226, 0, "i2613226_CanUse")

function c1006321_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,1)
	map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
end
aux.RegisterCreatureEvent(1006321,4,"c1006321_OnDie")
