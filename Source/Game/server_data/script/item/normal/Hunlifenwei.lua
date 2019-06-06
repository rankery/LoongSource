--------------
--1361113	»éÀñÇìµä»¨Àº
--1361114	»éÀñÇìµä»¨Àº
--1361115	»éÀñÆÁ·ç
--1361116	»éÀñ´óºìµÆÁý
--1361117	°ÙÄêºÃºÏ»¨Êø
--1361118	ð½·ïºÍÃù»¨Êø
--1361119	Ï²½áÁ¼Ôµ»¨Êø
--1361120	Õä°®Ãµ¹å»¨Êø
--1361121	Ï²Çì»¨Æ¿
--1361122	Ï²Çì»¨Æ¿
--------------
--4900428	»¨Àº
--4900429	»¨Àº
--4900430	ÆÁ·ç
--4900431	ºìµÆÁý
--4900432	Ò»Êø»¨
--4900433	Ò»Êø»¨
--4900434	Ò»Êø»¨
--4900435	Ãµ¹å»¨Êø
--4900436	»¨Æ¿
--4900437	»¨Æ¿


function I1361113_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900428, x+2, y, z+2)
end

aux.RegisterItemEvent(1361113, 1, "I1361113_QuestUsable")


function I1361114_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900429, x+2, y, z+2)
end

aux.RegisterItemEvent(1361114, 1, "I1361114_QuestUsable")

function I1361115_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900430, x+2, y, z+2)
end

aux.RegisterItemEvent(1361115, 1, "I1361115_QuestUsable")

function I1361116_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900431, x+2, y, z+2)
end

aux.RegisterItemEvent(1361116, 1, "I1361116_QuestUsable")

function I1361117_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900432, x+2, y, z+2)
end

aux.RegisterItemEvent(1361117, 1, "I1361117_QuestUsable")

function I1361118_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900433, x+2, y, z+2)
end

aux.RegisterItemEvent(1361118, 1, "I1361118_QuestUsable")

function I1361119_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900434, x+2, y, z+2)
end

aux.RegisterItemEvent(1361119, 1, "I1361119_QuestUsable")

function I1361120_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900435, x+2, y, z+2)
end

aux.RegisterItemEvent(1361120, 1, "I1361120_QuestUsable")

function I1361121_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900436, x+2, y, z+2)
end

aux.RegisterItemEvent(1361121, 1, "I1361121_QuestUsable")

function I1361122_QuestUsable(MapID, InstanceID, TypeID, TargetID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
	local a = map.MapCreateCreature(MapID, InstanceID, 4900437, x+2, y, z+2)
end

aux.RegisterItemEvent(1361122, 1, "I1361122_QuestUsable")




