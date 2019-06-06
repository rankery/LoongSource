--ScriptData第0位记录大门7282是否打开
--ScriptData第1位记录大门7279是否打开
--ScriptData第2-8位为祭魔石相关
--9，10为杀怪数量记录
--触发脚本区域记录
--触发器记录
--11，12，13用于触发脚本区域记录
--14用于记录奖励领取的人数
--击杀无妄之壁（1010304）刷出场景传送石（3075111）X=399 Y=5664 Z=379

--一层大门（7282）触发器（7291）钥匙（2615601）
--三层大门（7279）触发器（7289）钥匙（2615605）
function m43_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)

	local TriggerSet = map.GetMapScriptData(MapID, InstanceID, 1, 12)
	local n_trigger = {}

	for i=1,63 do
		n_trigger[i] = math.floor(TriggerSet/math.pow(2,i-1)) % 2
	end

	if ObjID == 7291 then
		local BOOL_EnterTrigger7291 = map.GetMapScriptData(MapID,InstanceID,1,0)
		if BOOL_EnterTrigger7291 == 0 then --判断是否被触发过
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 525)	-- "你必须拥有钥匙才能通过此门。"
			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
		end
--	elseif ObjID == 7289 then
--		local BOOL_EnterTrigger7289 = map.GetMapScriptData(MapID,InstanceID,1,1)
--		if BOOL_EnterTrigger7289 == 0 then --判断是否被触发过
--			local MsgID = msg.BeginMsgEvent()
--			msg.AddMsgEvent(MsgID, 26, 525)	-- "你必须拥有钥匙才能通过此门。"
--			msg.DispatchRoleMsgEvent(RoleID, MsgID)     --发送消息
--		end
	elseif	ObjID==7338 and n_trigger[1]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010213, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,0))
	elseif ObjID==7339 and n_trigger[2]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010213, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,1))
	elseif ObjID==7340 and n_trigger[3]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010213, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,2))
	elseif ObjID==7341 and n_trigger[4]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010213, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,3))
	elseif ObjID==7374 and n_trigger[5]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010213, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,4))
	elseif ObjID==7512 and n_trigger[6]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010254, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,5))
	elseif ObjID==7513 and n_trigger[7]==0 then
		map.MapCreateColCreature(MapID, InstanceID, 1010254, x, y, z, 1)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,6))
	elseif ObjID==7646 and n_trigger[8]==0 then
		CMCreateCreature(MapID, InstanceID, 1010254, 27275, 3880, 8386)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,7))
	elseif ObjID==7738 and n_trigger[9]==0 then
		CMCreateCreature(MapID, InstanceID, 1010311, 24850, 5845, 8593)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,8))
	elseif ObjID==7739 and n_trigger[10]==0 then
		CMCreateCreature(MapID, InstanceID, 1010311, 26769, 5845, 8273)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,9))
	elseif ObjID==7740 and n_trigger[11]==0 then
		CMCreateCreature(MapID, InstanceID, 1010311, 28750, 5845, 8439)
		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet+math.pow(2,10))
	end

end
aux.RegisterMapEvent("m43",6,"m43_OnEnterTrigger")

--可否使用函数
function i2615601_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在天墉地宫，则不能使用
	if MapID ~= 3084407247 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x <= 499 or x >= 534 or z <= 379 or z >= 395 or y <= 1900 or y >= 1930 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--钥匙使用效果
function i2615601_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.SetMapScriptData(MapID,InstanceID, 1, 0, 1)--记录已经触发过7291
	map.OpenCloseDoor(MapID, InstanceID, 7282, 1)
end

--注册
aux.RegisterItemEvent(2615601, 1, "i2615601_QuestUsable")
aux.RegisterItemEvent(2615601, 0, "i2615601_CanUse")

--可否使用函数
function i2615605_CanUse(MapID, InstanceID, TypeID, TargetID)
    local bRet, bIgnore = 0, false

	--如果不在天墉地宫，则不能使用
	if MapID ~= 3084407247 then
	    bRet = 43
	else
	    --得到玩家当前坐标
	    local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	    if x <= 538 or x >= 574 or z <= 253 or z >= 269 or y <= 5830 or y >= 5860 then  --如果不在这个坐标范围，则不能使用
	        bRet = 43
	    end
	end

	--返回
	return bRet, bIgnore
end

--钥匙使用效果
function i2615605_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	map.SetMapScriptData(MapID,InstanceID,1,1,1)--记录已经触发过7289
	map.OpenCloseDoor(MapID, InstanceID, 7279, 1)
end

--注册
aux.RegisterItemEvent(2615605, 1, "i2615605_QuestUsable")
aux.RegisterItemEvent(2615605, 0, "i2615605_CanUse")

function m43_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	end

	local TriggerSet2 = map.GetMapScriptData(MapID, InstanceID, 1, 12)
	local n_trigger2 = {}
	n_trigger2[17] = math.floor(TriggerSet2/math.pow(2,17-1)) % 2

	if n_trigger2[17] == 0 then
		local temp = math.random(3)
		if temp==1 then
			CMCreateCreature(MapID, InstanceID, 3075107, 18448, 1914, 16197)
		elseif temp==2 then
			CMCreateCreature(MapID, InstanceID, 3075108, 18448, 1914, 16197)
		elseif temp==3 then
			CMCreateCreature(MapID, InstanceID, 3075109, 18448, 1914, 16197)
		end

		local temp = math.random(2)
		if temp==1 then
			CMCreateCreature(MapID, InstanceID, 1010202, 27716, 1914, 31909)
		elseif temp==2 then
			CMCreateCreature(MapID, InstanceID, 1010202, 13980, 1914, 25417)
		end

		local temp = math.random(2)
		if temp==1 then
			CMCreateCreature(MapID, InstanceID, 1010301, 16692, 1914, 8516)
		elseif temp==2 then
			CMCreateCreature(MapID, InstanceID, 1010301, 23837, 1914, 12265)
		end

		local temp = math.random(2)
		if temp==1 then
			CMCreateCreature(MapID, InstanceID, 1010252, 20302, 3880, 8377)
		elseif temp==2 then
			CMCreateCreature(MapID, InstanceID, 1010252, 21750, 3880, 31630)
		end

		map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,16))
	end
end

--注册
aux.RegisterMapEvent("m43", 2, "m43_OnPlayerEnter")

function m43_OnRoleEnterArea(MapID, InstanceID, RoleID, ObjAreaID)

	local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)

	local TriggerSet = map.GetMapScriptData(MapID, InstanceID, 1, 11)
	local n_trigger = {}
	for i=1,63 do
		n_trigger[i] = math.floor(TriggerSet/math.pow(2,i-1)) % 2
	end

	local TriggerSet2 = map.GetMapScriptData(MapID, InstanceID, 1, 12)
	local n_trigger2 = {}
	for i=1,63 do
		n_trigger2[i] = math.floor(TriggerSet2/math.pow(2,i-1)) % 2
	end

	local TriggerSet3 = map.GetMapScriptData(MapID, InstanceID, 1, 13)
	local n_trigger3 = {}
	for i=1,63 do
		n_trigger3[i] = math.floor(TriggerSet3/math.pow(2,i-1)) % 2
	end

	if ObjAreaID==7323 and n_trigger[1]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 16195, 1914, 26079)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,0))
	elseif ObjAreaID==7328 and n_trigger[2]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 23845, 1914, 27931)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,1))
	elseif ObjAreaID==7446 and n_trigger[3]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 27904, 1914, 24037)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,2))
	elseif ObjAreaID==7335 and n_trigger[4]==0 then	CMCreateCreature(MapID, InstanceID, 3075113, 29968, 1914, 30600)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,3))
	elseif ObjAreaID==7365 and n_trigger[5]==0 then	CMCreateCreature(MapID, InstanceID, 3075113, 17938, 1914, 28266)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,4))
	elseif ObjAreaID==7364 and n_trigger[6]==0 then	CMCreateCreature(MapID, InstanceID, 1010203, 14094, 1914, 29280)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,5))
	elseif ObjAreaID==7335 and n_trigger[7]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 12122, 1914, 8368)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,6))
	elseif ObjAreaID==7417 and n_trigger[8]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 20684, 1914, 8388)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,7))
	elseif ObjAreaID==7421 and n_trigger[9]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 31742, 1914, 12861)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,8))
	elseif ObjAreaID==7423 and n_trigger[10]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 29538, 1914, 10597)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,9))
	elseif ObjAreaID==7426 and n_trigger[11]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 24014, 1914, 14322)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,10))
	elseif ObjAreaID==7431 and n_trigger[12]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 25933, 1914, 16715)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,11))
	elseif ObjAreaID==7432 and n_trigger[13]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 17242, 1914, 12306)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,12))
	elseif ObjAreaID==7435 and n_trigger[14]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 12136, 1914, 12399)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,13))
	elseif ObjAreaID==7427 and n_trigger[15]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 27491, 1914, 16045)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,14))
	elseif ObjAreaID==7439 and n_trigger[16]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 29963, 1914, 25171)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,15))
	elseif ObjAreaID==7437 and n_trigger[17]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 32202, 1914, 25859)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,16))
	elseif ObjAreaID==7438 and n_trigger[18]==0 then	CMCreateCreature(MapID, InstanceID, 1010213, 24322, 1914, 31906)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,17))
	elseif ObjAreaID==7483 and n_trigger[19]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 31753, 3880, 25755)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,18))
	elseif ObjAreaID==7494 and n_trigger[20]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 29838, 3880, 25328)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,19))
	elseif ObjAreaID==7494 and n_trigger[21]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 30221, 3880, 31317)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,20))
	elseif ObjAreaID==7515 and n_trigger[22]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 23215, 3880, 29927)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,21))
	elseif ObjAreaID==7521 and n_trigger[23]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 28031, 3880, 27934)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,22))
	elseif ObjAreaID==7524 and n_trigger[24]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 25893, 3880, 20599)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,23))
	elseif ObjAreaID==7544 and n_trigger[25]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 21912, 3880, 25582)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,24))
	elseif ObjAreaID==7548 and n_trigger[26]==0 then	CMCreateCreature(MapID, InstanceID, 3075113, 24285, 3880, 24228)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,25))
	elseif ObjAreaID==7583 and n_trigger[27]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 10050, 3880, 20358)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,26))
	elseif ObjAreaID==7591 and n_trigger[28]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 9787, 3880, 26098)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,27))
	elseif ObjAreaID==7591 and n_trigger[29]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 11244, 3880, 31911)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,28))
	elseif ObjAreaID==7615 and n_trigger[30]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 8556, 3880, 18774)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,29))
	elseif ObjAreaID==7623 and n_trigger[31]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 16110, 3880, 8253)	map.SetMapScriptData(MapID, InstanceID, 1, 11, TriggerSet+math.pow(2,30))

	elseif ObjAreaID==7630 and n_trigger3[1]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 31739, 3880, 13345)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,0))
	elseif ObjAreaID==7634 and n_trigger3[2]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 31635, 3880, 10446)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,1))
	elseif ObjAreaID==7636 and n_trigger3[3]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 29854, 3880, 12316)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,2))
	elseif ObjAreaID==7657 and n_trigger3[4]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 26862, 3880, 12277)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,3))
	elseif ObjAreaID==7663 and n_trigger3[5]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 23841, 3880, 16295)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,4))
	elseif ObjAreaID==7673 and n_trigger3[6]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 14742, 3880, 12293)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,5))
	elseif ObjAreaID==7680 and n_trigger3[7]==0 then	CMCreateCreature(MapID, InstanceID, 1010254, 12028, 3880, 12386)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,6))
	elseif ObjAreaID==7699 and n_trigger3[8]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 20115, 5845, 8379)	    map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,7))
	elseif ObjAreaID==7711 and n_trigger3[9]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 22899, 5845, 12510)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,8))
	elseif ObjAreaID==7723 and n_trigger3[10]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 23994, 5845, 10380)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,9))
	elseif ObjAreaID==7731 and n_trigger3[11]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 22184, 5845, 8851)	    map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,10))
	elseif ObjAreaID==7741 and n_trigger3[12]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 32196, 5845, 12347)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,11))
	elseif ObjAreaID==7749 and n_trigger3[13]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 31830, 5845, 21682)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,12))
	elseif ObjAreaID==7711 and n_trigger3[14]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 29907, 5845, 18830)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,13))
	elseif ObjAreaID==7759 and n_trigger3[15]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 26037, 5845, 31951)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,14))
	elseif ObjAreaID==7761 and n_trigger3[16]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 26475, 5845, 29987)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,15))
	elseif ObjAreaID==7765 and n_trigger3[17]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 22113, 5845, 29953)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,16))
	elseif ObjAreaID==7767 and n_trigger3[18]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 16583, 5845, 29970)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,17))
	elseif ObjAreaID==7776 and n_trigger3[19]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 15187, 5845, 31919)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,18))
	elseif ObjAreaID==7785 and n_trigger3[20]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 8301, 5845, 30930)	    map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,19))
	elseif ObjAreaID==7790 and n_trigger3[21]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 10656, 5845, 22516)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,20))
	elseif ObjAreaID==7802 and n_trigger3[22]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 8317, 5845, 21025)	    map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,21))
	elseif ObjAreaID==7808 and n_trigger3[23]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 14110, 5845, 12299)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,22))
	elseif ObjAreaID==7813 and n_trigger3[24]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 19962, 5845, 14291)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,23))
	elseif ObjAreaID==7820 and n_trigger3[25]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 25905, 5845, 16973)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,24))
	elseif ObjAreaID==7828 and n_trigger3[26]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 19979, 5845, 21895)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,25))
	elseif ObjAreaID==7830 and n_trigger3[27]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 27891, 5845, 18447)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,26))
	elseif ObjAreaID==7835 and n_trigger3[28]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 25915, 5845, 23728)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,27))
	elseif ObjAreaID==7840 and n_trigger3[29]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 20447, 5845, 26035)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,28))
	elseif ObjAreaID==7847 and n_trigger3[30]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 14179, 5845, 24130)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,29))
	elseif ObjAreaID==7852 and n_trigger3[31]==0 then	CMCreateCreature(MapID, InstanceID, 1010306, 14719, 5845, 17726)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,30))
	elseif ObjAreaID==7854 and n_trigger3[32]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 16102, 5845, 17594)	map.SetMapScriptData(MapID, InstanceID, 1, 13, TriggerSet3+math.pow(2,31))

	--这部分为几率刷，用trigger2
	elseif ObjAreaID==7778 and n_trigger2[12]==0 then	CMCreateCreature(MapID, InstanceID, 3075113, 14632, 5845, 29573)	map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,11))
	elseif ObjAreaID==7800 and n_trigger2[13]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 10210, 5845, 17132)	map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,12))
	elseif ObjAreaID==7803 and n_trigger2[14]==0 then	CMCreateCreature(MapID, InstanceID, 3075113, 7919, 5845, 24566)	map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,13))
	elseif ObjAreaID==7856 and n_trigger2[15]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 16095, 5845, 20190)	map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,14))
	elseif ObjAreaID==7858 and n_trigger2[16]==0 then	CMCreateCreature(MapID, InstanceID, 1010311, 15881, 5845, 22103)	map.SetMapScriptData(MapID, InstanceID, 1, 12, TriggerSet2+math.pow(2,15))


	end

end

aux.RegisterMapEvent("m43", 7, "m43_OnRoleEnterArea")

function CMCreateCreature(MapID, InstanceID, TargetID, x, y, z)

	local x_cm = math.floor(x/50)
	local z_cm = math.floor(z/50)

	map.MapCreateColCreature(MapID, InstanceID, TargetID, x_cm, y, z_cm, 1)

end

--[[
 - 青绝看守		1010203
 - 青绝看守		1010204
 - 青绝弓手		1010208
 - 地宫恶犬		1010212
 - 魍魉戾魔		1010253
 - 地宫厉鬼		1010257
 - 天墉勇士		1010255
 - 天墉看守		1010262
 - 寻脉赤僵		1010305
 - 残滞游巫		1010306
 - 残滞游巫		1010312
3306601		血药
3306701		魔药
3306801		仙豆
3306901		护体药
]]


--调查东极石
function c3075113_OnInvest(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
		local x, y, z = unit.GetPosition(MapID, InstanceID, RoleID)
		local k = math.random(1,100)
		local n = math.random(1,4)
	if k<= 10 then
		map.MapCreateColCreature(MapID, InstanceID, 1010203, x, y, z, 1)
	elseif k<= 20 then
		map.MapCreateColCreature(MapID, InstanceID, 1010204, x, y, z, 1)
	elseif k<= 30 then
		map.MapCreateColCreature(MapID, InstanceID, 1010208, x, y, z, 1)
	elseif k<= 40 then
		map.MapCreateColCreature(MapID, InstanceID, 1010212, x, y, z, 1)
	elseif k<= 60 then
		map.MapCreateColCreature(MapID, InstanceID, 1010257, x, y, z, 1)
	elseif k<= 70 then
		map.MapCreateColCreature(MapID, InstanceID, 1010255, x, y, z, 1)
	elseif k<= 80 then
		map.MapCreateColCreature(MapID, InstanceID, 1010262, x, y, z, 1)
	elseif k<= 90 then
		map.MapCreateColCreature(MapID, InstanceID, 1010305, x, y, z, 1)
	else
		map.MapCreateColCreature(MapID, InstanceID, 1010306, x, y, z, 1)
	end
	if n == 1 then
		unit.AddBuff(MapID, InstanceID, RoleID, 3306601, RoleID)
	elseif n == 2 then
		unit.AddBuff(MapID, InstanceID, RoleID, 3306701, RoleID)
	elseif n == 3 then
		unit.AddBuff(MapID, InstanceID, RoleID, 3306801, RoleID)
	else
		unit.AddBuff(MapID, InstanceID, RoleID, 3306901, RoleID)
	end
end
aux.RegisterCreatureEvent(3075113, 6, "c3075113_OnInvest")














