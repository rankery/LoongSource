--三镇妖仙：12806
--需求1：在三处镇妖石附近使用物品解封石叁[2612703]可分别刷新出怪物铄阴魔君[1005510]，玄狐妖1005511]，黑鱼妖王[1005512]
--坐标：
--[m04: 2663, 2435]
--[m04: 2063, 1979]
--[m04: 1811, 2514]

--物品解封石叁使用判断函数
function i2612703_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false

	--不在涿鹿不可使用
	if MapID ~= 3017299663 then
	    bRet = 43
	else
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
        --检测是否在这些点附近
        if x>2647 and x<2679 and z>2419 and z<2451 then
            bRet = 0
        elseif x>2047 and x<2079 and z>1963 and z<1995 then
            bRet = 0
        elseif x>1795 and x<1827 and z>2498 and z<2530 then
            bRet = 0
        else
            bRet = 43
        end
    end

    --返回
	return bRet, bIgnore
end

--物品解封石叁使用效果函数
function i2612703_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if x>2647 and x<2679 and z>2419 and z<2451 then
		map.MapCreateCreature(MapID, InstanceID, 1005510, 2663, 20783, 2435)
	elseif x>2047 and x<2079 and z>1963 and z<1995 then
		map.MapCreateCreature(MapID, InstanceID, 1005511, 2063,13898, 1979)
	elseif x>1795 and x<1827 and z>2498 and z<2530 then
		map.MapCreateCreature(MapID, InstanceID, 1005512, 1811, 12354, 2514)
	end
end

--注册
aux.RegisterItemEvent(2612703, 1, "i2612703_QuestUsable")
aux.RegisterItemEvent(2612703, 0, "i2612703_CanUse")
