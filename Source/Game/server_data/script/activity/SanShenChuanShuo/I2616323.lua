

function I2616323_OnUse(MapID, InstanceID, TypeID, RoleID)
	--根据几率为玩家添加不同数量的仙币[1350018],并随机播放广播
	local Rand = math.random(1,10010);
	if Rand <= 6000 then
		local AddNum = 2;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,5);		
		if Rand_2 == 1 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100062);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为10分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 10)
			else
				msg.AddMsgEvent(MsgID, 102, 100061);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100064);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为10分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 10)
			else
				msg.AddMsgEvent(MsgID, 102, 100063);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		elseif Rand_2 == 3 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100066);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为10分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 10)
			else
				msg.AddMsgEvent(MsgID, 102, 100065);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>6000 and Rand<=8000 then
		local AddNum = 6;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,5);
		if Rand_2 == 1 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100062);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 5)
			else
				msg.AddMsgEvent(MsgID, 102, 100061);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100064);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 5)
			else
				msg.AddMsgEvent(MsgID, 102, 100063);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		elseif Rand_2 == 3 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100066);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 5)
			else
				msg.AddMsgEvent(MsgID, 102, 100065);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>8000 and Rand<=9600 then
		local AddNum = 5;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,5);
		if Rand_2 == 1 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100062);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为3分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 3)
			else
				msg.AddMsgEvent(MsgID, 102, 100061);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100064);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为3分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 3)
			else
				msg.AddMsgEvent(MsgID, 102, 100063);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		else
			if math.random(1,1000) == 1 then
				msg.AddMsgEvent(MsgID, 102, 100066);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为3分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 3)
			else
				msg.AddMsgEvent(MsgID, 102, 100065);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 5)
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>9600 and Rand<=9800 then
		local AddNum = 8;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,5);
		if Rand_2 == 1 then
			if math.random(1,1000) < 2 then
				msg.AddMsgEvent(MsgID, 102, 100062);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100061);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) < 2 then
				msg.AddMsgEvent(MsgID, 102, 100064);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100063);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
			end
		else
			if math.random(1,1000) < 2 then
				msg.AddMsgEvent(MsgID, 102, 100066);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100065);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为5分钟
				RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>9800 and Rand<=9940 then
		local AddNum = 10;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,3);
		if Rand_2 == 1 then
			if math.random(1,1000) < 5 then
				msg.AddMsgEvent(MsgID, 102, 100068);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100067);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为2分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
				else
					msg.DispatchBroadcast(MsgID,-1,-1,-1);
				end				
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) < 5 then
				msg.AddMsgEvent(MsgID, 102, 100070);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100069);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为2分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
				else
					--msg.DispatchBroadcast(MsgID,-1,-1,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
				end
			end
		else
			if math.random(1,1000) < 5 then
				msg.AddMsgEvent(MsgID, 102, 100071);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100072);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为2分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 2)
				else
					--msg.DispatchBroadcast(MsgID,-1,-1,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
				end
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>9940 and Rand<=9980 then
		local AddNum = 20;
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, AddNum, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,3);
		if Rand_2 == 1 then
			if math.random(1,1000) < 10 then
				msg.AddMsgEvent(MsgID, 102, 100068);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100067);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 1)
				else
					--msg.DispatchBroadcast(MsgID,-1,-1,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
				end				
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) < 10 then
				msg.AddMsgEvent(MsgID, 102, 100070);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100069);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 1)
				else
					--msg.DispatchBroadcast(MsgID,-1,-1,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
				end
			end
		else
			if math.random(1,1000) < 10 then
				msg.AddMsgEvent(MsgID, 102, 100071);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				--msg.DispatchBroadcast(MsgID,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
				RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
			else
				msg.AddMsgEvent(MsgID, 102, 100072);
				msg.AddMsgEvent(MsgID, 2, RoleID);
				msg.AddMsgEvent(MsgID, 9, AddNum);
				if math.random(1,5) <= 2 then
					--msg.DispatchBroadcast(MsgID,MapID,InstanceID,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, MapID, InstanceID, -1, RoleID, DPR_INMAP, DPR_INTEAM, TypeID, 1)
				else
					--msg.DispatchBroadcast(MsgID,-1,-1,-1);
					--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为1分钟
					RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 1)
				end
			end
		end
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, AddNum);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>9980 and Rand<=9995 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, 50, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,3);
		if Rand_2 == 1 then
			if math.random(1,1000) < 50 then
				msg.AddMsgEvent(MsgID, 102, 100074);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			else
				msg.AddMsgEvent(MsgID, 102, 100073);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			end
		elseif Rand_2 == 2 then
			if math.random(1,1000) < 50 then
				msg.AddMsgEvent(MsgID, 102, 100076);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			else
				msg.AddMsgEvent(MsgID, 102, 100075);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			end
		else
			if math.random(1,1000) < 50 then
				msg.AddMsgEvent(MsgID, 102, 100078);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			else
				msg.AddMsgEvent(MsgID, 102, 100077);
				msg.AddMsgEvent(MsgID, 2, RoleID);
			end
		end
		--msg.DispatchBroadcast(MsgID,-1,-1,-1);
		--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为0分钟
		RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 0)
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, 50);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	elseif Rand>9995 and Rand<=10000 then
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, 100, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		local Rand_2 = math.random(1,2);
		if Rand_2 == 1 then
			if math.random(1,100) < 15 then
				local MsgID_2 = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID_2, 102, 100080);
				msg.AddMsgEvent(MsgID_2, 2, RoleID);
				msg.AddMsgEvent(MsgID_2, 9, 100);
				--msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为0分钟
				RunBroad(MsgID_2, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 0)
			end
			msg.AddMsgEvent(MsgID, 102, 100079);
			msg.AddMsgEvent(MsgID, 2, RoleID);
			msg.AddMsgEvent(MsgID_2, 9, 100);
		else
			if math.random(1,100) < 15 then
				local MsgID_2 = msg.BeginMsgEvent();
				msg.AddMsgEvent(MsgID_2, 102, 100082);
				msg.AddMsgEvent(MsgID_2, 2, RoleID);
				msg.AddMsgEvent(MsgID_2, 9, 100);
				--msg.DispatchBroadcast(MsgID_2,-1,-1,-1);
				--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为0分钟
				RunBroad(MsgID_2, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 0)
			end
			msg.AddMsgEvent(MsgID, 102, 100081);
			msg.AddMsgEvent(MsgID, 2, RoleID);
			msg.AddMsgEvent(MsgID_2, 9, 100);
		end
		--msg.DispatchBroadcast(MsgID,-1,-1,-1);
		--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为0分钟
		RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 0)
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, 100);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	else
		local Rand_2 = math.random(1,2);
		local Rand_3 = math.random(51,99);
		role.AddRoleItem(MapID, InstanceID, RoleID, 1350018, Rand_3, -1, 8, 420);
		local MsgID = msg.BeginMsgEvent();
		if Rand_2 == 1 then
			msg.AddMsgEvent(MsgID, 102, 100079);
			msg.AddMsgEvent(MsgID, 2, RoleID);
			msg.AddMsgEvent(MsgID_2, 9, Rand_3);
		else
			msg.AddMsgEvent(MsgID, 102, 100081);
			msg.AddMsgEvent(MsgID, 2, RoleID);
			msg.AddMsgEvent(MsgID_2, 9, Rand_3);
		end
		--msg.DispatchBroadcast(MsgID,-1,-1,-1);
		--同样由使用仙币礼包事件激发的两条广播之间最小时间间隔为0分钟
		RunBroad(MsgID, -1, -1, -1, RoleID, DPR_ALL, DPR_INTEAM, TypeID, 0)
		local MsgID_1 = msg.BeginMsgEvent();
		msg.AddMsgEvent(MsgID_1, 13, 0);
		msg.AddMsgEvent(MsgID_1, 1, 2729);
		msg.AddMsgEvent(MsgID_1, 4, 2616323);		
		msg.AddMsgEvent(MsgID_1, 4, 1350018);
		msg.AddMsgEvent(MsgID_1, 9, Rand_3);
		msg.DispatchRoleMsgEvent(RoleID, MsgID_1);
	end		
end

--检测物品是否可使用
function I2616323_CanUse(MapID, InstanceID, TypeID, RoleID)
	local bRet, bIgnore = 0, false
	--判断背包空闲空间是否足够
	local FreeSize = role.GetBagFreeSize(RoleID);
	if(FreeSize < 3) then
		--提示玩家背包空间不足
		bRet = 40;
		return bRet, bIgnore
	end
		return 0, false

end


--注册玩家使用仙币礼包事件--
aux.RegisterItemEvent(2616323, 1, "I2616323_OnUse")
aux.RegisterItemEvent(2616323, 0, "I2616323_CanUse")