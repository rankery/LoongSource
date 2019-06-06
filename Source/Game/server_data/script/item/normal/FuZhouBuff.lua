--[[
1340005	乱战符
buffid
3308101	物品	物品		乱战
3308201	物品	物品		乱战
3308301	物品	物品		乱战
3308401	物品	物品		乱战
3308501	物品	物品		乱战
3308601	物品	物品		乱战

1340011	请神符
buffid
3309701	物品	物品		请神
3309801	物品	物品		请神
3309901	物品	物品		请神
3310001	物品	物品		请神
3310101	物品	物品		请神
3310201	物品	物品		请神

1340012	大请神符
buffid
3310301	物品	物品		大请神
3310401	物品	物品		大请神
3310501	物品	物品		大请神
3310601	物品	物品		大请神
3310701	物品	物品		大请神
3310801	物品	物品		大请神

30%，25%，20%，15%，5%，5%

1340006	强战符
buffid
3308701	物品	物品		强战
3308801	物品	物品		强战
3308901	物品	物品		强战
3309001	物品	物品		强战
3309101	物品	物品		强战
3309201	物品	物品		强战

20%，20%，15%，15%, 15%，15%

]]


--使用效果
function FuZhouBuff_use(MapID, InstanceID, TypeID, TargetID)

    local BUFF = {}
	BUFF[1340005] = {3308101, 3308201, 3308301, 3308401, 3308501, 3308601}
	BUFF[1340006] = {3308701, 3308801, 3308901, 3309001, 3309101, 3309201}
	BUFF[1340011] = {3309701, 3309801, 3309901, 3310001, 3310101, 3310201}
	BUFF[1340012] = {3310301, 3310401, 3310501, 3310601, 3310701, 3310801}
	BUFF[1340016] = {3308101, 3308201, 3308301, 3308401, 3308501, 3308601}
	BUFF[1340017] = {3308701, 3308801, 3308901, 3309001, 3309101, 3309201}
	local i = math.random(1,100)
	if TypeID ~= 1340006 and TypeID ~= 1340017 then
	    if i <= 30 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][1], TargetID)
	    elseif i <= 55 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][2], TargetID)
	    elseif i <= 75 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][3], TargetID)
	    elseif i <= 90 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][4], TargetID)
	    elseif i <= 95 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][5], TargetID)
	    elseif i <= 100 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][6], TargetID)
	    end
	else
	    if i <= 20 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][1], TargetID)
	    elseif i <= 40 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][2], TargetID)
	    elseif i <= 55 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][3], TargetID)
	    elseif i <= 70 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][4], TargetID)
	    elseif i <= 85 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][5], TargetID)
	    elseif i <= 100 then
	        unit.AddBuff(MapID, InstanceID, TargetID, BUFF[TypeID][6], TargetID)
		end
	end
end

--注册
aux.RegisterItemEvent(1340005, 1, "FuZhouBuff_use")
aux.RegisterItemEvent(1340006, 1, "FuZhouBuff_use")
aux.RegisterItemEvent(1340011, 1, "FuZhouBuff_use")
aux.RegisterItemEvent(1340012, 1, "FuZhouBuff_use")
aux.RegisterItemEvent(1340016, 1, "FuZhouBuff_use")
aux.RegisterItemEvent(1340017, 1, "FuZhouBuff_use")

