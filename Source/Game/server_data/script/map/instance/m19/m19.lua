--ScriptData  索引
--0: 物品3200513 已用
--1: 怪物 土巴丹 1006533 是否可刷记录
--2: 幽量亲兵击杀数量
--3: 寒冰
--4: Boss
--5: 是否刷过烟花

--当角色进入副本后，无法使用部分技能
--m19_SkillLimit = {}
--m19_SkillLimit[11005] = true
--m19_SkillLimit[11009] = true
--m19_SkillLimit[11012] = true
--m19_SkillLimit[11014] = true
--m19_SkillLimit[90012] = true

--function m19_CanUseSkill(MapID, InstanceID, SkillTypeID)
--    if m19_SkillLimit[SkillTypeID] then
--        return false
--    end
--    return true
--end

--aux.RegisterMapEvent("m19", 21, "m19_CanUseSkill")

--进入副本时删除玩家的坐骑buff
function m19_OnPlayerEnter(MapID, InstanceID,RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 99996)
	if bhave == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 9999601)
	end

	local Yanhua=map.GetMapScriptData(MapID, InstanceID, 1, 5)

	if Yanhua==0 then

		local YanHua_Pos={}
		YanHua_Pos[1]={x=1021, z=956, y=5888}
		YanHua_Pos[2]={x=1063, z=902, y=5883}
		YanHua_Pos[3]={x=1024, z=822, y=5889}
		YanHua_Pos[4]={x=1089, z=854, y=5880}
		YanHua_Pos[5]={x=1100, z=775, y=5858}
		YanHua_Pos[6]={x=1146, z=763, y=5696}
		YanHua_Pos[7]={x=1181, z=794, y=5696}

		local index = math.random(7)

		for i=1,7 do
			local monsterid = map.MapCreateCreature(MapID, InstanceID, 3040539, YanHua_Pos[i].x, YanHua_Pos[i].y, YanHua_Pos[i].z)
			if i==index then
				cre.SetCreatureScriptData(MapID, InstanceID, monsterid, 1, 0, 1)
			end
		end

		map.SetMapScriptData(MapID, InstanceID, 1, 5, 1)
	end
end

--注册
aux.RegisterMapEvent("m19", 2, "m19_OnPlayerEnter")


function m19_OnRoleEnterArea(MapID, InstanceID, RoleID, ObjAreaID)

	if ObjAreaID==3132 then

		local wujian = map.GetMapScriptData(MapID, InstanceID, 1, 4)
		wujian = math.floor(wujian/100) % 10

		local temp = math.random(100)

		if wujian==0 and temp<=1 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1739)	--点燃营地中的烟花才可惊出无间军头！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end
	end

end

aux.RegisterMapEvent("m19", 7, "m19_OnRoleEnterArea")
