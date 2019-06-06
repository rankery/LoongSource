--女王蜂-贰：10438
--需求1：在龙城坐标<3584, 639>半径30范围内使用物品引蜂香露[2610171]，刷新出怪物幻水蜂女王[1003166]

--引蜂香露可否使用函数
function i2610171_CanUse(MapID, InstanceID, TypeID, RoleID)
    local bRet, bIgnore = 0, false

	--如果不在龙城场景，则不能使用
	if MapID ~= 3017298127 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
	    if x<=3554 or x>=3614 or z<=609 or z>=669 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
		else
			if map.GetMapScriptData(MapID,InstanceID,1,3) >= 10 then
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

--引蜂香露使用效果
function i2610171_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	--m02地图索引3，10438 幻水蜂女王[1003166]记录数量
	local a = map.GetMapScriptData(MapID,InstanceID,1,3)
	if a == nil then
		a = 0
	end
	if a < 10 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1003166, x+2, y, z+2)
		map.SetMapScriptData(MapID,InstanceID,1,3,a+1)
	end
end

--注册
aux.RegisterItemEvent(2610171, 1, "i2610171_QuestUsable")
aux.RegisterItemEvent(2610171, 0, "i2610171_CanUse")

--怪物死亡，将ScriptData索引3 数值减1


function c1003166_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local a = map.GetMapScriptData(MapID,InstanceID,1,3)
	map.SetMapScriptData(MapID,InstanceID,1,3,a-1)
end
aux.RegisterCreatureEvent(1003166,4,"c1003166_OnDie")
