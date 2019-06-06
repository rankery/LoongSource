--·击杀 鬼叉骠骑 1008073 有10%几率触发下面任意一个脚本：
--刷出 濂虐魔帅 1008086 坐标：x 2412 y 26387 z 707 ，场景内所有 鬼叉骠骑 1008073 消失
--刷出 相黜魔帅 1008087 坐标：x 2817 y 26387 z 708 ，场景内所有 鬼叉骠骑 1008073 消失
--刷出 鬼叉魔帅 1008088 坐标：x 2795 y 26387 z 310 ，场景内所有 鬼叉骠骑 1008073 消失
--刷出 贪金近侍 1008078 坐标：x 2436 y 26387 z 355
--刷出 贪金近侍 1008079 坐标：x 2377 y 26387 z 741
--刷出 贪金近侍 1008080 坐标：x 2840 y 26387 z 298
--有1%几率触发下面脚本：
--刷出 干戍奇煞狂魔君 1008081 坐标：x 2600 y 25797 z 447 ，场景内所有 鬼叉骠骑 1008073 消失

function c1008073_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local k = math.random(1,100)
	local i = math.random(1,6)
	if k > 90 then
		if i == 1 then
			map.MapCreateCreature(MapID, InstanceID, 1008086, 2412, 26387, 707)
			--map.MapDeleteCreature(MapID, InstanceID, 1008073)
		elseif i == 2 then
			map.MapCreateCreature(MapID, InstanceID, 1008087, 2817, 26387, 708)
			--map.MapDeleteCreature(MapID, InstanceID, 1008073)
		elseif i == 3 then
			map.MapCreateCreature(MapID, InstanceID, 1008088, 2795, 26387, 310)
			--map.MapDeleteCreature(MapID, InstanceID, 1008073)
		elseif i == 4 then
			map.MapCreateCreature(MapID, InstanceID, 1008078, 2436, 26387, 355)
		elseif i == 5 then
			map.MapCreateCreature(MapID, InstanceID, 1008079, 2377, 26387, 741)
		elseif i == 6 then
			map.MapCreateCreature(MapID, InstanceID, 1008080, 2840, 26387, 298)
		end
	elseif k == 1 then
			map.MapCreateCreature(MapID, InstanceID, 1008081, 2600, 25797, 447)
			map.MapDeleteCreature(MapID, InstanceID, 1008073)
	end
end
aux.RegisterCreatureEvent(1008073, 4, "c1008073_OnDie")

-- 怪物进入战斗触发示警
function c1008073_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008073, 2, "c1008073_OnEnterCombat")
