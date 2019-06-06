--m41ScriptData 索引0 区域3334 记录怪物数量
--m41ScriptData 索引1 区域3609 记录怪物数量
--m41ScriptData 索引2 区域3611 记录怪物数量
--m41ScriptData 索引3-9 用于祭魔台
--m41ScriptData 索引10，11任务死战 刷怪数量限制用
--m41ScriptData 索引12, 13 用于前区小怪BOSS
--m41ScriptData 索引14, 15 用于后区小怪BOSS
--m41ScriptData 索引16 用于领取奖励的人数纪录

--对话 杜维 3060003 给与5秒钟跳跃buff
function n3060003_OnTalk(MapID, InstanceID, TargetID, TargetTypeID, RoleID, TalkIndex)

	if TalkIndex == -1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 20, 268)	-- "我可以暂时增强你的能力，要试试吗？"
		msg.AddMsgEvent(MsgID, 21, 4)	-- 确定按钮
		msg.AddMsgEvent(MsgID, 1, 206)	-- "确定"
		msg.AddMsgEvent(MsgID, 21, 5)	-- 取消按钮
		msg.AddMsgEvent(MsgID, 1, 207)	-- "取消"
		msg.AddMsgEvent(MsgID, 24, TargetID)
		msg.DispatchRoleMsgEvent(RoleID, MsgID)

	elseif TalkIndex == 4 then
		unit.AddBuff(MapID, InstanceID, RoleID, 2016501, RoleID)
		TalkIndex = -1
	end
end
aux.RegisterCreatureEvent(3060003, 7, "n3060003_OnTalk")

function m41_OnEnterArea(MapID, InstanceID, RoleID, ObjID)
	--·玩家进入地图区域 3334 即被传送至坐标：x 2325 y 26170 z 244
	if ObjID == 3334 then
		role.RoleGotoNewMap(MapID, InstanceID, RoleID, 3084407759, 2325, 26170, 244)
	--刷出 枯厌力士 1008026 按导航点 b01 路线巡逻 / 同时播放特效 txcj49 坐标：x 890 y 14780 z 676
	--刷出 枯厌妖师 1008027 按导航点 b02 路线巡逻 / 同时播放特效 txcj49 坐标：x 896 y 14780 z 676
	--刷出 枯厌力士 1008026 按导航点 b03 路线巡逻 / 同时播放特效 txcj49 坐标：x 902 y 14780 z 676
	elseif ObjID == 3609 then
		local Activated0 = map.GetMapScriptData(MapID,InstanceID,1,0) --索引0数值为0则刷怪
		if Activated0 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,0,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008026, 890, 14780, 676, 1, "b01")
		map.MapCreateColCreature(MapID, InstanceID, 1008027, 896, 14780, 676, 1, "b02")
		map.MapCreateColCreature(MapID, InstanceID, 1008026, 902, 14780, 676, 1, "b03")
		end
	--刷出 枯厌弓卫 1008031 按导航点 b04 路线巡逻 / 同时播放特效 txcj49 坐标：x 895 y 14780 z 577
	--刷出 枯厌弓卫 1008031 按导航点 b04 路线巡逻 / 同时播放特效 txcj49 坐标：x 904 y 14784 z 574
	elseif ObjID == 3610 then
		local Activated1 = map.GetMapScriptData(MapID,InstanceID,1,1) -----索引1数值为0则刷怪
		if Activated1 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,1,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008031, 895, 14780, 577, 1, "b04")
		map.MapCreateColCreature(MapID, InstanceID, 1008031, 904, 14784, 574, 1, "b04")
		end
	--刷出 濂虐妖将 1008037 按导航点 b05 路线巡逻 坐标：x 1140 y 14780 z 365
	--刷出 濂虐妖将 1008037 按导航点 b05 路线巡逻 坐标：x 1245 y 14780 z 480
	elseif ObjID == 3611 then
		local Activated2 = map.GetMapScriptData(MapID,InstanceID,1,2) -----索引2数值为0则刷怪
		if Activated2 == 0 then --判断是否被触发过
		map.SetMapScriptData(MapID,InstanceID,1,2,1)
		map.MapCreateColCreature(MapID, InstanceID, 1008037, 1140, 14780, 365, 1, "b05")
		map.MapCreateColCreature(MapID, InstanceID, 1008037, 1245, 14780, 480, 1, "b05")
		end
	end
end

aux.RegisterMapEvent("m41", 7, "m41_OnEnterArea")



--·玩家在坐标：x 668 y 10277 z 841 半径40的范围内 使用物品 百越令牌 2614608，在身边刷出 3060038
-- x 668 y 10277 z 841 半径40的范围内 使用物品 百越令牌 2614608
function I2614608_CanUse(MapID, InstanceID, TypeID, TargetID)
	local bRet, bIgnore = 0, false
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	if MapID ~= 3084407759 then
		bRet = 43
	else
		local r = 40
		if x<668 - r or x>668 + r or z<841 - r or z>841 + r then
			bRet = 43
		end

	end
	return bRet, bIgnore
end

--使用百越令牌 2614608，在身边刷出 3060038
function I2614608_QuestUsable(MapID, InstanceID, TypeID, TargetID)
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
		map.MapCreateCreature(MapID, InstanceID, 3060038, x, y, z-5)
end


aux.RegisterItemEvent(2614608, 1, "I2614608_QuestUsable")
aux.RegisterItemEvent(2614608, 0, "I2614608_CanUse")

--进入副本后，部分技能无法使用
--m41_SkillLimit = {}
--m41_SkillLimit[11005] = true
--m41_SkillLimit[11009] = true
--m41_SkillLimit[11012] = true
--m41_SkillLimit[11014] = true
--m41_SkillLimit[90012] = true

--function m41_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m41_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m41", 21, "m41_CanUseSkill")

--进入副本时删除玩家的坐骑buff
function m41_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
	 unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	 end
end

--注册
aux.RegisterMapEvent("m41", 2, "m41_OnPlayerEnter")

--怪物消失或者死亡，将ScriptData 数值减1
function m41_OnDisappear(MapID,InstanceID,CreatureID,CreatureTypeID)

	if CreatureTypeID == 1008006 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,10)
		if a < 1 then
		a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,10,a-1)
	elseif CreatureTypeID == 1008007 then
		local a = map.GetMapScriptData(MapID,InstanceID,1,11)
		if a < 1 then
		a = 1
		end
		map.SetMapScriptData(MapID,InstanceID,1,11,a-1)
	end

end
aux.RegisterMapEvent("m41", 18, "m41_OnDisappear")
