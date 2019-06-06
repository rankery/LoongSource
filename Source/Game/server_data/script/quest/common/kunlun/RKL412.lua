--15412使用道具致邪头骨2615207可在雪瘟寨召唤BOSS雪瘟神（1009234）x1330 y17622 z881

--可否使用函数
function i2615207_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false
	--如果不在昆仑，则不能使用
	if MapID ~= 3017299407 then
	    return 43, false
	else
	--如果不在指定地点，则不能使用
	    local x,y,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x < 1230 or x > 1430 or z < 781 or z > 981 then
	          return 43, false 
            end
	end
    return bRet, bIgnore
end

--使用效果
function i2615207_QuestUsable(MapID, InstanceID, TypeID, TargetID)
       map.MapCreateCreature(MapID, InstanceID, 1009234, 1330, 17622, 881)
end

--注册
aux.RegisterItemEvent(2615207, 1, "i2615207_QuestUsable")
aux.RegisterItemEvent(2615207, 0, "i2615207_CanUse")