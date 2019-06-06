--[[任务ID：14508
使用物品刷出敌人
物品ID：2614257
敌人ID：1007229
坐标：须弥台灵狐巢穴，943，13799，1214（格子坐标）]]


--物品使用效果脚本函数
function I2614258_QuestUsable(MapID, InstanceID, TypeID, TargetID)
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 1007229, x+2, y, z+2)
end

--物品使用判断脚本函数
function I2614258_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	--必须在凤翔使用
	if MapID ~= 1146339967 then
	    bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
        if x<=943-20 or x>=943+20 or z<=1214-20 or z>=1214+20 then  --必须在该范围内使用
            bRet = 43
        end
    end

	--返回
	return bRet, bIgnore
end

--注册
aux.RegisterItemEvent(2614258, 1, "I2614258_QuestUsable")
aux.RegisterItemEvent(2614258, 0, "I2614258_CanUse")
