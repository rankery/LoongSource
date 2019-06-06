function m16_sanjiang_OnDie(MapID, InstanceID, TargetID, TargetTypeID, RoleID)

	local Sanjiang_Num = map.GetMapScriptData(MapID,InstanceID,1,2)
	local Boss_num = map.GetMapScriptData(MapID,InstanceID,1,3)
	local Tuwei = math.floor(Boss_num/100) % 10

	local Huanlingtai_Pos = {}
	Huanlingtai_Pos[1] = {id=3030424, x=0, y=0, z=0}
	Huanlingtai_Pos[2] = {id=3030425, x=875, y=17541, z=926}
	Huanlingtai_Pos[3] = {id=3030426, x=891, y=19303, z=778}
	Huanlingtai_Pos[4] = {id=3030427, x=903, y=21061, z=599}
	Huanlingtai_Pos[5] = {id=3030428, x=0, y=0, z=0}
	Huanlingtai_Pos[6] = {id=3030429, x=497, y=16951, z=963}
	Huanlingtai_Pos[7] = {id=3030430, x=492, y=18711, z=798}
	Huanlingtai_Pos[8] = {id=3030431, x=506, y=20473, z=641}

	if TargetTypeID==1005603 then

		map.SetMapScriptData(MapID,InstanceID,1,2,Sanjiang_Num+1)
		local this_num=Sanjiang_Num % 10 + 1
		if this_num< 3 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1719)	--您已击杀xx名烛阴三将，击杀3名后烛阴鬼母将会出现！
			msg.AddMsgEvent(MsgID, 9, this_num)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		elseif this_num== 3 then
			map.MapCreateColCreature(MapID, InstanceID, 1005453, 698, 11589, 1802, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1720)	--烛阴鬼母已经出现于坐标[698, 1802]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end

	elseif TargetTypeID==1005604 then

		map.SetMapScriptData(MapID,InstanceID,1,2,Sanjiang_Num+10)
		local this_num=math.floor(Sanjiang_Num/10) % 10 + 1
		if this_num< 3 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1721)	--您已击杀xx名辟疟三将，击杀3名后辟疟将会出现！
			msg.AddMsgEvent(MsgID, 9, this_num)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		elseif this_num== 3 then
			map.MapCreateColCreature(MapID, InstanceID, 1005454, 862, 13431, 1112, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1722)	--辟疟已经出现于坐标[862, 1112]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end

	elseif TargetTypeID==1005605 then

		local index = math.floor(Sanjiang_Num/100) % 10 + math.floor(Sanjiang_Num/1000) % 10 + 1 + Tuwei
		map.MapCreateCreature(MapID, InstanceID, Huanlingtai_Pos[index+1].id, Huanlingtai_Pos[index+1].x, Huanlingtai_Pos[index+1].y, Huanlingtai_Pos[index+1].z)

		map.SetMapScriptData(MapID,InstanceID,1,2,Sanjiang_Num+100)
		local this_num=math.floor(Sanjiang_Num/100) % 10 + 1
		if this_num< 3 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1723)	--您已击杀xx名荼尾三将，击杀3名后荼尾将会出现！
			msg.AddMsgEvent(MsgID, 9, this_num)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		elseif this_num== 3 then
			map.MapCreateColCreature(MapID, InstanceID, 1005455, 922, 22233, 471, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1724)	--荼尾已经出现于坐标[922, 471]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end

	elseif TargetTypeID==1005606 then

		local index = math.floor(Sanjiang_Num/100) % 10 + math.floor(Sanjiang_Num/1000) % 10 + 1 + Tuwei
		map.MapCreateCreature(MapID, InstanceID, Huanlingtai_Pos[index+1].id, Huanlingtai_Pos[index+1].x, Huanlingtai_Pos[index+1].y, Huanlingtai_Pos[index+1].z)

		map.SetMapScriptData(MapID,InstanceID,1,2,Sanjiang_Num+1000)
		local this_num=math.floor(Sanjiang_Num/1000) % 10 + 1
		if this_num< 3 then
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 26, 1725)	--您已击杀xx名朱廉三将，击杀3名后朱廉将会出现！
			msg.AddMsgEvent(MsgID, 9, this_num)
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		elseif this_num== 3 then
			map.MapCreateColCreature(MapID, InstanceID, 1005456, 476, 22233, 470, 1)
			local MsgID = msg.BeginMsgEvent()
			msg.AddMsgEvent(MsgID, 101, 1726)	--朱廉已经出现于坐标[476, 470]！
			msg.DispatchMapMsgEvent(MsgID, MapID, InstanceID)
		end

	end

end

aux.RegisterCreatureEvent(1005603, 4, "m16_sanjiang_OnDie")
aux.RegisterCreatureEvent(1005604, 4, "m16_sanjiang_OnDie")
aux.RegisterCreatureEvent(1005605, 4, "m16_sanjiang_OnDie")
aux.RegisterCreatureEvent(1005606, 4, "m16_sanjiang_OnDie")
