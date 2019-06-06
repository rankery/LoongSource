
--12274，在村口[3020-3100,y,1513-1557]，使用物品哨子[2612148]，召唤2个洪庭护卫[1005140]，1个洪庭亲信[1005139]



--物品哨子[2612148] 在村口[3020-3100,y,1513-1557]之外坐标不可用
function I2612148_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--不在涿鹿不可使用
	if MapID ~= 3017299663 then
	    bRet = 43
	else
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
        if x<=3020 or x>=3100 or z<= 1513 or z>=1557 then  --必须在该范围内使用
            bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,1) >= 2 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 148)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				bRet = 32
			elseif map.GetMapScriptData(MapID,InstanceID,1,2) >= 1 then
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 13, 0)
				msg.AddMsgEvent(MsgID, 1, 148)
				msg.DispatchRoleMsgEvent(RoleID, MsgID)
				bRet = 32
			end
        end
    end
	return bRet, bIgnore
end



--使用哨子[2612148]召唤2个洪庭护卫[1005140]，1个洪庭亲信[1005139]
function I2612148_Use(MapID, InstanceID, TypeID, RoleID)
	--索引1 ，12274，1005140，洪庭护卫
	--索引2 ，12274，1005139，洪庭亲信
	local m = map.GetMapScriptData(MapID,InstanceID,1,1)
	local n = map.GetMapScriptData(MapID,InstanceID,1,2)

	if m == nil then
		m = 0
	end
	if n == nil then
		n = 0
	end

	local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	map.MapCreateCreature(MapID, InstanceID, 1005140, x-2, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1005140, x+2, y, z)
	map.MapCreateCreature(MapID, InstanceID, 1005139, x, y, z+2)

	map.SetMapScriptData(MapID,InstanceID,1,1,m+2)
	map.SetMapScriptData(MapID,InstanceID,1,2,n+1)

end

aux.RegisterItemEvent(2612148, 1, "I2612148_Use")
aux.RegisterItemEvent(2612148, 0, "I2612148_CanUse")

function c1005140_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local a = map.GetMapScriptData(MapID,InstanceID,1,1)
	map.SetMapScriptData(MapID,InstanceID,1,1,a-1)

end
aux.RegisterCreatureEvent(1005140,4,"c1005140_OnDie")

function c1005139_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
end
aux.RegisterCreatureEvent(1005139,4,"c1005139_OnDie")
