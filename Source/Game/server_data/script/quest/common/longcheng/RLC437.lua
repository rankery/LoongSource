--女王蜂-壹：10437
--需求1：在龙城坐标<3351, 837>和<3371, 857>范围内使用物品引蜂香露[2610170]，刷新出怪物毒蜂女王[1003165]

--引蜂香露物品使用判断函数
function i2610170_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false

	--如果不在龙城场景，则不能使用
	if MapID ~= 3017298127 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	    if x<=3351 or x>=3371 or z<=837 or z>=857 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,2) >= 10 then
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

--引蜂香露物品使用效果函数
function i2610170_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--m02地图索引2，10437 毒蜂女王[1003165]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1003165, x+2, y, z+2)
		map.SetMapScriptData(MapID,InstanceID,1,2,a+1)
	end

end

--注册
aux.RegisterItemEvent(2610170, 1, "i2610170_QuestUsable")
aux.RegisterItemEvent(2610170, 0, "i2610170_CanUse")

--怪物死亡，将ScriptData索引2 数值减1


function c1003165_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,2)
	map.SetMapScriptData(MapID,InstanceID,1,2,a-1)
end
aux.RegisterCreatureEvent(1003165,4,"c1003165_OnDie")
