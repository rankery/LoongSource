-- 怪物进入战斗触发示警
function c1008102_OnEnterCombat(MapID, InstanceID, CreatureID)

    local Temp = map.GetMapScriptData(MapID, InstanceID, 1, 16)

	if Temp~=1 then
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1799)	--摧元阵将在30分钟后消失，请尽快获取其中的困魔绳以对抗首领叱涅！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

		map.SetMapScriptData(MapID, InstanceID, 1, 16, 1)
	end
end

aux.RegisterCreatureEvent(1008102, 2, "c1008102_OnEnterCombat")





