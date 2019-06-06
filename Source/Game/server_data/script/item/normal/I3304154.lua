
--使用道具：端午节的祝福，给自身添加 各类BUFF

function I3304154_OnUse(MapID, InstanceID, TypeID, TargetID)

local k = math.random(1,5)
	if k == 1 then
	unit.AddBuff(MapID, InstanceID, TargetID, 3317801, TargetID)
	elseif k == 2 then
	unit.AddBuff(MapID, InstanceID, TargetID, 3317901, TargetID)
	elseif k == 3 then
	unit.AddBuff(MapID, InstanceID, TargetID, 3318001, TargetID)
	elseif k == 4 then
	unit.AddBuff(MapID, InstanceID, TargetID, 3318101, TargetID)
	else
	unit.AddBuff(MapID, InstanceID, TargetID, 3318201, TargetID)
	end
end
aux.RegisterItemEvent(3304154, 1, "I3304154_OnUse")


























