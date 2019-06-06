
function shengxiao_OnComplete(mapID, instanceID, questID, ownerID, enderID)

	local roleShengxiao = role.GetRoleScriptData(ownerID, 1, RoleDataType["Shengxiao_Val"])

	local value_inc = {}
	value_inc[20021] = 2
	value_inc[20022] = 2
	value_inc[20023] = 4
	value_inc[20024] = 15
	value_inc[20025] = 6
	value_inc[20026] = 10
	value_inc[20027] = 50
	value_inc[20028] = 25
	value_inc[20029] = 12
	value_inc[20030] = 25
	value_inc[20031] = 22
	value_inc[20032] = 35

	roleShengxiao = roleShengxiao+value_inc[questID]

	role.SetRoleScriptData(ownerID, 1, RoleDataType["Shengxiao_Val"], roleShengxiao)

--	local MsgID = msg.BeginMsgEvent()
--	msg.AddMsgEvent(MsgID, 26, 37)
--	msg.AddMsgEvent(MsgID, 9, roleShengxiao)
--	msg.DispatchRoleMsgEvent(ownerID, MsgID)

	--增加进阶实力值
	Increase_shili(MapID, InstanceID, ownerID, 0, 2, "jinjieshili_A")

end

aux.RegisterQuestEvent(20021, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20022, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20023, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20024, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20025, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20026, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20027, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20028, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20029, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20030, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20031, 1, "shengxiao_OnComplete")
aux.RegisterQuestEvent(20032, 1, "shengxiao_OnComplete")


function I2612601_CanUse(MapID, InstanceID, TypeID, TargetID)

	local S_shizhe={}
	S_shizhe[1] = {npcid=3610004, mapcrc=3000522447, x=837, y=150, z=351, value_req=0}
	S_shizhe[2] = {npcid=3610007, mapcrc=3084407503, x=866, y=5060, z=470, value_req=0}
	S_shizhe[3] = {npcid=3610010, mapcrc=3084406991, x=770, y=2586, z=290, value_req=0}
	S_shizhe[4] = {npcid=3610012, mapcrc=1826988583, x=770, y=4048, z=811, value_req=0}

	--判断使用地点限制
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)
	local roleShengxiao = role.GetRoleScriptData(TargetID, 1, RoleDataType["Shengxiao_Val"])

	for i=1, 4 do
		if (MapID==S_shizhe[i].mapcrc) and (x<S_shizhe[i].x+16) and (x>S_shizhe[i].x-16) and (z<S_shizhe[i].z+16) and (z<S_shizhe[i].z+16) then
			if roleShengxiao >= S_shizhe[i].value_req then
				return 0, false

			else
				local MsgID = msg.BeginMsgEvent()
				msg.AddMsgEvent(MsgID, 26, 38)
				msg.DispatchRoleMsgEvent(ownerID, MsgID)
				return 32, false
			end
		end
	end

	return 43, false
end

function I2612601_ShengXiao(MapID, InstanceID, TypeID, TargetID)

	local S_shizhe={}
	S_shizhe[1] = {npcid=3610004, mapcrc=3000522447, x=837, y=150, z=351, value_req=20}
	S_shizhe[2] = {npcid=3610007, mapcrc=3084407503, x=866, y=5060, z=470, value_req=80}
	S_shizhe[3] = {npcid=3610010, mapcrc=3084406991, x=770, y=2586, z=290, value_req=180}
	S_shizhe[4] = {npcid=3610012, mapcrc=1826988583, x=770, y=4048, z=811, value_req=500}

	--判断使用地点限制
	local x, y ,z = unit.GetPosition(MapID, InstanceID, TargetID)

	for i=1, 4 do
		if (MapID==S_shizhe[i].mapcrc) and (x<S_shizhe[i].x+16) and (x>S_shizhe[i].x-16) and (z<S_shizhe[i].z+16) and (z<S_shizhe[i].z+16) then
			map.MapCreateCreature(MapID, InstanceID, S_shizhe[i].npcid, S_shizhe[i].x, S_shizhe[i].y, S_shizhe[i].z)
		end
	end
end

aux.RegisterItemEvent(2612601, 0, "I2612601_CanUse")
aux.RegisterItemEvent(2612601, 1, "I2612601_ShengXiao")
