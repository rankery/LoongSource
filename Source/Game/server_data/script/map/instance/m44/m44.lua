--ScriptData 索引1 触发器id：2580 是否已生效
--ScriptData 索引2 触发器id：2597 是否已生效
--ScriptData 索引3 触发器id：2613 是否已生效
--ScriptData 索引4 触发器id：2622 是否已生效
--ScriptData 索引5 触发器id：2636 是否已生效
--ScriptData 索引6 触发器id：2704 是否已生效
--ScriptData 索引7 触发器id：2715 是否已生效
--ScriptData 索引8 触发器id：2696 是否已生效
--ScriptData 索引9 触发器id：2809 是否已生效
--ScriptData 索引10 触发器id：2974 是否已生效
--ScriptData 索引11 触发器id：3116 是否已生效

--ScriptData 索引12 调查（3075006）	怪物刷出 是否已生效
--ScriptData 索引13 调查（3075007）	怪物刷出 是否已生效
--ScriptData 索引14 （1010106） 记录是否可刷新
--ScriptData 索引15 1010102  记录是否可刷新
--ScriptData 索引16 1010112	怪物刷出 是否已生效
--ScriptData 索引17 1010113	怪物刷出 是否已生效
--ScriptData 索引18 1010112,1010113	是否已全部死亡

--ScriptData 索引19 1010005	记录怪物ID
--ScriptData 索引20 1010006	记录怪物ID
--ScriptData 索引21 1010007	记录怪物ID
--ScriptData 索引22 1010008	记录怪物ID


--ScriptData 索引23 记录是否召唤出100级神器BOSS?

--ScriptData 索引24 1010010	记录怪物ID
--ScriptData 索引25-31 用于祭魔石。

--ScriptData 索引32 1010106	是否已刷新
--ScriptData 索引33 3075008	是否已刷新

--ScriptData 索引35 记录1010106 是否已刷新过，0否
--ScriptData 索引36 记录3075010 的ID


-------玉虚峰三层 玉虚神女刷出时：激烈的争斗惊动玉虚神女，狂性大发！ 玉虚守护刷出时，玉虚守护现身，欲为枉死部众报仇，请速速前往击杀！封步石消失时，显示系统提示：封步石法力涣散，封步效果尽失！
-------启动玉虚峰二层溶洞中的开门机关后打开大门 并显示系统提示：溶洞大门已打开
function SendMsg_MapCreatureEvent(MapID,InstanceID,MID,CID,kind)--单怪,MID-IDMsgID,CID--CreatureID,kind--信息类型，显示怪物名称填5
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, MID)
	msg.AddMsgEvent(MsgID, kind, CID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

function SendMsg_MapEvent(MapID,InstanceID,ID)--场景内提示，全体，多用于群体怪物和机关
	local MsgID = msg.BeginMsgEvent()
	msg.AddMsgEvent(MsgID, 26, ID)
	msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
end

--[[进入副本时刷新4个怪物
445 2392 530
452 2279 522
442 2371 521
438 2468 532]]
function m44_OnPlayerEnter(MapID, InstanceID,RoleID)

	unit.CancelBuff(MapID, InstanceID, RoleID, 99996)

--进入副本时删除玩家的坐骑buff
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	end
--[[在 445 2392 530 刷新 1010005
452 2279 522 1010006
442 2371 521 1010007
438 2468 532 1010008
431,4657, 1525,1010010 ]]
	local x = map.GetMapScriptData(MapID,InstanceID,1,24)

	if x < 1 then

	local a = map.MapCreateCreature(MapID, InstanceID, 1010005, 450, 2047, 509)
	local b = map.MapCreateCreature(MapID, InstanceID, 1010006, 453, 2169, 515)
	local c = map.MapCreateCreature(MapID, InstanceID, 1010007, 445, 2290, 519)
	local d = map.MapCreateCreature(MapID, InstanceID, 1010008, 448, 2358, 528)
	local e = map.MapCreateCreature(MapID, InstanceID, 1010010, 444, 2384, 528)
	local f = map.MapCreateCreature(MapID, InstanceID, 3075010, 703, 2554, 1213)
	map.SetMapScriptData(MapID,InstanceID,1,19,a)
	map.SetMapScriptData(MapID,InstanceID,1,20,b)
	map.SetMapScriptData(MapID,InstanceID,1,21,c)
	map.SetMapScriptData(MapID,InstanceID,1,22,d)
	map.SetMapScriptData(MapID,InstanceID,1,24,e)
	map.SetMapScriptData(MapID,InstanceID,1,36,f)
	end

end

--注册
aux.RegisterMapEvent("m44", 2, "m44_OnPlayerEnter")

function m44_OnEnterTrigger(MapID, InstanceID, RoleID, ObjID)
--[[触发器id 2580
精英A-1（1010011）X=989 Y=1421 Z=296
精英A-1（1010011）X=1011 Y=1467 Z=291
精英A-1（1010011）X=993 Y=1438 Z=310
精英A-1（1010011）X=1013 Y=1480 Z=305]]
	if ObjID == 2580 then
		local Activated0 = map.GetMapScriptData(MapID,InstanceID,1,1) --索引1数值为0则刷怪
		if Activated0 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,1,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 989, 1421, 296, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 1011, 1467, 291, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 993, 1438, 310, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1008517, 1013, 1480, 305, 1)
		end
--[[触发器id：2597
精英B-1（1010013）X=945 Y=1427 Z=304
精英B-1（1010013）X=947 Y=1413 Z=314
精英A-2（1010012）X=934 Y=1415 Z=321
精英A-2（1010012）X=930 Y=1426 Z=309]]
	elseif ObjID == 2597 then
		local Activated1 = map.GetMapScriptData(MapID,InstanceID,1,2) -----索引2数值为0则刷怪
		if Activated1 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,2,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 945, 1427, 304, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 947, 1413, 314, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 934, 1415, 321, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 930, 1426, 309, 1)
		end
--[[触发器id：2613
精英A-1（1010011）X=818 Y=2109 Z=208
精英A-1（1010011）X=810 Y=2043 Z=215
精英A-1（1010011）X=795 Y=2071 Z=202
精英B-2（1010014）X=801 Y=2033 Z=210]]
	elseif ObjID == 2613 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,3) -----索引3数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,3,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 818, 2109, 208, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 810, 2043, 215, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 795, 2071, 202, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010014, 801, 2033, 210, 1)
		end
--[[触发器id 2622
精英A-2（1010012）X=523 Y=1788 Z=244
精英A-2（1010012）X=509 Y=1779 Z=252
精英B-1（1010013）X=503 Y=1800 Z=241
精英B-1（1010013）X=516 Y=1782 Z=234]]
	elseif ObjID == 2622 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,4) -----索引4数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,4,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 523, 1788, 244, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 509, 1779, 252, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 503, 1800, 241, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 516, 1782, 234, 1)
		end
--[[触发器id 2636
精英B-1（1010013）X=760 Y=1400 Z=238
精英A-2（1010012）X=756 Y=1380 Z=253
精英A-1（1010011）X=731 Y=1408 Z=230
精英B-2（1010014）X=728 Y=1418 Z=246]]
	elseif ObjID == 2636 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,5) -----索引5数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,5,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 760, 1400, 238, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 756, 1380, 253, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 731, 1408, 230, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 728, 1418, 246, 1)
		end
--[[触发器id 2704
精英B-2（1010014）X=366 Y=1504 Z=386
精英A-1（1010011）X=380 Y=1504 Z=382
精英A-2（1010012）X=375 Y=1504 Z=408
精英B-1（1010013）X=387 Y=1504 Z=403]]
	elseif ObjID == 2704 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,6) -----索引6数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,6,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010014, 366, 1504, 386, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 380, 1504, 382, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 375, 1504, 408, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 387, 1504, 403, 1)
		end
--[[触发器id 2715
精英A-1（1010011）X=590 Y=1793 Z=453
精英A-1（1010011）X=587 Y=1760 Z=442
精英A-2（1010012）X=611 Y=1803 Z=445
精英A-2（1010012）X=607 Y=1758 Z=435]]
	elseif ObjID == 2704 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,7) -----索引7数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,7,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 590, 1793, 453, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 587, 1760, 442, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 611, 1803, 445, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 607, 1758, 435, 1)
		end
--[[触发器id 2696
精英A-1（1010011）X=450 Y=1442 Z=413
精英A-1（1010011）X=437 Y=1466 Z=383
精英A-2（1010012）X=457 Y=1438 Z=402
精英B-1（1010013）X=422 Y=1404 Z=384
精英B-1（1010013）X=408 Y=1404 Z=400
精英B-2（1010014）X=410 Y=1404 Z=387
精英B-2（1010014）X=428 Y=1457 Z=379]]
	elseif ObjID == 2696 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,8) -----索引8数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,8,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 450, 1442, 413, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010011, 437, 1466, 383, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010012, 457, 1438, 402, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 422, 1404, 384, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010013, 408, 1404, 400, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010014, 410, 1404, 387, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010014, 428, 1457, 379, 1)
		end
--[[触发器id 2809
精英G-1(1010064)X=455 Y=2632 Z=792
精英E-1(1010060)X=456 Y=2658 Z=841
精英E-1(1010060)X=426 Y=2624 Z=840
精英E-1(1010060)X=425 Y=2701 Z=799]]
	elseif ObjID == 2809 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,9) -----索引9数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,9,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010064, 455, 2632, 792, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010060, 456, 2658, 841, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010060, 426, 2624, 840, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010060, 426, 2701, 799, 1)
		end
--[[触发器id 2974
精英H-1(1010114)X=509 Y=1130 Z=1421
精英H-1(1010114)X=518 Y=1130 Z=1431
精英I-2(1010117)X=511 Y=1130 Z=1441]]
	elseif ObjID == 2974 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,10) -----索引10数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,10,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010114, 509, 1130, 1421, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010114, 518, 1130, 1431, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010117, 511, 1130, 1441, 1)
		end
--[[触发器id 3116
精英J-2(1010119)X=580 Y=3510 Z=1422
精英J-2(1010119)X=584 Y=3510 Z=1432
精英J-2(1010119)X=568 Y=3510 Z=1421
精英J-2(1010119)X=565 Y=3510 Z=1439]]
	elseif ObjID == 3116 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,11) -----索引11数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,11,1)
		map.MapCreateColCreature(MapID, InstanceID, 1010119, 580, 3510, 1422, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010119, 584, 3510, 1432, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010119, 568, 3510, 1421, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010119, 565, 3510, 1439, 1)
		end
--触发器id 2589 进入后添加BUFF()

	elseif ObjID == 2589 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003601, RoleID)
--触发器id 2650 进入后添加BUFF()
	elseif ObjID == 2650 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003601, RoleID)
--触发器id 2852 进入后添加BUFF()
	elseif ObjID == 2852 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003601, RoleID)
--触发器id 2670	进入后添加BUFF

	elseif ObjID == 2670 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003701, RoleID)
--触发器id 2832	进入后添加BUFF()
	elseif ObjID == 2832 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003701, RoleID)
--触发器id 3065	进入后添加BUFF()
	elseif ObjID == 3065 then
		unit.AddBuff(MapID, InstanceID, RoleID, 5003501, RoleID)
	end
end
aux.RegisterMapEvent("m44",6,"m44_OnEnterTrigger")




function m44_OnInvest(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
--[[刷怪机关-1	调查（3075006）	怪物刷出同时系统给予明确提示
BOSS-3 (1010003)X=552 Y=1400 Z=373
精英B-1(1010113)X=532 Y=1412 Z=363
精英B-1(1010113)X=545 Y=1382 Z=351
精英B-2(1010114)X=548 Y=1390 Z=380
精英B-2(1010114)X=559 Y=1389 Z=376
精英A-2(1010112)X=541 Y=1404 Z=369
精英A-2(1010112)X=551 Y=1428 Z=361]]
	if CreatureTypeID == 3075006 then
		local Activated1 = map.GetMapScriptData(MapID,InstanceID,1,12) --索引12数值为0则刷怪
		if Activated1 == 0 then --判断是否被触发过
			map.SetMapScriptData(MapID,InstanceID,1,12,1)

			map.MapCreateColCreature(MapID, InstanceID, 1010003, 552, 1400, 373, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010013, 532, 1412, 363, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010013, 545, 1382, 351, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010014, 548, 1390, 380, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010014, 559, 1389, 376, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010012, 541, 1404, 369, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010012, 551, 1428, 361, 1)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1785)	--双面武师已经出现于坐标[552, 373]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		end
--[[刷怪机关-2	调查（3075007）     怪物刷出同时系统给予明确提示
BOSS-4(1010004)X=408 Y=1884 Z=305
精英A-1(1010011)X=416 Y=1787 Z=315
精英A-1(1010011)X=402 Y=1791 Z=315
精英B-2(1010014)X=421 Y=1884 Z=302
精英B-2(1010014)X=399 Y=1884 Z=302 ]]
	elseif CreatureTypeID == 3075007 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,13) --索引12数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
			map.SetMapScriptData(MapID,InstanceID,1,13,1)

			map.MapCreateColCreature(MapID, InstanceID, 1010004, 408, 1884, 305, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010011, 416, 1787, 315, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010011, 402, 1791, 315, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010014, 421, 1884, 302, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010014, 399, 1884, 302, 1)

			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1786)	--邪毒咒王已经出现于坐标[408, 305]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		end
--[[
开门机关（3075008） 出发后打开大门 同时系统给予文字提示]]
	elseif CreatureTypeID == 3075008 then
		map.OpenCloseDoor(MapID,InstanceID,3144,true)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1798)	--溶洞大门已经打开！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end
aux.RegisterCreatureEvent(3075006, 6, "m44_OnInvest")
aux.RegisterCreatureEvent(3075007, 6, "m44_OnInvest")
aux.RegisterCreatureEvent(3075008, 6, "m44_OnInvest")


--------------------------------------------------------------------------------

function m44_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
--[[击杀特定怪物刷怪
击杀精英K-3（1010122）刷出
精英I-1(1010116)X=645 Y=238 Z=1292
精英I-1(1010116)X=666 Y=281 Z=1280
精英H-2(1010115)X=640 Y=172 Z=1280
精英H-2(1010115)X=658 Y=192 Z=1267
精英H-2(1010115)X=662 Y=207 Z=1294
精英K-1(1010120)X=647 Y=140 Z=1271
全部击杀上述怪刷出BOSS-11(1010106)X=654 Y=200 Z=1283 ]]
	if TargetTypeID == 1010122 then
		map.MapCreateColCreature(MapID, InstanceID, 1010116, 645, 238, 1292, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010116, 666, 281, 1280, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010115, 640, 172, 1280, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010115, 658, 192, 1267, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010115, 662, 207, 1294, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1010120, 647, 140, 1271, 1)

		map.SetMapScriptData(MapID,InstanceID,1,14,6)

	elseif TargetTypeID == 1010116 or TargetTypeID == 1010115 or TargetTypeID == 1010120 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,14)
		local data35 = map.GetMapScriptData(MapID,InstanceID,1,35)
		if a < 0 then
			a = 0
		end
		if a == 1 and data35 ~= 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010106, 654, 200, 1283, 1)
			map.SetMapScriptData(MapID,InstanceID,1,35,1)

		else
			map.SetMapScriptData(MapID,InstanceID,1,14,a-1)
		end

--[[击杀BOSS-9（1010103）刷出祭祀-1（1010108）X=403 Y=5188 Z=1454        怪物刷出同时系统给予明确提示
击杀BOSS-10（1010104）刷出祭祀-2（1010109）X=504 Y=5138 Z=1543	怪物刷出同时系统给予明确提示
击杀BOSS-11（1010106）刷出祭祀-3（1010110）X=420 Y=4988 Z=1638    怪物刷出同时系统给予明确提示
击杀BOSS-12（1010107）刷出祭祀-4（1010111）X=322 Y=5308 Z=1557	怪物刷出同时系统给予明确提示
每个祭祀的存活时间为20分钟，当4名祭祀同时存活时，刷出隐藏BOSS(1010102)  X=Y=Z=       怪物刷出同时系统给予明确提示 --------------------------------
同时抵消封步石（3075010）产生的减速DEBUFF(5003102)

--全部击杀BOSS-9，10（1010103，1010104）刷出祭祀护卫A（1010112）X=312 Y=3590 Z=1458	怪物刷出同时系统给予明确提示
--全部击杀BOSS-11，12（1010106，1010107）刷出祭祀护卫B（1010113）X=511 Y=3470 Z=1634	怪物刷出同时系统给予明确提示]]

	elseif TargetTypeID == 1010103 then
		map.MapCreateColCreature(MapID, InstanceID, 1010108, 403, 5188, 1454, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1787)	--仁念祭祀已经出现！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		local b1 = map.GetMapScriptData(MapID,InstanceID,1,16)

		if b1 == 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010112, 312, 3590, 1458, 1)
			SendMsg_MapCreatureEvent(MapID,InstanceID,400,1010112,5)
		else
			map.SetMapScriptData(MapID,InstanceID,1,16,1)
		end


		local a1 = map.GetMapScriptData(MapID,InstanceID,1,15)
		if a1 == 3 then
			map.MapCreateColCreature(MapID,InstanceID, 1010102, 464, 0, 1517, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1791)	--"四名祭祀共行法事，玉虚守护终于现身玉虚峰！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			local stoneID = map.GetMapScriptData(MapID,InstanceID,1,36)
			map.MapDeleteCreature(MapID,InstanceID,stoneID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1792)	--"封步石法力涣散，封步效果尽失！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		else
			map.SetMapScriptData(MapID,InstanceID,1,15,a1 +1)
		end

	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010104 then
		local b2 = map.GetMapScriptData(MapID,InstanceID,1,16)

		if b2 == 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010112, 312, 3590, 1458, 1)
			SendMsg_MapCreatureEvent(MapID,InstanceID,400,1010112,5)
		else
			map.SetMapScriptData(MapID,InstanceID,1,16,1)
		end

		map.MapCreateColCreature(MapID, InstanceID, 1010109, 504, 5138, 1543, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1788)	--智意祭祀已经出现！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		local a2 = map.GetMapScriptData(MapID,InstanceID,1,15)
		if a2 == 3 then
			map.MapCreateColCreature(MapID,InstanceID, 1010102, 464, 0, 1517, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1791)	--"四名祭祀共行法事，玉虚守护终于现身玉虚峰！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			local stoneID = map.GetMapScriptData(MapID,InstanceID,1,36)
			map.MapDeleteCreature(MapID,InstanceID,stoneID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1792)	--"封步石法力涣散，封步效果尽失！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		else
			map.SetMapScriptData(MapID,InstanceID,1,15,a2 +1)
		end
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010106 then

		local b3 = map.GetMapScriptData(MapID,InstanceID,1,17)

		if b3 == 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010113, 511, 3470, 1634, 1)
			SendMsg_MapCreatureEvent(MapID,InstanceID,400,1010113,5)
		else
			map.SetMapScriptData(MapID,InstanceID,1,17,1)
		end

		map.MapCreateColCreature(MapID, InstanceID, 1010110, 420, 4988, 1638, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1789)	--义心祭祀已经出现！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		local a3 = map.GetMapScriptData(MapID,InstanceID,1,15)
		if a3 == 3 then
			map.MapCreateColCreature(MapID,InstanceID, 1010102, 464, 0, 1517, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1791)	--"四名祭祀共行法事，玉虚守护终于现身玉虚峰！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			local stoneID = map.GetMapScriptData(MapID,InstanceID,1,36)
			map.MapDeleteCreature(MapID,InstanceID,stoneID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1792)	--"封步石法力涣散，封步效果尽失！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		else
			map.SetMapScriptData(MapID,InstanceID,1,15,a3 +1)
		end
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010107 then

		local b4 = map.GetMapScriptData(MapID,InstanceID,1,17)

		if b4 == 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010113, 511, 3470, 1634, 1)
			SendMsg_MapCreatureEvent(MapID,InstanceID,400,1010113,5)
		else
			map.SetMapScriptData(MapID,InstanceID,1,17,1)
		end

		map.MapCreateColCreature(MapID, InstanceID, 1010111, 322, 5308, 1557, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1790)	--勇神祭祀已经出现！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		local a4 = map.GetMapScriptData(MapID,InstanceID,1,15)
		if a4 == 3 then
			map.MapCreateColCreature(MapID,InstanceID, 1010102, 464, 0, 1517, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1791)	--"四名祭祀共行法事，玉虚守护终于现身玉虚峰！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			local stoneID = map.GetMapScriptData(MapID,InstanceID,1,36)
			map.MapDeleteCreature(MapID,InstanceID,stoneID)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1792)	--"封步石法力涣散，封步效果尽失！"
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		else
			map.SetMapScriptData(MapID,InstanceID,1,15,a4 +1)
		end
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010108 or TargetTypeID == 1010109 or TargetTypeID == 1010110 or TargetTypeID == 1010111 then
		local a5 = map.GetMapScriptData(MapID,InstanceID,1,15)
		if a5 < 1 then
			a5 = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,15,a5 -1)



--[[击杀护卫A,B（1010112，1010113）  怪物刷出同时系统给予明确提示
最终BOSS（1010101）X=331 Y=3560 Z=1641
精英H-1(1010114)X=338 Y=3560 Z=1626
精英H-1(1010114)X=345 Y=3560 Z=1633
精英K-1(1010120)X=331 Y=3560 Z=1633
精英K-1(1010120)X=339 Y=3560 Z=1640
精英K-2(1010121)X=320 Y=3560 Z=1640
精英K-2(1010121)X=332 Y=3560 Z=1651]]
	elseif TargetTypeID == 1010112 then

		local a6 = map.GetMapScriptData(MapID,InstanceID,1,18)
		if a6 ~= 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010101, 424, 4827, 1534, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010114, 338, 3560, 1626, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010114, 345, 3560, 1633, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010120, 331, 3560, 1633, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010120, 339, 3560, 1640, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010121, 320, 3560, 1640, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010121, 332, 3560, 1651, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1793)	--玉虚神女已经出现于坐标[424, 1534]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			map.SetMapScriptData(MapID,InstanceID,1,18,1)
		end
	elseif TargetTypeID == 1010113 then

		local a6 = map.GetMapScriptData(MapID,InstanceID,1,18)
		if a6 ~= 1 then
			map.MapCreateColCreature(MapID, InstanceID, 1010101, 424, 4827, 1534, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010114, 338, 3560, 1626, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010114, 345, 3560, 1633, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010120, 331, 3560, 1633, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010120, 339, 3560, 1640, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010121, 320, 3560, 1640, 1)
			map.MapCreateColCreature(MapID, InstanceID, 1010121, 332, 3560, 1651, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1793)	--玉虚神女已经出现于坐标[424, 1534]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
			map.SetMapScriptData(MapID,InstanceID,1,18,1)
		end
--[[
击杀BOSS-1（1010001）BOSS-A全部属性削弱75%（1010005
击杀BOSS-2（1010002）BOSS-A全部属性削弱75%（1010006）
击杀BOSS-3（1010003）BOSS-A全部属性削弱75%（1010007）
击杀BOSS-4（1010004）BOSS-A全部属性削弱75%（1010008 ]]

	elseif TargetTypeID == 1010001 then
	    cre.MonsterSay(MapID, InstanceID, TargetID, 50032)
		local i = map.GetMapScriptData(MapID,InstanceID,1,19)
		map.MapDeleteCreature(MapID, InstanceID, i)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1794)	--分身消失
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010002 then
	    cre.MonsterSay(MapID, InstanceID, TargetID, 50033)
		local i = map.GetMapScriptData(MapID,InstanceID,1,20)
		map.MapDeleteCreature(MapID, InstanceID, i)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1795)	--分身消失
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010003 then
	    cre.MonsterSay(MapID, InstanceID, TargetID, 50034)
		local i = map.GetMapScriptData(MapID,InstanceID,1,21)
		map.MapDeleteCreature(MapID, InstanceID, i)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1796)	--分身消失
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010004 then
	    cre.MonsterSay(MapID, InstanceID, TargetID, 50035)
		local i = map.GetMapScriptData(MapID,InstanceID,1,22)
		map.MapDeleteCreature(MapID, InstanceID, i)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1797)	--分身消失
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010057 then
		local data33 = map.GetMapScriptData(MapID,InstanceID,1,33)
		if data33 == 1 then
			map.MapCreateCreature(MapID, InstanceID, 3075008, 738 , 2700 , 743)
		else
			map.SetMapScriptData(MapID,InstanceID,1,33,1)
		end
			Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010056 then
		local data33 = map.GetMapScriptData(MapID,InstanceID,1,33)
		if data33 == 1 then
			map.MapCreateCreature(MapID, InstanceID, 3075008, 738 , 2700 , 743)
		else
			map.SetMapScriptData(MapID,InstanceID,1,33,1)
		end
			Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	elseif TargetTypeID == 1010101 then
		cre.MonsterSay(MapID, InstanceID, TargetID, 50048)
		--刷新祭魔台
		--local x, y, z = unit.GetPosition(MapID, InstanceID, TargetID)
		--map.MapCreateCreature(MapID, InstanceID, 1531108, x, y, z)
		map.MapCreateColCreature(MapID, InstanceID, 4006009, 424, 4827, 1534, 1)
		Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

		local TeamID = role.IsRoleHaveTeam(MapID, InstanceID, RoleID)
		local Role = {}
		Role[1], Role[2], Role[3], Role[4], Role[5], Role[6] = role.GetRoleTeamMemID(TeamID)
		for i=1, 6 do
			if Role[i] and Role[i]~= 4294967295 then
				local Target_MapID, Target_InstanceID = role.GetRoleMapID(Role[i])
				if Target_MapID == MapID then
					role.AddRoleItem(Target_MapID, Target_InstanceID, Role[i], 3305003, 5, -1, 8, 420)
				    if act.GetActIsStart(151) then
						HuoDeYinYuanJie(Role[i])
						HuoDeYinYuanJie(Role[i])
						HuoDeYinYuanJie(Role[i])
						HuoDeYinYuanJie(Role[i])
						HuoDeYinYuanJie(Role[i])
					end
				end
			end
		end
		--击杀玉虚神女后播放服务器广播的相关脚本内容
		local MsgID = msg.BeginMsgEvent();					--注册广播消息句柄
		local n_RoleLv = role.GetRoleLevel(MapID, InstanceID, RoleID);		--获得击杀玩家的等级

		if ( n_RoleLv<101 ) then						--若击杀玩家的等级不大于100级时，则播放全服大字屏显广播
			msg.AddMsgEvent(MsgID,102,100017);
			msg.AddMsgEvent(MsgID,2,RoleID);
			msg.DispatchBroadcast(MsgID,-1,-1,-1);
		else
			if ( n_RoleLv<111 ) then
				msg.AddMsgEvent(MsgID,102,100018);
				msg.AddMsgEvent(MsgID,2,RoleID);
			else
				msg.AddMsgEvent(MsgID,102,100019);
				msg.AddMsgEvent(MsgID,2,RoleID);
			end

			local r = math.random(100);
			if ( r<16 ) then
				msg.DispatchBroadcast(MsgID,-1,-1,-1);			--击杀玩家等级大于100级时有15%几率播放全服广播，60%几率场景广播
			elseif ( r<61 ) then
				msg.DispatchBroadcast(MsgID,3017299407,-1,-1);--昆仑
				msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
			else
				msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
			end
		end
		----------------------------------------------------------------------------------------
	end
end


aux.RegisterCreatureEvent(1010122,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010116,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010115,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010120,4,"m44_OnDie")

aux.RegisterCreatureEvent(1010103,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010104,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010106,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010107,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010108,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010109,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010110,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010111,4,"m44_OnDie")

aux.RegisterCreatureEvent(1010112,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010113,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010001,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010002,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010003,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010004,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010057,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010056,4,"m44_OnDie")
aux.RegisterCreatureEvent(1010101,4,"m44_OnDie")

function C1010005_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")
end
aux.RegisterCreatureEvent(1010005, 4, "C1010005_OnDie")

function C1010006_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")
end
aux.RegisterCreatureEvent(1010006, 4, "C1010006_OnDie")

function C1010007_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")
end
aux.RegisterCreatureEvent(1010007, 4, "C1010007_OnDie")

function C1010008_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")
end
aux.RegisterCreatureEvent(1010008, 4, "C1010008_OnDie")

function C1010009_OnDie(MapID, InstanceID, CreatureID, CreatureTypeID, RoleID)
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")
end
aux.RegisterCreatureEvent(1010009, 4, "C1010009_OnDie")


--物品使用
--使用道具<碎丹咒（2615501）>去除【至攻诀（加攻BUFF）】：**9422902**

function I2615501_use(MapID, InstanceID, TypeID, TargetID)
	local i = map.GetMapScriptData(MapID,InstanceID,1,24)
	unit.CancelBuff(MapID, InstanceID, i, 9422902)
end
--只能在m44 X=435 Y=2538 Z=543 附近使用
function I2615501_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	if MapID ~= 3084408527 then
		bRet = 43
	else
		local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		if x < 415 or x > 455 or z < 523 or z > 563 or y < 2500 or y > 2580 then
		bRet = 43
		end
	end
	return bRet, bIgnore
end

aux.RegisterItemEvent(2615501, 1, "I2615501_use")
aux.RegisterItemEvent(2615501, 0, "I2615501_CanUse")

--物品使用
--使用道具<震元咒（2615502）>去除【至防诀（加防BUFF）】：**9423002**

function I2615502_Use(MapID, InstanceID, TypeID, TargetID)
	local i = map.GetMapScriptData(MapID,InstanceID,1,24)
	unit.CancelBuff(MapID, InstanceID, i, 9423002)
end

aux.RegisterItemEvent(2615502, 1, "I2615502_Use")
aux.RegisterItemEvent(2615502, 0, "I2615501_CanUse")



--[[当角色进入副本后，无法使用部分技能
m44_SkillLimit = {}
m44_SkillLimit[11005] = true
m44_SkillLimit[11009] = true
m44_SkillLimit[11012] = true
m44_SkillLimit[11014] = true
m44_SkillLimit[90012] = true

function m44_CanUseSkill(MapID, InstanceID, SkillTypeID)
    if m44_SkillLimit[SkillTypeID] then
        return false
    end
    return true
end

aux.RegisterMapEvent("m44", 21, "m44_CanUseSkill")]]
