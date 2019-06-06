--挖宝至尊：11461
--需求1：调查地物五色山土包[3020438]时50%几率在玩家身边刷新怪物钢臂青猿[1004106]

--地物五色山土包被调查脚本
function c3020438_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local Rate = math.random(1, 100)
	if Rate <= 50 then
		local x, y ,z = unit.GetPosition(MapID, InstanceID, RoleID)
		map.MapCreateCreature(MapID, InstanceID, 1004106, x+2, y, z+2)
	end
end

--注册
aux.RegisterCreatureEvent(3020438, 6, "c3020438_OnInvest")
