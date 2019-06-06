
--杀掉两个囚徒看守，刷出两个可击杀地物3075209和3075210.

function c1010408_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 3075209, 529, 5670, 1380, 1, "")
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	local sealed = map.GetMapScriptData(MapID, InstanceID, 1, 12)

	if sealed == 0 then
		local Kanshou2_id = map.GetMapScriptData(MapID, InstanceID, 1, 10)
		map.MapDeleteCreature(MapID, InstanceID, Kanshou2_id)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1753)	--秘道另一端的囚徒看守已经望风而逃！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1010408, 4, "c1010408_OnDie")

--杀掉两个囚徒看守，刷出两个可击杀地物3075209和3075210.

function c1010409_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	map.MapCreateColCreature(MapID, InstanceID, 3075210, 471, 5670, 1337, 1, "")
	Increase_shili(MapID, InstanceID, RoleID, 1, 4, "jinjieshili")

	local sealed = map.GetMapScriptData(MapID, InstanceID, 1, 11)

	if sealed == 0 then
		local Kanshou1_id = map.GetMapScriptData(MapID, InstanceID, 1, 9)
		map.MapDeleteCreature(MapID, InstanceID, Kanshou1_id)

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1753)	--秘道另一端的囚徒看守已经望风而逃！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
	end

end

aux.RegisterCreatureEvent(1010409, 4, "c1010409_OnDie")

--打碎地物3075209消去9422201，打碎地物3075210消去9422401

function c3075209_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 94222)
	if bhave == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 9422201)
	end
end
aux.RegisterCreatureEvent(3075209, 4, "c3075209_OnDie")

function c3075210_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)
	local bhave = role.IsRoleHaveBuff(MapID, InstanceID, RoleID, 94224)
	if bhave == true then
		unit.CancelBuff(MapID, InstanceID, RoleID, 9422401)
	end
end
aux.RegisterCreatureEvent(3075210, 4, "c3075210_OnDie")
