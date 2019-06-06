--女王蜂-叁：10439，
--需求1：在龙城坐标<3488, 791>范围内使用物品梦露花蜜[2610172]，刷新出怪物巨峰女王[1003167]

--梦露花蜜物品使用判断函数
function i2610172_CanUse(MapID, InstanceID, TypeID, RoleID)
    local bRet, bIgnore = 0, false

	--如果不在龙城场景，则不能使用
	if MapID ~= 3017298127 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)

	    if x<=3448 or x>=3538 or z<=751 or z>=831 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,4) >= 10 then
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

--梦露花蜜使用效果函数
function i2610172_QuestUsable(MapID, InstanceID, TypeID, TargetID)
--m02地图索引4，10439 巨峰女王[1003167]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,4)
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1003167, x+2, y, z+2)
		map.SetMapScriptData(MapID,InstanceID,1,4,a+1)
	end
end

--注册
aux.RegisterItemEvent(2610172, 1, "i2610172_QuestUsable")
aux.RegisterItemEvent(2610172, 0, "i2610172_CanUse")

function c1003167_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,4)
	map.SetMapScriptData(MapID,InstanceID,1,4,a-1)
end
aux.RegisterCreatureEvent(1003167,4,"c1003167_OnDie")
