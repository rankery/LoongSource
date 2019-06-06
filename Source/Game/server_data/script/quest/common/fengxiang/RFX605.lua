--小心陷阱！：11605
--需求1：调查地物陷阱位置[3020320]时50%几率获得BUFF侵蚀[2011601]

--地物陷阱被调查时脚本函数
function c3020320_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local Rate = math.random(1, 100)
	if Rate <= 50 then
		unit.AddBuff(mapID, InstanceID, RoleID, 2011601, RoleID)
	end
end

--注册
aux.RegisterCreatureEvent(3020320, 6, "c3020320_OnInvest")
