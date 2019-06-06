--seed CreatureScriptData
--1，种植玩家ID
--2，浇水或施肥次数
--3，上次浇水或施肥时间

function Seed_3303006_Use(MapID, InstanceID, TypeID, RoleID)
	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local creatureID = map.MapCreateCreature(MapID, InstanceID, 4900408, x, y, z)

	cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 1, RoleID)--记录种植玩家的ID
	cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 2, 0)	--重置植物的浇水次数
	cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 3, 0) --重置植物的浇水间隔
end

aux.RegisterItemEvent(3303006, 1, "Seed_3303006_Use")

function Seed_3303007_Use(MapID, InstanceID, TypeID, RoleID)
	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local creatureID = map.MapCreateCreature(MapID, InstanceID, 4900411, x, y, z)

	cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 1, RoleID)--记录种植玩家的ID
	--scriptdata2,3默认为0

end
aux.RegisterItemEvent(3303007, 1, "Seed_3303007_Use")

function Seed_3303008_Use(MapID, InstanceID, TypeID, RoleID)
	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
	local creatureID = map.MapCreateCreature(MapID, InstanceID, 4900414, x, y, z)

	cre.SetCreatureScriptData(MapID, InstanceID, creatureID, 1, 1, RoleID)--记录种植玩家的ID
	--scriptdata2,3默认为0

end
aux.RegisterItemEvent(3303008, 1, "Seed_3303008_Use")

function Seed_CanUse(MapID, InstanceID, TypeID, TargetID)
	local a = map.IsInArea(3017298127, InstanceID, TargetID, 13285)
	local b = map.IsInArea(3017298383, InstanceID, TargetID, 6861)
	if a == true or b == true then
		return 0,false
	elseif a == false and b == false then
		return 43,false
	end
end
aux.RegisterItemEvent(3303006, 0, "Seed_CanUse")
aux.RegisterItemEvent(3303007, 0, "Seed_CanUse")
aux.RegisterItemEvent(3303008, 0, "Seed_CanUse")
