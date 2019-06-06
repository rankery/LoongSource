-- 怪物AI测试

-- 怪物的脚本数据
-- 0——怪物进入喊叫的tick
-- 1——怪物是否喊叫完成

-- 怪物进入战斗函数
function x1003001_OnEnterCombat(mapid, instanceid, creatureid)
	cre.SetCreatureAITimer(mapid, instanceid, creatureid, 5)
	cre.SetCreatureScriptData(mapid, instanceid, creatureid, 2, 0, 0, 1, 0)
end

-- 怪物离开战斗函数
function x1003001_OnLeaveCombat(mapid, instanceid, creatureid)
	cre.SetCreatureAITimer(mapid, instanceid, creatureid, -1)
end

-- 怪物AI状态更新函数
function x1003001_OnUpdateAI(mapid, instanceid, creatureid, curstate)

	if 11 == curstate then	-- 处于喊叫状态下
		-- 得到当前服务器心跳和怪物开始喊话的心跳
		local starttick = cre.GetCreatureScriptData(mapid, instanceid, creatureid, 1, 0)	--得到开始吼叫的tick
		local worldtick = aux.GetWorldTick()

		-- 如果喊话时间超过了3秒，则吼叫状态结束
		if nil ~= starttick and worldtick - starttick >= 20 then
			cre.CreatureChangeAIState(mapid, instanceid, creatureid, 1)	--切换到追击状态
		end

	else					-- 不处于喊叫状态下
		-- 如果怪物还没有进入过喊叫状态
		local entered = cre.GetCreatureScriptData(mapid, instanceid, creatureid, 1, 1)

		if 0 == entered then
			-- 得到怪物进入战斗的服务器心跳和当前服务器心跳
			local entercombattick = cre.GetEnterCombatTick(mapid, instanceid, creatureid)
			local worldtick = aux.GetWorldTick()

			-- 如果进入战斗大于1秒了，则切换到吼叫状态
			if nil ~= entercombattick and worldtick - entercombattick >= 1 then
				cre.CreatureChangeAIState(mapid, instanceid, creatureid, 11)	--切换到喊叫状态
			end
		end
	end

end

-- 怪物进入AI状态函数
function x1003001_OnEnterCurAI(mapid, instanceid, creatureid, curstate)
	local ai_state_table = x1003001_AITable[curstate]

	if not ai_state_table then
		return
	end

	local fn = ai_state_table[1]

	if not fn then
		return
	end

	-- 调用函数
	fn(mapid, instanceid, creatureid)
end

-- 怪物离开AI状态函数
function x1003001_OnLeaveCurAI(mapid, instanceid, creatureid, curstate)
	local ai_state_table = x1003001_AITable[curstate]

	if not ai_state_table then
		return
	end

	local fn = ai_state_table[2]

	if not fn then
		return
	end

	-- 调用函数
	fn(mapid, instanceid, creatureid)
end

-- 怪物更新AI状态函数
function x1003001_OnUpdateCurAI(mapid, instanceid, creatureid, curstate)
	local ai_state_table = x1003001_AITable[curstate]

	if not ai_state_table then
		return
	end

	local fn = ai_state_table[3]

	if not fn then
		return
	end

	-- 调用函数
	fn(mapid, instanceid, creatureid)
end

-- 怪物吼叫状态进入函数
local function x1003001_OnEnterCurAI_Shout(mapid, instanceid, creatureid)
	cre.SetCreatureScriptData(mapid, instanceid, creatureid, 2, 0, aux.GetWorldTick(), 1, 1)

	unit.StopAttack(mapid, instanceid, creatureid)

	cre.MonsterSay(mapid, instanceid, creatureid, 3)
end

-- 怪物吼叫状态更新函数
local function x1003001_OnUpdateCurAI_Shout(mapid, instanceid, creatureid)
	local starttick = cre.GetCreatureScriptData(mapid, instanceid, creatureid, 1, 0)	--得到开始吼叫的tick
	local worldtick = aux.GetWorldTick()

	local x, y, z = unit.GetPosition(mapid, instanceid, creatureid)
	local distance = math.pow(x-2200,2) + math.pow(z-2200,2)

	if distance<=100 then
		unit.StopMove(mapid, instanceid, creatureid, 1)
		cre.MonsterPlayerAction(mapid, instanceid, creatureid, "dfly")
	else
		cre.PullCreatureToPos(mapid, instanceid, creatureid, 2200, 6646, 2200)
	end
end

-- 怪物吼叫状态退出函数
local function x1003001_OnLeaveCurAI_Shout(mapid, instanceid, creatureid)
	unit.AddBuff(mapid, instanceid, creatureid, 9500601)
end

-- 怪物AI表
x1003001_AITable = {[11] = {x1003001_OnEnterCurAI_Shout, x1003001_OnLeaveCurAI_Shout, x1003001_OnUpdateCurAI_Shout}}

aux.RegisterCreatureEvent(1003001, 2, "x1003001_OnEnterCombat")
aux.RegisterCreatureEvent(1003001, 3, "x1003001_OnLeaveCombat")
aux.RegisterCreatureEvent(1003001, 8, "x1003001_OnUpdateAI")
aux.RegisterCreatureEvent(1003001, 9, "x1003001_OnUpdateCurAI")
aux.RegisterCreatureEvent(1003001, 10, "x1003001_OnEnterCurAI")
aux.RegisterCreatureEvent(1003001, 11, "x1003001_OnLeaveCurAI")
