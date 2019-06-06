--怪物死亡计数
function m16_xiaoguai_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local XiaoGuai_Num = map.GetMapScriptData(MapID,InstanceID,1,1)
	map.SetMapScriptData(MapID,InstanceID,1,1,XiaoGuai_Num+1)

	if XiaoGuai_Num+1< 24 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1709)	--您已击杀xx名礼天怪物，击杀24名后苦罗将会出现！
		msg.AddMsgEvent(MsgID, 9, XiaoGuai_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1== 24 then

		map.MapCreateColCreature(MapID, InstanceID, 1005601, 1915, 5303, 1759, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1710)	--苦罗已经出现于坐标[1915, 1759]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1< 40 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1711)	--您已击杀xx名礼天怪物，击杀40名后独足将会出现！
		msg.AddMsgEvent(MsgID, 9, XiaoGuai_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1== 40 then

		map.MapCreateColCreature(MapID, InstanceID, 1005435, 1978, 5303, 1609, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1712)	--独足已经出现于坐标[1978, 1609]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1< 85 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1713)	--您已击杀xx名礼天怪物，击杀85名后血齿将会出现！
		msg.AddMsgEvent(MsgID, 9, XiaoGuai_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1== 85 then

		map.MapCreateColCreature(MapID, InstanceID, 1005487, 1726, 3078, 1478, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1714)	--血齿已经出现于坐标[1726, 1478]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1< 150 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1715)	--您已击杀xx名礼天怪物，击杀150名后敛魂将会出现！
		msg.AddMsgEvent(MsgID, 9, XiaoGuai_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1== 150 then

		map.MapCreateColCreature(MapID, InstanceID, 1005602, 1478, 1503, 1950, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1716)	--敛魂已经出现于坐标[1478, 1950]！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1< 200 then

		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 26, 1717)	--您已击杀xx名礼天怪物，击杀200名后擒龙战将和玄寂将会出现！
		msg.AddMsgEvent(MsgID, 9, XiaoGuai_Num+1)
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	elseif XiaoGuai_Num+1== 200 then

		map.MapCreateColCreature(MapID, InstanceID, 1005433, 1330, 3265, 2399, 1)
		map.MapCreateColCreature(MapID, InstanceID, 1005434, 1434, 3265, 2357, 1)
		local MsgID = msg.BeginMsgEvent()
		msg.AddMsgEvent(MsgID, 101, 1718)	--擒龙战将和受伤的玄寂已经出现！
		msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)

	end
end

aux.RegisterCreatureEvent(1005421, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005422, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005423, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005425, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005426, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005427, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005428, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005429, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005430, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005431, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005479, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005480, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005481, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005482, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005483, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005484, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005485, 4, "m16_xiaoguai_OnDie")
aux.RegisterCreatureEvent(1005486, 4, "m16_xiaoguai_OnDie")
