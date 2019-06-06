
FDGJ_TimerMin = 0  --记录活动进行的时间

FDGJ_KillGhost = 0 --记录活动中被杀掉的冤魂游鬼个数

LingHuo_Pos = {}    --记录刷出灵火的位置坐标
LingHuo_Pos[1] = {x=1138, y=20139, z=822}
LingHuo_Pos[2] = {x=972, y=18885, z=1196}
LingHuo_Pos[3] = {x=963, y=23066, z=1807}
LingHuo_Pos[4] = {x=775, y=25000, z=1656}
LingHuo_Pos[5] = {x=1306, y=26047, z=1593}
LingHuo_Pos[6] = {x=1873, y=17854, z=1938}
LingHuo_Pos[7] = {x=1883, y=16255, z=1773}
LingHuo_Pos[8] = {x=2507, y=19760, z=339}
LingHuo_Pos[9] = {x=1689, y=19637, z=506}
LingHuo_Pos[10] = {x=1883, y=19667, z=1286}

LingHuo_Compensate = {}    --用来记录调查地物的血量是否被多减了1.


function FDGJ_OnStart(actID)
    --初始化活动全局变量
	for k=1, 10 do
		table.remove(LingHuo_Compensate)
	end

	FDGJ_TimerMin = 0

	FDGJ_KillGhost = 0

	--服务器全服通告：“巫水“酆都鬼节”活动已经开始，阴间地府之门大开，请大家寻找出现于巫水场景内的通冥灵火，焚烧冥钱以祭飨鬼神！”
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 100, 718)
	msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)

	--于巫水场景内的策划编辑制定地点刷新出10个地物形态NPC“通冥灵火”
	for i=1, 10 do  --生成怪物
		local CreatureID = map.MapCreateCreature(3017299919, -1, 3021004, LingHuo_Pos[i].x, LingHuo_Pos[i].y, LingHuo_Pos[i].z)
		table.insert(LingHuo_Compensate, CreatureID, 0)
	end
end

function FDGJ_OnTimerMin(actID)

	FDGJ_TimerMin = FDGJ_TimerMin + 1

	if FDGJ_TimerMin == 5 then
	    --服务器全服通告：“巫水“酆都鬼节”活动已经开始，阴间地府之门大开，请大家寻找出现于巫水场景内的通冥灵火，焚烧冥钱以祭飨鬼神！”
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 100, 718)
	    msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)

	elseif FDGJ_TimerMin % 10 == 9 then
		--服务器全服通告：“通冥灵火将在一分钟后刷新”
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 100, 719)
	    msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)

	elseif FDGJ_TimerMin % 10 == 0 and FDGJ_TimerMin ~= 0 and FDGJ_TimerMin ~= 60 then
	    --初始化活动全局变量
	    for k=1, 10 do
		    table.remove(LingHuo_Compensate)
	    end
	    for i=1, 10 do    --于巫水场景内的策划编辑制定地点刷新出10个地物形态NPC“通冥灵火”
			local CreatureID = map.MapCreateCreature(3017299919, -1, 3021004, LingHuo_Pos[i].x, LingHuo_Pos[i].y, LingHuo_Pos[i].z)
			table.insert(LingHuo_Compensate, CreatureID, 0)
	    end
	end
end


--注册该活动的相关事件
aux.RegisterActEvent(65, 2, "FDGJ_OnStart")
aux.RegisterActEvent(66, 2, "FDGJ_OnStart")
aux.RegisterActEvent(67, 2, "FDGJ_OnStart")
aux.RegisterActEvent(68, 2, "FDGJ_OnStart")

aux.RegisterActEvent(65, 4, "FDGJ_OnTimerMin")
aux.RegisterActEvent(66, 4, "FDGJ_OnTimerMin")
aux.RegisterActEvent(67, 4, "FDGJ_OnTimerMin")
aux.RegisterActEvent(68, 4, "FDGJ_OnTimerMin")

--调查通冥灵火
function C3021004_OnInVest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
    --如果玩家低于55级，则调查无效
	local level = role.GetRoleLevel(MapID, InstanceID, RoleID)
	if level < 55 then
	    --如果该灵火的血量满
    	local HP = unit.GetUnitAttValue(MapID, InstanceID, CreatureID, 9)
		if HP == 3 then
		    LingHuo_Compensate[CreatureID] = 1     --记录该地物应该获得血量补偿
		end
		--血量补偿
		unit.ChangeHP(MapID, InstanceID, CreatureID, 1)
	    --提示：“您的等级不足55级，不能参加酆都鬼节活动”
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 720)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)
	else
    	--检测玩家是否有冥钱黄纸，如果有则直接扣除
    	local Role_Have_HuangZhi = role.GetRoleItemNum(RoleID, 2615901)
        --如果没有黄纸
    	if Role_Have_HuangZhi < 1 or Role_Have_HuangZhi == nil then
    		--如果该灵火的血量满
        	local HP = unit.GetUnitAttValue(MapID, InstanceID, CreatureID, 9)
    		if HP == 3 then
    		    LingHuo_Compensate[CreatureID] = 1     --记录该地物应该获得血量补偿
    		end
    		--血量补偿
    		unit.ChangeHP(MapID, InstanceID, CreatureID, 1)
    		--提示：“您没有冥钱黄纸可烧，请准备好再来。”
    		local MsgID = msg.BeginMsgEvent()
    		msg.AddMsgEvent(MsgID, 26, 721)
    		msg.DispatchRoleMsgEvent(RoleID, MsgID)
        else
	    	--如果身上有可供删除的黄纸
	        if 0 == role.RemoveFromRole(MapID, InstanceID, RoleID, 2615901, 1, 1000) then
	    	    --提示：“您将一张冥钱黄纸丢入火堆之中，瞬间化为灰烬。”
	    	    local MsgID = msg.BeginMsgEvent()
	    	    msg.AddMsgEvent(MsgID, 26, 722)
	    	    msg.DispatchRoleMsgEvent(RoleID, MsgID)

	    	    --给予相应的奖励
	    		local addexp = {}
	    		addexp[11] = 8380
	    		addexp[12] = 9136
	    		addexp[13] = 10116
	     		addexp[14] = 11916
	    		addexp[15] = 13761
	    		addexp[16] = 15641
	    		addexp[17] = 17841
	    		addexp[18] = 19641
	    		addexp[19] = 21044
				addexp[20] = 22044
				addexp[21] = 23044

	    		local rolelevel = role.GetRoleLevel(MapID, InstanceID, RoleID)
	    		local k = math.floor((rolelevel-1)/5)
	    		if rolelevel == 55 then
	    		    k = 11
	    		end
	    		role.AddRoleExp(MapID, InstanceID, RoleID, addexp[k])

		    	--提示：“您获得了XX点经验”
				local MsgID = msg.BeginMsgEvent()
	    	    msg.AddMsgEvent(MsgID, 26, 723)
	    		msg.AddMsgEvent(MsgID, 9, addexp[k])
	     	    msg.DispatchRoleMsgEvent(RoleID, MsgID)

		    	--根据判断给予血量补偿
	            local HP = unit.GetUnitAttValue(MapID, InstanceID, CreatureID, 9)
	    	    if HP == 2 then
		            if LingHuo_Compensate[CreatureID] == 1 then       --判断该地物应该获得血量补偿
			            --血量补偿
		                unit.ChangeHP(MapID, InstanceID, CreatureID, 1)
			    		LingHuo_Compensate[CreatureID] = 0     --清空血量补偿位
					end
				end
		    else
		        --如果该灵火的血量满
				local HP = unit.GetUnitAttValue(MapID, InstanceID, CreatureID, 9)
		        if HP == 3 then
		            LingHuo_Compensate[CreatureID] = 1     --记录该地物应该获得血量补偿
		        end
		        --血量补偿
		        unit.ChangeHP(MapID, InstanceID, CreatureID, 1)
		        --提示：“您没有冥钱黄纸可烧，请准备好再来。”
		        local MsgID = msg.BeginMsgEvent()
		        msg.AddMsgEvent(MsgID, 26, 721)
		        msg.DispatchRoleMsgEvent(RoleID, MsgID)
		    end
	    end
	end
end
--注册调查通冥灵火事件
aux.RegisterCreatureEvent(3021004, 6, "C3021004_OnInVest")

--通冥灵火被调查消失后刷出3个冤魂游鬼1532041
function c3021004_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    local a, b, c = unit.GetPosition(MapID, InstanceID, TargetID)  --获取灵火的坐标
	map.MapCreateColCreature(MapID, InstanceID, 1532041, a, b, c, 1, "")
	map.MapCreateColCreature(MapID, InstanceID, 1532041, a, b, c, 1, "")
	map.MapCreateColCreature(MapID, InstanceID, 1532041, a, b, c, 1, "")
end
aux.RegisterCreatureEvent(3021004, 4, "c3021004_OnDie")

--杀掉150个冤魂游鬼可以刷出无间修罗1532042
function c1532041_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    FDGJ_KillGhost = FDGJ_KillGhost + 1
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, 726)    --已经击杀冤魂游鬼XX个。达到150个以后无间修罗将会出现
	msg.AddMsgEvent(MsgID, 9, FDGJ_KillGhost)
	msg.DispatchMapMsgEvent(MsgID, MapID, -1)
	if FDGJ_KillGhost == 150 then
        map.MapCreateCreature(MapID, InstanceID, 1532042, 977, 20140, 979)
    	--服务器全服通告：“阴间亡魂被超度者甚众，无间修罗已被惊动，亲自降临巫水拘魂锁魄，请各路英雄速速前往降服！”
	    local MsgID = msg.BeginMsgEvent()
	    msg.AddMsgEvent(MsgID, 100, 724)
	    msg.DispatchMapMsgEvent(MsgID, 3017299919, -1)
    end
end
aux.RegisterCreatureEvent(1532041, 4, "c1532041_OnDie")

--杀掉无间修罗，全服通告“####（对无间修罗完成最后一击的玩家角色名）英勇无双，降服无间修罗，实乃天下英雄之楷模！”
function c1532042_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
    --通知全服
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 13, 1)
	msg.AddMsgEvent(MsgID, 1, 725)
	msg.AddMsgEvent(MsgID, 2, RoleID)
	msg.DispatchWorldMsgEvent(MsgID)

	--local MsgID = msg.BeginMsgEvent()
	--msg.AddMsgEvent(MsgID, 100, )
	--msg.AddMsgEvent(MsgID, 2, RoleID)
	--msg.DispatchWorldMsgEvent(MsgID)
end
aux.RegisterCreatureEvent(1532042, 4, "c1532042_OnDie")

-- 冤魂游鬼进入战斗喊话
function c1532041_OnEnterCombat(MapID, InstanceID, CreatureID)
	local temp = math.random(1,100)
	if temp>=1 and temp<=50 then
		--50%几率喊“谁能救我脱离苦海！”
		cre.MonsterSay(MapID, InstanceID, CreatureID, 30140)
	end
end

aux.RegisterCreatureEvent(1532041, 2, "c1532041_OnEnterCombat")

-- 无间修罗进入战斗喊话
function c1532042_OnEnterCombat(MapID, InstanceID, CreatureID)
    --喊“顺我者昌，逆我者亡！”
	cre.MonsterSay(MapID, InstanceID, CreatureID, 30141)
end

aux.RegisterCreatureEvent(1532042, 2, "c1532042_OnEnterCombat")
