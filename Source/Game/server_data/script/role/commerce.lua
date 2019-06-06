-- 跑商


--完成跑商
function Commerce_OnCompleteCommerce(MapID, InstanceID, RoleID, TaelProgress)
	
	--当缴纳商银数量÷指标商银数量≥125%时，获得1个道具"城市发展铜令"，ID：3300001
	--当缴纳商银数量÷指标商银数量≥150%时，获得1个道具"城市发展银令" ，ID：3300002
	--当缴纳商银数量÷指标商银数量≥175%时，获得1个道具"城市发展金令" ，ID：3300003

	if TaelProgress > 175 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3300003	, 1, -1, 3, 104)
	elseif TaelProgress > 150 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3300002	, 1, -1, 3, 104)		
	elseif TaelProgress > 125 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 3300001	, 1, -1, 3, 104)
	end
end

aux.RegisterRoleEvent(23, "Commerce_OnCompleteCommerce")
