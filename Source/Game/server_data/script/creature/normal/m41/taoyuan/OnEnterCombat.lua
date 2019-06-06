-- 怪物进入战斗触发示警
function taoyuan_xiaoguai_OnEnterCombat(MapID, InstanceID, CreatureID)
    --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为5个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008001, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008004, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008006, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008016, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008017, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008018, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008020, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008023, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008024, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008025, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008026, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008027, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008028, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008030, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008031, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008035, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008037, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008038, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008040, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008041, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008048, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008049, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008050, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008051, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008052, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008053, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008054, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008055, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008056, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008057, 2, "taoyuan_xiaoguai_OnEnterCombat")
aux.RegisterCreatureEvent(1008091, 2, "taoyuan_xiaoguai_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008003_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
		cre.MonsterSay(MapID, InstanceID, CreatureID, 40023)
	end
end
aux.RegisterCreatureEvent(1008003, 2, "c1008003_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008019_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40027)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008019, 2, "c1008019_OnEnterCombat")


-- 怪物进入战斗喊话
function c1008021_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40029)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008021, 2, "c1008021_OnEnterCombat")

--怪物喊话脚本/示警脚本

-- 怪物进入战斗喊话
function c1008022_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40030)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008022, 2, "c1008022_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008029_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40033)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008029, 2, "c1008029_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008032_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40034)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008032, 2, "c1008032_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008033_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40035)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008033, 2, "c1008033_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008034_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40036)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008034, 2, "c1008034_OnEnterCombat")

-- 怪物进入战斗喊话
function c1008036_OnEnterCombat(MapID, InstanceID, CreatureID)
       local temp = math.random(1,100)
	if temp>=1 and temp<=15 then
		--15%几率喊话
             cre.MonsterSay(MapID, InstanceID, CreatureID, 40038)
       end
	       --得到怪物的当前目标
    local TargetID = cre.GetCreatureCurTargetID(MapID, InstanceID, CreatureID)
    --同步仇恨给范围为10个格子，高度为20的友方怪物
    g_SynCreatureEnmity(MapID, InstanceID, CreatureID, TargetID, 1, 10, 20)
end

aux.RegisterCreatureEvent(1008036, 2, "c1008036_OnEnterCombat")
