--烈酒引妖：11279
--需求1：在环形湖祭坛[m03: 2056, 910]附近使用物品烈酒[2611410]后刷新怪物酒吞童子[1004607]

--物品烈酒使用效果脚本函数
function i2611410_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--m03地图索引1，11279 酒吞童子[1004607]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,1)
	if a == nil then
		a = 0
	end
	if a < 3 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1004607, x+2, y, z+2)
		map.SetMapScriptData(MapID,InstanceID,1,1,a+1)
	end
end

--物品烈酒使用判断脚本函数
function i2611410_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	--必须在凤翔使用
	if MapID ~= 3017298383 then
	    bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x<=2040 or x>=2072 or z<=894 or z>=926 then --必须在该位置附近使用
	        bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,1) >= 3 then
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

--注册
aux.RegisterItemEvent(2611410, 1, "i2611410_QuestUsable")
aux.RegisterItemEvent(2611410, 0, "i2611410_CanUse")

--怪物消失或者死亡，将ScriptData索引 数值减1


function c1004607_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,1) --副本Data索引1
	map.SetMapScriptData(MapID,InstanceID,1,1,a-1)
end
aux.RegisterCreatureEvent(1004607,4,"c1004607_OnDie")
