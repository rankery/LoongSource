--[[ 小丹炉使用后获得物品脚本

2612803	金丹	0.5	996-1000
2612802	太极石	0.5	991-995
1220008	[鉴]捌级灵鉴	1	981-990
1100004	[食]精制肉包	3	951-980
1210001	[强]粗糙磨石	3	921-950
2004001	[宝]血晶石原石	3	891-920
2004002	[宝]气玄石原石	3	861-890
2004003	[宝]太阳石原石	3	831-860
2004004	[宝]月光石原石	3	801-830
2004007	[宝]雷鸣石原石	3	771-800
2004008	[宝]风吟石原石	3	741-770
2612801	阴阳石	5	691-740
1000009	[药]七宝活命丹	5	641-690
1000109	[药]七经玄清散	5	591-640
2612805	变形记单页	6	531-590
2612806	仙人掌	7	461-530
2612808	精益血丸	7	391-460
2612809	精益气丸	7	321-390
2612810	蓄势灵丹	7	251-320
2612807	一品绝强活力丸	10	151-250
2003002	枯草	15	1-150
]]
function I2612804_OnUse(MapID, InstanceID, TypeID, TargetID)
	local r = math.random(1,1000)

	if r >= 996 then
		--2612803	金丹	5	996-1000
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612803, 1, -1, 8, 420)
	elseif r >= 991 then
		--2612802	太极石	5	991-995
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612802, 1, -1, 8, 420)
	elseif r >= 981 then
		--1220008	[鉴]捌级灵鉴	10	981-990
		role.AddRoleItem(MapID, InstanceID, TargetID, 1220008, 1, -1, 8, 420)
	elseif r >= 951 then
		--1100004	[食]精制肉包	30	951-980
		role.AddRoleItem(MapID, InstanceID, TargetID, 1100004, 1, -1, 8, 420)
	elseif r >= 921 then
		--1210001	[强]粗糙磨石	30	921-950
		role.AddRoleItem(MapID, InstanceID, TargetID, 1210001, 1, -1, 8, 420)
	elseif r >= 891 then
		--2004001	[宝]血晶石原石	30	891-920
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004001, 1, -1, 8, 420)
	elseif r >= 861 then
		--2004002	[宝]气玄石原石	30	861-890
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004002, 1, -1, 8, 420)
	elseif r >= 831 then
		--2004003	[宝]太阳石原石	30	831-860
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004003, 1, -1, 8, 420)
	elseif r >= 801 then
		--2004004	[宝]月光石原石	30	801-830
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004004, 1, -1, 8, 420)
	elseif r >= 771 then
		--2004007	[宝]雷鸣石原石	30	771-800
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004007, 1, -1, 8, 420)
	elseif r >= 741 then
		--2004008	[宝]风吟石原石	30	741-770
		role.AddRoleItem(MapID, InstanceID, TargetID, 2004008, 1, -1, 8, 420)
	elseif r >= 691 then
		--2612801	阴阳石	50	691-740
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612801, 1, -1, 8, 420)
	elseif r >= 641 then
		--1000009	[药]七宝活命丹	50	641-690
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000009, 1, -1, 8, 420)
	elseif r >= 591 then
		--1000109	[药]七经玄清散	50	591-640
		role.AddRoleItem(MapID, InstanceID, TargetID, 1000109, 1, -1, 8, 420)
	elseif r >= 531 then
		--2612805	变形记单页	60	531-590
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612805, 1, -1, 8, 420)
	elseif r >= 461 then
		--2612806	仙人掌	70	461-530
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612806, 1, -1, 8, 420)
	elseif r >= 391 then
		--2612808	精益血丸	70	391-460
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612808, 1, -1, 8, 420)
	elseif r >= 321 then
		--2612809	精益气丸	70	321-390
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612809, 1, -1, 8, 420)
	elseif r >= 251 then
		--2612810	蓄势灵丹	70	251-320
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612810, 1, -1, 8, 420)
	elseif r >= 151 then
		--2612807	一品绝强活力丸	100	151-250
		role.AddRoleItem(MapID, InstanceID, TargetID, 2612807, 1, -1, 8, 420)
	elseif r >= 1 then
		--2003002	枯草	150	1-150
		role.AddRoleItem(MapID, InstanceID, TargetID, 2003002, 1, -1, 8, 420)
	end
end

function I2612804_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(TargetID)
	if(FreeSize < 1) then
		--提示玩家背包空间不足
		bRet = 40
		return bRet, bIgnore
	end
end

aux.RegisterItemEvent(2612804, 1, "I2612804_OnUse")
aux.RegisterItemEvent(2612804, 0, "I2612804_CanUse")
